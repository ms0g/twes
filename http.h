#ifndef TWES_HTTP_H
#define TWES_HTTP_H

#define BUFLEN 8096

typedef struct {
    char method[10];
    char protocol[10];
    char path[BUFLEN];
    char *headers;
} http_request_t;

/**
 * Initialize http request
 */
http_request_t *init_http_request(char *buf);

/**
 *
 * Clean http request
 */
void clean_http_request(http_request_t *req);


/**
 * Send Http response
 */
void send_http_response(char *buf, int client_socket, http_request_t *request, FILE *html_file, char *status);


#endif //TWES_HTTP_H
