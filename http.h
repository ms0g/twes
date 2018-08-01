#ifndef TWES_HTTP_H
#define TWES_HTTP_H

#define BUFLEN 1024

typedef struct {
    char *path;
    FILE *fd;
} file_t;

typedef struct {
    char method[10];
    char protocol[10];
    file_t file;
    char *headers;
} http_request_t;

/**
 * Initialize the http request
 */
http_request_t *init_http_request(char *, char *);

/**
 *
 * Clean the http request
 */
void clean_http_request(http_request_t *);


/**
 * Send the Http response
 */
void send_http_response(char *, int, http_request_t *, char *);


#endif //TWES_HTTP_H
