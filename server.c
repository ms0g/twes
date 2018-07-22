#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include "server.h"


void error(char *msg) {
    fprintf(OUT(logfd, opts), "%s: %s\n", msg, strerror(errno));
    exit(1);
}


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


static void handle_shutdown(int sig) {
    if (server_socket) close(server_socket);
    if (client_socket) close(client_socket);
    if (file) fclose(file);
    if (logfd) fclose(logfd);
    if (request) clean_http_request(request);
    if (buf) {
        free(buf);
        buf = NULL;
    }

    exit(0);
}


static int catch_signal(int sig, void (*handler)(int)) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction(sig, &action, NULL);
}


void init_server(int port, char *path) {
    if (catch_signal(SIGINT, handle_shutdown) == -1)
        error("Can't set the interrupt handler");

    pid_t ch_pid;
    struct sockaddr_in client_addr;
    size_t address_size = sizeof(client_addr);

    server_socket = open_socket();
    bind_to_port(server_socket, port);

    if (listen(server_socket, 10) == -1)
        error("Can't listen");

    buf = (char *) tws_calloc(BUFLEN);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *) &address_size);
        if (client_socket == -1)
            error("Can’t open secondary socket");

        if ((ch_pid = fork()) == 0) {
            read_in(client_socket, buf);

            request = init_http_request(buf);
            strcat(path, &request->path[1]);

            if (strcmp(&request->method[0], "GET") != 0)
                send_http_response(buf, client_socket, request, NULL, "405");
            else if ((file = fopen(path, "rb")) == NULL)
                send_http_response(buf, client_socket, request, NULL, "404");
            else {
                send_http_response(buf, client_socket, request, file, "200");
            }

            close(client_socket);
            if (file)
                fclose(file);
            free(buf);
            free(path);
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



