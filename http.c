#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include "http.h"
#include "tweslib.h"
#include "server.h"

static const char *error_codes[] = {
        "404",
        "405",
        "500"
};

static const char *status[] = {
        "404 Not Found",
        "405 Method Not Allowed",
        "500 Internal Server Error"
};

static const char *error_tmpl = "<html><body><h1>%s</h1></body></html>";
const char *res_header_tmpl = "%s %s\nServer: twes/1.0\nDate: %s\nContent-Length: %ld\nContent-Type: %s\n\n";

http_request_t *init_http_request(char *buf) {
    http_request_t *req = (http_request_t *) tw_alloc(sizeof(http_request_t));
    req->headers = (char *) tw_alloc(BUFLEN);

    sscanf(buf, "%s %s %s", req->method, req->path, req->protocol);

    if (strcmp(req->path, "/") == 0)
        strcpy(req->path, "/index.html");

    strcpy(req->headers, buf);
    return req;

}


void http_error(int connectfd, http_request_t *request, char *err, const char *mime) {

#define NUM_ERRORS (sizeof(error_codes) / sizeof(char *))

    char *st, err_htm[100], r_header[300];

    for (int i = 0; i < NUM_ERRORS; ++i) {
        if (strcmp(err, error_codes[i]) == 0) {
            sprintf(err_htm, error_tmpl, status[i]);
            st = status[i];
            break;
        }
    }
    sprintf(r_header, res_header_tmpl, request->protocol, st, get_gmt(), strlen(err_htm), mime);
    write(connectfd, r_header, strlen(r_header));
    write(connectfd, err_htm, strlen(err_htm));
    LOG(request, st, opts, logfd)
}

void clean_http_request(http_request_t *req) {
    free(req->headers);
    req->headers = NULL;
    free(req);
    req = NULL;
}
