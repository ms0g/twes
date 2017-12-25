#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "server.h"
#include "tweslib.h"

const char *res_header = "%s %s\nServer: twes/1.0\nContent-Length: %ld\nContent-Type: %s\n\n";


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

    if (logfd)
        fclose(logfd);

    if (request) {
        clean_http_request(request);
    }

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

            request = init_http_request(buf);
            memset(buf, 0, BUFLEN);

            if (strcmp(&request->method[0], "GET")) {
                http_error(connectfd,request, buf, res_header, ERROR405, DEFAULTMIME);
                LOG(request, ERR405, opts, logfd)
            } else if (chdir(dir) == -1) {
                http_error(connectfd,request, buf, res_header, ERROR500, DEFAULTMIME);
                LOG(request, ERR500, opts, logfd)
            } else if ((file = open(&request->path[1], O_RDONLY)) == -1) {
                http_error(connectfd,request, buf, res_header, ERROR404, DEFAULTMIME);
                LOG(request, ERR404, opts, logfd)
            } else {
                mime = get_mime(&request->path[1]);
                len = lseek(file, (off_t) 0, SEEK_END);
                lseek(file, (off_t) 0, SEEK_SET);
                sprintf(buf, res_header, request->protocol, STATE200, len, mime);
                write(connectfd, buf, strlen(buf));

                while (read(file, buf, BUFLEN) > 0)
                    write(connectfd, buf, BUFLEN);
                close(file);
                LOG(request, STATE200, opts, logfd)
            }
            close(connectfd);
            free(buf);
            clean_http_request(request);
            exit(EXIT_SUCCESS);
        } else if (ch_pid > 0) {
            close(connectfd);
        } else if (ch_pid < 0)
            error("system call fork");
    }
}



