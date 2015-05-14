/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/05/2015 05:33:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  @gurpinars, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "server.h"
#include "utils.h"

void error(char* msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}		


void read_in(int socket, char* buf, int len){
	int c  = recv(socket, buf, len, 0);
	if(c == 0 || c == -1)
		error("failed to read browser request");
}


void response(int socket, const char* path, char* buf){
	int file;
	long len;
	char method[BUFLEN];
	char request_path[BUFLEN];
	char protocol[BUFLEN];	

	sscanf(buf,"%s %s %s", method, request_path, protocol);
	
	if(strncmp(method,"GET",4) && strncmp(method,"get",4))
		error("Only simple GET operation permitted");	
	
	if(!strcmp(request_path,"/"))
		strcpy(request_path,"/index.html");

	if(chdir(path) == -1) 
		error("Can't Change to directory");
	
	const char* ext = get_filename_ext(&request_path[1]);
	const char* mime = lookup(ext);
			
	if((file = open(&request_path[1], O_RDONLY)) == -1) 
		error("failed to open file");
	
	len = lseek(file, (off_t)0, SEEK_END);
	lseek(file, (off_t)0, SEEK_SET);
	
	sprintf(buf,"%s 200 OK\nServer: twebserv/1.0\nContent-Length: %ld\nContent-Type: %s\n\n",protocol, len, mime);
	write(socket, buf, strlen(buf));
	
	while (read(file, buf, BUFLEN) > 0) {
		write(socket, buf, BUFLEN);
	}
}


int open_listener_socket(void){
	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
		error("Can't open socket");
	return s;
}	


void bind_to_port(int socket, int port){
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	reuse(socket);
	int c = bind (socket, (struct sockaddr *)&name, sizeof(name));
	if (c == -1)
		error("Can't bind to socket");
}	


void handle_shutdown(int listener_d){
	if(listener_d)
		close(listener_d);
	fprintf(stderr, "Bye!\n");
	exit(0);
}	


int catch_signal(int sig, void (*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	return sigaction (sig, &action, NULL);
}		


void init_server(int port, const char* path){
	int listener_d;

	if (catch_signal(SIGINT, handle_shutdown) == -1)
		error("Can't set the interrupt handler");
	listener_d = open_listener_socket();
	bind_to_port(listener_d, port);
	if (listen(listener_d, 10) == -1)
		error("Can't listen");
	
	struct sockaddr_in client_addr;	
	unsigned int address_size = sizeof(client_addr);
	
	char* buf = malloc(BUFLEN);
	puts("Listening");
	
	while(1){
		int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
		if (connect_d == -1)
			error("Can’t open secondary socket");
		if(!fork()){
			close(listener_d);
			read_in(connect_d, buf, BUFLEN);
			response(connect_d, path, buf);
			close(connect_d);
			free(buf);
			exit(0);
		}
	}
}


inline void reuse(int socket){
	int reuse = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		error("Can't set the reuse option on the socket");

}
