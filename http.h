#ifndef TWES_HTTP_H
#define TWES_HTTP_H

#define BUFLEN 8096

/** Response header template */
extern const char *res_header_tmpl;

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
 * Clean request header
 */
void clean_http_request(http_request_t *req);

/**
 * Create http error
 */
void http_error(int connectfd, http_request_t *request, char *err, const char *mime);


#endif //TWES_HTTP_H
