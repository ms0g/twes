#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include "http.h"
#include "tweslib.h"

http_request *init_http_request(char *buf) {
    http_request *req = (http_request *) tmalloc(sizeof(http_request));
    req->headers = (char *) tmalloc(BUFLEN);

    sscanf(buf, "%s %s %s", req->method, req->path, req->protocol);

    if (strcmp(req->path, "/") == 0)
        strcpy(req->path, "/index.html");

    strcpy(req->headers, buf);
    return req;

}


void http_error(http_request *request, char *buf, int connectfd, const char *res_header,
                const char *status, const char *status_html, const char *mime) {
    sprintf(buf, res_header, request->protocol, status, strlen(status_html), mime);
    write(connectfd, buf, strlen(buf));
    write(connectfd, status_html, strlen(status_html));
}

void clean_http_request(http_request *req) {
    free(req->headers);
    req->headers = NULL;
    free(req);
    req = NULL;
}