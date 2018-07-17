#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include "http.h"
#include "tweslib.h"

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
static const char *response_header = "%s %s\r\nServer: twes/1.0\r\nDate: %s\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n";

http_request_t *init_http_request(char *buf) {
    http_request_t *req = (http_request_t *) tws_calloc(sizeof(http_request_t));
    req->headers = (char *) tws_calloc(BUFLEN);

    sscanf(buf, "%s %s %s", req->method, req->path, req->protocol);

    if (strcmp(req->path, "/") == 0)
        strcpy(req->path, "/index.html");

    strcpy(req->headers, buf);
    return req;

}

void send_http_response(char *buf, int client_socket, http_request_t *request, FILE *file, char *status) {
#define NUM_STATUS (sizeof(status_codes) / sizeof(char *))

    char *mime = "text/html";
    long len;

    if(file)
        mime = get_mime_type(&request->path[1]);

    char *st, response_data[BUFLEN];

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
        bzero(response_data, sizeof(response_data));
        fseek(file, 0L, SEEK_END);
        len = ftell(file);
        fseek(file, 0L, SEEK_SET);
        if (!is_image(mime))
            fread(response_data, 1, len, file);

    } else
        len = strlen(response_data);

    sprintf(buf, response_header, request->protocol, st, get_gmt(), len, mime);
    strcat(buf, response_data);
    write(client_socket, buf, strlen(buf));

    if (file) {
        if (is_image(mime)) {
            while (!feof(file)) {
                fread(buf, 1, sizeof(buf), file);
                write(client_socket, buf, sizeof(buf));
                bzero(buf, sizeof(buf));
            }
        }
        fclose(file);
    }


#undef is_image
}


void clean_http_request(http_request_t *req) {
    free(req->headers);
    req->headers = NULL;
    free(req);
    req = NULL;
}
