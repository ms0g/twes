#include <getopt.h>
#include <memory.h>
#include "server.h"

int main(int argc, char *argv[]) {
    int opt;
    int port;
    const char *usage = "Usage: ./twes -p [port] path/root/";
    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
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
    else {
        init_server(port, argv[optind]);
    }
    return 0;
}
