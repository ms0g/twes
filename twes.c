#include <getopt.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"

int main(int argc, char *argv[]) {
    int opt;
    int port=0;
    char cwd[BUFLEN];
    pid_t chpid;
    int verbose = 0;
    int daemonize = 0;

    const char *usage = "Usage: ./twes -p [port] path/html/files\nOptions: -d [run as daemon]\n\t -v [print full log]";
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

    opts = (options){.verbose=verbose, .daemonize=daemonize};

    if (port <= 0 || argv[optind] == NULL || strlen(argv[optind]) == 0) {
        printf("%s\n", usage);
        exit(EXIT_FAILURE);
    }

    if (opts.daemonize) {
        if ((chpid = fork()) > 0)
            // Kill parent process
            exit(EXIT_SUCCESS);
        else if (chpid < 0)
            exit(EXIT_FAILURE);

        // Change the file mode mask
        umask(0);
        // Get current working dir
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/twes.log");
        // Create log file
        logfd = fopen(cwd, "w+");

        // Set new session id for child processs
        if (setsid() < 0)
            exit(EXIT_FAILURE);

        //Change current working dir
        if (chdir("/") < 0)
            exit(EXIT_FAILURE);

        // Close out the standard file descriptors
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    init_server(port, argv[optind]);
    return 0;
}
