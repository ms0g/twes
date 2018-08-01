#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include "server.h"


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
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(int)) == -1)
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


static char *check_status(http_request_t *request) {
    if (strcmp(&request->method[0], "GET") != 0)
        return "405";
    else if (fd_isreg(request->file.path) < 0)
        return "404";
    return "200";


}

http_request_t *request;
int server_socket, client_socket;
char *buf;

void init_server(int port, char *path) {
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

    buf = (char *) tws_malloc(BUFLEN * sizeof(char));

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
            request = init_http_request(buf, path);

            char *status = check_status(request);
            if (strcmp(status, "200") == 0)
                request->file.fd = fopen(request->file.path, "rb");

            send_http_response(buf, client_socket, request, status);


            // always clean
            close(client_socket);
            if (request->file.fd)
                fclose(request->file.fd);
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


void error(char *msg) {
    fprintf(OUT(logfd, opts), "%s: %s\n", msg, strerror(errno));
    exit(1);
}

