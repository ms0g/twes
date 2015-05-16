/*
 * =====================================================================================
 *
 *       Filename:  http.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/16/2015 10:05:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  @gurpinars, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "http.h"
#include "utils.h"


const char* error404 = "<html><body><h1>404 Not Found</h1></body></html>";
const char* error500 = "<html><body><h1>500 Internal Server Error</h1></body></html>";
const char* error405 = "<html><body><h1>405 Method Not Allowed</h1></body></html>";


char* http_state(char* protocol,char* state,long len,const char* mime,char* buf){	
	sprintf(buf,"%s %s\nServer: twebserv/1.0\nContent-Length: %ld\nContent-Type: %s\n\n",protocol, state, len, mime);
	return buf;
}


void http_parse(int socket,char* buf, request* req){
	char method[BUFLEN];
	char path[BUFLEN];
	char protocol[BUFLEN];	
	
	sscanf(buf,"%s %s %s", method, path, protocol);
	strcpy(req->method, method);
	strcpy(req->path, path);
	strcpy(req->protocol, protocol);

	if(strcmp(req->method, "GET")){
		buf = http_state(req->protocol,"405 Method Not Allowed", strlen(error405),"text/html", buf);	
		write(socket, buf, strlen(buf));
        write(socket, error405, strlen(error405));
	}
	
	if(!strcmp(req->path,"/"))
		strcpy(req->path,"/index.html");
}

	
void http_response(int socket, const char* path, char* buf, request* req){
	int file;
	long len;
	
	if(chdir(path) == -1){
		buf = http_state(req->protocol,"500 Internal Server Error",strlen(error500),"text/html",buf);
		write(socket, buf, strlen(buf));
		write(socket, error500,strlen(error500));
	}
	else{
		const char* ext = get_filename_ext(&(req->path[1]));
		const char* mime = lookup(ext);

		if((file = open(&(req->path[1]), O_RDONLY)) == -1){ 
			buf = http_state(req->protocol,"404 Not Found", strlen(error404), mime, buf);
			write(socket, buf, strlen(buf));
			write(socket,error404, strlen(error404));
		}
		else{
			len = lseek(file, (off_t)0, SEEK_END);
			lseek(file, (off_t)0, SEEK_SET);
			buf = http_state(req->protocol,"200 OK", len, mime,buf);
			write(socket, buf, strlen(buf));

			while (read(file, buf, BUFLEN) > 0)
				write(socket, buf, BUFLEN);
		}
	}
}

