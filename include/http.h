#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFLEN 8096


typedef struct{
	char method[BUFLEN];
	char path[BUFLEN];
	char protocol[BUFLEN];

}request;

char* http_state(char* protocol, char* state, long len, const char* mime, char* buf);
void http_parse(int socket, char* buf, request* req);
void http_response(int socket, const char* path, char* buf, request* req);

#endif
