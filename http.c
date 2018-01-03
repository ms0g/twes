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


void http_error(int connectfd, http_request *request, char *buf, const char *res_header,
                int err, const char *mime) {
    char *status;
    char *err_htm;

    switch (err) {
        case ERROR404:
            status = ERR404;
            err_htm = "<html><body><h1>404 Not Found</h1></body></html>";
            break;
        case ERROR405:
            status = ERR405;
            err_htm = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
            break;
        case ERROR500:
            status = ERR500;
            err_htm = "<html><body><h1>500 Internal Server Error</h1></body></html>";
            break;
        default:
            break;
    }
    sprintf(buf, res_header, request->protocol, status, get_gmt(), strlen(err_htm), mime);
    write(connectfd, buf, strlen(buf));
    write(connectfd, err_htm, strlen(err_htm));
}

void clean_http_request(http_request *req) {
    free(req->headers);
    req->headers = NULL;
    free(req);
    req = NULL;
}
