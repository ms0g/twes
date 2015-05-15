/*
 * =====================================================================================
 *
 *       Filename:  twebserv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2015 12:39:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  @gurpinars 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "server.h"


int main ( int argc, char *argv[] )
{
	int opt;
	int port = NULL;
	const char* usage = "Usage: ./twebserv -p [port] path/root/";
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }
	if (port < 0 || argv[optind] == NULL || strlen(argv[optind]) == 0)
		printf("%s\n", usage);
	else
		init_server(port, argv[optind]);
	
	return 0;
}
