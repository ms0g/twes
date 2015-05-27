#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


void handle_shutdown(int sig);
void error(char* msg);
void read_in(int socket, char* buf);
int open_listener_socket(void);
void bind_to_port(int socket, int port);
int catch_signal(int sig, void (*handler)(int));
void init_server(int port, const char* path);
void reuse(int socket);
void destroy(void* ptr);
#endif

