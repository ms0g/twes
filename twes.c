#include <getopt.h>
#include <memory.h>
#include <stdlib.h>
#include "server.h"

int main(int argc, char *argv[]) {
    int opt;
    int port;
    int verbose = 0;
    int daemonize = 0;

    const char *usage = "Usage: ./twes -p [port] path/html/files";
    while ((opt = getopt(argc, argv, "dvhp:")) != -1) {
        switch (opt) {
            // -h
            case 'h':
                printf("%s\n", usage);
                return 0;

                // -p port
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'd':
                daemonize = 1;
                break;
            default:
                break;
        }
    }

    options opts = {
            .verbose=verbose,
            .daemonize=daemonize
    };
    if (port < 0 || argv[optind] == NULL || strlen(argv[optind]) == 0)
        printf("%s\n", usage);
    else {
        init_server(port, argv[optind], opts);
    }
    return 0;
}
