#ifndef TWES_SERVER_H
#define TWES_SERVER_H

#include <stdio.h>
#include "http.h"
#include "tweslib.h"

typedef struct {
    int daemonize;
} options_t;

/** Options  */
options_t opts;

/** Log file */
FILE *logfd;

FILE *file;

/** Socket and file descriptors */
int server_socket, client_socket;

/** Buffer keeping request headers */
char *buf;

/** Http Request */
http_request_t *request;

/**
 * System error
 */
void error(char *msg) __NORETURN;

/**
 * Shutdown server
 */
static void handle_shutdown(int sig) __NORETURN;

/**
 * Read the socket
 */
static void read_in(int client_socket, char *buf);

/**
 * Open the listener socket
 */
static int open_socket(void);

/**
 * Associate the socket with an IP address and port number
 */
static void bind_to_port(int server_socket, int port);

/**
 * Catch the interrupt signal
 */
static int catch_signal(int sig, void (*handler)(int));

/**
 * Initialize server
 */
void init_server(int port, char *path);

#endif //TWES_SERVER_H

