#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "server.h"
#include "tweslib.h"

#define BUFLEN 8096
#define ERR404 "404 Not Found"
#define ERR405 "405 Method Not Allowed"
#define ERR500 "500 Internal Server Error"
#define STATE200 "200 Ok"
#define DEFAULTMIME "text/html"

static const char *error405 = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
static const char *error404 = "<html><body><h1>404 Not Found</h1></body></html>";
static const char *error500 = "<html><body><h1>500 Internal Server Error</h1></body></html>";
static const char *res_header = "%s %s\nServer: twes/1.0\nContent-Length: %ld\nContent-Type: %s\n\n";

int listenerfd, connectfd, file; // file descriptors
char *buf; // buffer keeping http request and response

static void HTTP_ERROR(char *buf, int connectfd, const char *res_header,
                       const char *protocol, const char *err, const char *err_str,
                       const char *mime) {
    sprintf(buf, res_header, protocol, err, strlen(err_str), mime);
    write(connectfd, buf, strlen(buf));
    write(connectfd, err_str, strlen(err_str));
}


static void read_in(int connectfd, char *buf) {
    int c;
    if ((c = recv(connectfd, buf, BUFLEN, 0)) >= 0) {
        if (c == 0)
            error("peer has closed");
    } else error("failed to read browser HTTP_Request");
}


static int open_listener_socket(void) {
    int s;
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error("Can't open socket");
    return s;
}


static void bind_to_port(int listenerfd, int port) {
    struct sockaddr_in name;

    name.sin_family = PF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    if (setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(int)) == -1)
        error("Can't set the reuse option on the listenerfd");

    if (bind(listenerfd, (struct sockaddr *) &name, sizeof(name)) == -1)
        error("Can't bind to listenerfd");
}


static void handle_shutdown(int sig) {
    if (listenerfd)
        close(listenerfd);

    if (connectfd)
        close(connectfd);

    if (file)
        close(file);

    if (buf) {
        free(buf);
        buf = NULL;
    }
    fprintf(stderr, "Interrupted!\n");
    exit(0);
}


static int catch_signal(int sig, void (*handler)(int)) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction(sig, &action, NULL);
}


void init_server(int port, const char *dir) {
    pid_t ch_pid;
    char *mime;
    long len;
    struct sockaddr_in client_addr;
    size_t address_size = sizeof(client_addr);

    if (catch_signal(SIGINT, handle_shutdown) == -1)
        error("Can't set the interrupt handler");

    listenerfd = open_listener_socket();
    bind_to_port(listenerfd, port);

    if (listen(listenerfd, 10) == -1)
        error("Can't listen");

    buf = (char *) tmalloc(BUFLEN);

    while (1) {
        connectfd = accept(listenerfd, (struct sockaddr *) &client_addr, (socklen_t *) &address_size);
        if (connectfd == -1)
            error("Can’t open secondary socket");
        if ((ch_pid = fork()) == 0) {

            close(listenerfd);
            read_in(connectfd, buf);

            char method[BUFLEN], protocol[BUFLEN], path[BUFLEN];
            sscanf(buf, "%s %s %s", method, path, protocol);

            if (strcmp(path, "/") == 0)
                strcpy(path, "/index.html");

            memset(buf, 0, BUFLEN);

            if (strcmp(&method[0], "GET")) {
                HTTP_ERROR(buf, connectfd, res_header, protocol, ERR405, error405, DEFAULTMIME);
            } else if (chdir(dir) == -1) {
                HTTP_ERROR(buf, connectfd, res_header, protocol, ERR500, error500, DEFAULTMIME);
            } else if ((file = open(&path[1], O_RDONLY)) == -1) {
                HTTP_ERROR(buf, connectfd, res_header, protocol, ERR404, error404, DEFAULTMIME);
            } else {
                mime = get_mime(&path[1]);
                len = lseek(file, (off_t) 0, SEEK_END);
                lseek(file, (off_t) 0, SEEK_SET);
                sprintf(buf, res_header, protocol, STATE200, len, mime);
                write(connectfd, buf, strlen(buf));

                while (read(file, buf, BUFLEN) > 0)
                    write(connectfd, buf, BUFLEN);
                close(file);
            }
            close(connectfd);
            free(buf);
            exit(EXIT_SUCCESS);
        } else if (ch_pid > 0) {
            close(connectfd);
        } else if (ch_pid < 0)
            error("system call fork");
    }
}



