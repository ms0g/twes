#ifndef TWES_SERVER_H
#define TWES_SERVER_H

#include <stdio.h>
#include "http.h"
#include "utils.h"

typedef struct {
    int daemonize;
} options_t;

/** Options  */
options_t opts;

/** Log file */
FILE *logfd;

/**
 * System error
 */
void error(char *msg) __attribute__ ((noreturn));

/**
 * Shutdown server
 */
static void harakiri(int sig) __attribute__ ((noreturn));

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

/**
 * Check status
 */
static char *check_status(http_request_t *request);

#endif //TWES_SERVER_H

