#ifndef TWES_HTTP_H
#define TWES_HTTP_H

#define BUFLEN 1024

typedef struct {
    char method[10];
    char version[10];
    char *resource;
    char *headers;
} http_request_t;

/**
 * Initialize the http request
 */
http_request_t *init_http_request(char *buf, char *path);

/**
 *
 * Clean the http request
 */
void clean_http_request(http_request_t *req);


/**
 * Send the Http response
 */
void send_http_response(char *buf, int client_socket, http_request_t *request, FILE *html_file, char *status);


#endif //TWES_HTTP_H
