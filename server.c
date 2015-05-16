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
#include "http.h"


void error(char* msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}		

void read_in(int socket, char* buf, int len){
	int c  = recv(socket, buf, len, 0);
	if(c == 0 || c == -1)
		error("failed to read browser request");
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
	
	char* buf = (char*)malloc(BUFLEN);
	request* req =(request*)malloc(sizeof(request));
	puts("Listening");
	
	while(1){
		int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
		if (connect_d == -1)
			error("Can’t open secondary socket");
		if(!fork()){
			close(listener_d);
			read_in(connect_d, buf, BUFLEN);
			http_parse(connect_d,buf,req);
			http_response(connect_d, path, buf, req);
			close(connect_d);
			free(buf);
			free(req);
			exit(0);
		}
	}
}


inline void reuse(int socket){
	int reuse = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		error("Can't set the reuse option on the socket");

}
