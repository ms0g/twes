#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


void handle_shutdown(int sig);
void error(char* msg);
void read_in(int socket, char* buf);
int open_listener_socket(void);
void bind_to_port(int socket, int port);
int catch_signal(int sig, void (*handler)(int));
void init_server(int port, const char* path);
void reuse(int socket);
void response(int socket, const char* path, char* buf);


