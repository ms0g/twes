#ifndef TWES_HTTP_H
#define TWES_HTTP_H


typedef struct {
    char *path;
    FILE *fd;
} file_t;

typedef struct header_s {
    char *key;
    char *value;
    struct header_s *next;
} header_t;

typedef struct {
    char method[10];
    char protocol[10];
    char resource[100];
    file_t file;
    header_t *headers;
} http_request_t;

/**
 * Initialize the http request
 */
http_request_t *init_http_request(char * buf, char *path);

/**
 *
 * Clean the http request
 */
void clean_http_request(http_request_t *request);


/**
 * Send the Http response
 */
void send_http_response(char *buf, int client_socket, http_request_t *request, char *status);


#endif //TWES_HTTP_H
