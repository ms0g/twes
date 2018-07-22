#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include "http.h"
#include "tweslib.h"

#define VERSION "0.1"

static const char *status_codes[] = {
        "200",
        "404",
        "405",
        "500"
};

static const char *status_list[] = {
        "200 OK",
        "404 Not Found",
        "405 Method Not Allowed",
        "500 Internal Server Error"
};

static const char *error_html = "<html><body><h1>%s</h1></body></html>";
static const char *response_header = "%s %s\r\n"
                                     "Server: twes/"VERSION
                                     "\r\n"
                                     "Date: %s\r\n"
                                     "Content-Length: %ld\r\n"
                                     "Content-Type: %s\r\n\r\n";


http_request_t *init_http_request(char *buf, char *path) {
    char res[100];

    // find the start and end pointer of the headers
    const char *pStart = strstr(buf, "\r\n");
    const char *pEnd = strstr(buf, "\r\n\r\n");
    // calculate the needed size for the headers
    size_t size = pEnd - &pStart[2];

    // create request struct
    http_request_t *request = (http_request_t *) tws_malloc(sizeof(http_request_t));
    request->resource = strdup(path);
    // allocate the headers. +1 for null-termination
    request->headers = (char *) tws_malloc(size + 1);
    strncpy(request->headers, &pStart[2], size);

    // parse the request line. e.g GET / HTTP/1.1\r\n
    buf = strtok(buf, "\r\n");
    strcpy(request->method, strtok(buf, " "));
    strcpy(res, strtok(NULL, " "));
    strcpy(request->version, strtok(NULL, " "));


    if (strcmp(res, "/") == 0)
        strcpy(res, "/index.html");

    // append the res at the end of the resource
    request->resource = (char *) realloc(request->resource, strlen(request->resource) + sizeof(res));
    strcat(request->resource, &res[1]);

    return request;

}

void send_http_response(char *buf, int client_socket, http_request_t *request, FILE *file, char *status) {
#define NUM_STATUS (sizeof(status_codes) / sizeof(char *))

    char *mime = "text/html";
    long len;
    char *st;

    // allocate the buffer for the response
    char *response_data = (char *) tws_malloc(100 * sizeof(char));

    for (int i = 0; i < NUM_STATUS; ++i) {
        if (strcmp(status, status_codes[i]) == 0) {
            sprintf(response_data, error_html, status_list[i]);
            st = (char *) status_list[i];
            break;
        }
    }
#undef NUM_STATUS

#define is_image(mime)              \
strcmp(mime,"image/jpeg") == 0  ||  \
strcmp(mime,"image/gif") == 0   ||  \
strcmp(mime,"image/png") == 0   ||  \
strcmp(mime,"image/x-icon") == 0

    if (file) {
        // clean the response buffer
        bzero(response_data, sizeof(response_data));
        mime = get_mime_type(&request->resource[1]);

        // get the file size
        fseek(file, 0L, SEEK_END);
        len = ftell(file);
        fseek(file, 0L, SEEK_SET);
        // reallocate the response buffer for the size of the file
        response_data = (char *) realloc(response_data, len + 1);
        if (!is_image(mime))
            fread(response_data, 1, len, file);
    } else
        len = strlen(response_data);

    // create the response header and send
    sprintf(buf, response_header, request->version, st, get_gmt(), len, mime);
    strcat(buf, response_data);
    write(client_socket, buf, strlen(buf));

    // images must be sent in chunks
    if (file && is_image(mime)) {
        while (!feof(file)) {
            fread(response_data, 1, sizeof(response_data), file);
            write(client_socket, response_data, sizeof(response_data));
            bzero(buf, sizeof(response_data));
        }
    }
    free(response_data);
}

#undef is_image

void clean_http_request(http_request_t *req) {
    free(req->headers);
    free(req->resource);
    free(req);
    req = NULL;
}
