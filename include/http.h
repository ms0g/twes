#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFLEN 8096
#define ERR404 "404 Not Found"
#define ERR405 "405 Method Not Allowed"
#define ERR500 "500 Internal Server Error"
#define STATE200 "200 Ok"
#define DEFAULTMIME "text/html"

typedef struct{
	char method[BUFLEN];
	char path[BUFLEN];
	char protocol[BUFLEN];

}request;

char* http_header(char* protocol, char* state, long len, const char* mime, char* buf);
void http_parse(int socket, char* buf, request* req);
void http_response(int socket, const char* path, char* buf, request* req);

#endif
