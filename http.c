#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include "http.h"
#include "utils.h"

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


static void clean_http_headers(header_t **);
static void insert_headers(header_t **, char *, char *);
static void parse_headers(char *, http_request_t *);
static char *get_header_key(char *);
static void parse_request_line(char *, http_request_t *);


http_request_t *init_http_request(char *buf, char *path) {
    char res[100];
    bzero(res, 100);
    http_request_t *request;

    // create request struct
    ALLOC(request, http_request_t, sizeof(http_request_t))
    request->file.path = strdup(path);

    // parse the request header fields. e.g.Accept-Language: en
    parse_headers(buf, request);

    // parse the request line. e.g GET / HTTP/1.1\r\n
    parse_request_line(buf, request);

    if (strcmp(request->resource, "/") == 0)
        strcpy(res, "/index.html");
    // append the res at the end of the resource
    request->file.path = (char *) realloc(request->file.path,
                                          strlen(request->file.path) + (
                                                  strlen(res) != 0 ? strlen(res) : strlen(request->resource)) + 1);
    strcat(request->file.path, strlen(res) != 0 ? &res[1] : &request->resource[1]);

    return request;

}

void send_http_response(char *buf, int client_socket, http_request_t *request, char *status) {
#define NUM_STATUS (sizeof(status_codes) / sizeof(char *))

    char *mime = "text/html";
    long len;
    char *st;
    char *response_data;

    // allocate the buffer for the response
    ALLOC(response_data, char, 100 * sizeof(char))

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

    if (request->file.fd) {
        // clean the response buffer
        bzero(response_data, sizeof(response_data));
        mime = get_mime_type(&request->file.path[1]);

        // get the file size
        fseek(request->file.fd, 0L, SEEK_END);
        len = ftell(request->file.fd);
        fseek(request->file.fd, 0L, SEEK_SET);
        // reallocate the response buffer for the size of the file
        response_data = (char *) realloc(response_data, (size_t) len + 1);
        if (!is_image(mime))
            fread(response_data, 1, (size_t) len, request->file.fd);
    } else
        len = strlen(response_data);

    // create the response header and send
    sprintf(buf, response_header, request->protocol, st, get_time(GMT), len, mime);
    strcat(buf, response_data);
    write(client_socket, buf, strlen(buf));

    // images must be sent in chunks
    if (request->file.fd && is_image(mime)) {
        while (!feof(request->file.fd)) {
            fread(response_data, 1, sizeof(response_data), request->file.fd);
            write(client_socket, response_data, sizeof(response_data));
            bzero(buf, sizeof(response_data));
        }
    }
    free(response_data);
}

#undef is_image

static void parse_request_line(char *buf, http_request_t *request) {
    strcpy(request->method, strtok(buf, " "));
    strcpy(request->resource, strtok(NULL, " "));
    strcpy(request->protocol, strtok(NULL, " "));
}


static void parse_headers(char *buf, http_request_t *request) {
    char *found = strtok(buf, "\r\n");
    while (found != NULL) {
        char *pos = strchr(found, ':');
        if (pos) {
            char *key = get_header_key(found);
            char *value = strdup(&pos[2]);
            insert_headers(&request->headers, key, value);
        }
        found = strtok(NULL, "\r\n");
    }

}


static void insert_headers(header_t **headers, char *key, char *value) {
    if (*headers == NULL) {
        *headers = (header_t *) malloc(sizeof(header_t));
        (*headers)->key = key;
        (*headers)->value = value;
    } else {
        insert_headers(&(*headers)->next, key, value);
    }
}

static char *get_header_key(char *str) {
    int ind;
    size_t size = strlen(str);

    for (int i = 0; i < strlen(str); ++i) {
        if (str[i] == ':') {
            ind = i;
            break;
        }
    }

    char key[size];
    bzero(key, size);
    strncpy(key, str, (size_t) ind);
    return strdup(key);

}


void clean_http_request(http_request_t *request) {
    clean_http_headers(&request->headers);
    free(request->file.path);
    if (request->file.fd)
        fclose(request->file.fd);
    free(request);
    request = NULL;
}


static void clean_http_headers(header_t **headers) {
    if (*headers == NULL)
        return;
    clean_http_headers(&(*headers)->next);
    free((*headers)->key);
    free((*headers)->value);
    free(*headers);
    *headers = NULL;

}
