#include <getopt.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <assert.h>
#include "http.h"
#include "utils.h"

#define BUFLEN 1024

static void error(char *) __attribute__ ((noreturn));
static void harakiri(int) __attribute__ ((noreturn));
static void read_in(int, char *);
static int open_socket(void);
static void bind_to_port(int, int);
static int catch_signal(int, void (*)(int));
static char *get_status(http_request_t *);
static void echo_usage(void) __attribute__ ((noreturn));
static void init_daemonizing(void);


static void read_in(int client_socket, char *buf) {
    ssize_t c;
    if ((c = recv(client_socket, buf, BUFLEN, 0)) >= 0) {
        if (c == 0)
            error("peer has closed");
    } else error("failed to read browser HTTP_Request");
}


static int open_socket(void) {
    int s;
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error("Can't open socket");
    return s;
}


static void bind_to_port(int server_socket, int port) {
    struct sockaddr_in server_address;

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
        error("Can't set the reuse option on the server_socket");

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1)
        error("Can't bind to server_socket");
}


static int catch_signal(int sig, void (*handler)(int)) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction(sig, &action, NULL);
}


static char *get_status(http_request_t *request) {
    if (strcmp(&request->method[0], "GET") != 0)
        return "405";
    else if (fd_isreg(request->file.path) < 0)
        return "404";
    return "200";
}


typedef struct {
    int daemonize;
} options_t;

options_t opts;

http_request_t *request;
FILE *logfd;
int server_socket;
int client_socket;
char *buf;

int main(int argc, char *argv[]) {
    int opt;
    int port = 0;
    int daemonize = 0;

    while ((opt = getopt(argc, argv, "dvhp:")) != -1) {
        switch (opt) {
            // -h
            case 'h':
                echo_usage();

                // -p port
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                daemonize = 1;
                break;
            default:
                break;
        }
    }

    opts = (options_t) {.daemonize=daemonize};

    assert(port);

    if (port <= 0 || argv[optind] == NULL || strlen(argv[optind]) == 0)
        echo_usage();

    if (opts.daemonize)
        init_daemonizing();

    if (catch_signal(SIGINT, harakiri) == -1)
        error("Can't set the interrupt handler");

    pid_t ch_pid;
    struct sockaddr_in client_addr;
    size_t address_size = sizeof(client_addr);

    // create socket
    server_socket = open_socket();
    // bind the socket to port
    bind_to_port(server_socket, port);

    // listen
    if (listen(server_socket, 10) == -1)
        error("Can't listen");

    buf = (char *) calloc(BUFLEN, sizeof(char));

    while (1) {
        // wait for connection.If it happens,create a new socket and keep on communication over it
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *) &address_size);
        if (client_socket == -1)
            error("Can’t open secondary socket");

        // create a child process
        if ((ch_pid = fork()) == 0) {

            // read bytes into buf from socket
            read_in(client_socket, buf);

            // create request struct
            request = init_http_request(buf, argv[optind]);

            char *status = get_status(request);
            if (strcmp(status, "200") == 0)
                request->file.fd = fopen(request->file.path, "rb");

            send_http_response(buf, client_socket, request, status);

            // always clean
            close(client_socket);
            free(buf);
            clean_http_request(request);
            exit(0);
        } else if (ch_pid < 0)
            error("system call fork");
        else {
            close(client_socket);
            // clean child processes otherwise they will be zombie
            if (waitpid(ch_pid, NULL, 0) < 0)
                error("failed to collect child process");
        }
    }
}

static void init_daemonizing(void) {
    char cwd[BUFLEN];
    pid_t chpid;

    if ((chpid = fork()) > 0)
        // Kill parent process
        exit(EXIT_SUCCESS);
    else if (chpid < 0)
        exit(EXIT_FAILURE);

    // Change the file mode mask
    umask(0);
    // Get current working dir
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/twes.log");

    // Create log file
    logfd = fopen(cwd, "w+");

    // Set new session id for child processs
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    //Change current working dir
    if (chdir("/") < 0)
        exit(EXIT_FAILURE);

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


static void harakiri(int sig) {
    if (server_socket) close(server_socket);
    if (client_socket) close(client_socket);
    if (logfd) fclose(logfd);
    if (request) clean_http_request(request);
    if (buf) {
        free(buf);
        buf = NULL;
    }
    exit(0);
}


static void error(char *msg) {
    fprintf(OUT(logfd, opts), "%s: %s\n", msg, strerror(errno));
    exit(1);
}


static void echo_usage(void) {
    const char *usage = "Usage: ./twes -p [port] path/html/files\nOptions: -d [run as daemon]\n\t";
    printf("%s\n", usage);
    exit(1);
}




