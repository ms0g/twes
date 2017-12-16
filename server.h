#ifndef TWES_SERVER_H
#define TWES_SERVER_H

#include <stdio.h>
#include <stdlib.h>

static void HTTP_ERROR(char *buf, int connectfd, const char *res_header, const char *protocol,
                       const char *err, const char *err_str, const char *mime);
/**
 * Shutdown server
 */
static void handle_shutdown(int sig);

/**
 * Read the socket
 */
static void read_in(int connectfd, char *buf);

/**
 * Open the listener socket
 */
static int open_listener_socket(void);

/**
 * Associate the socket with an IP address and port number
 */
static void bind_to_port(int listenerfd, int port);

/**
 * Catch the interrupt signal
 * */
static int catch_signal(int sig, void (*handler)(int));

/**
 * Initialize server
 */
void init_server(int port, const char *path);

#endif //TWES_SERVER_H

