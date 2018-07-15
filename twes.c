#include <getopt.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"

/** print usage */
void echo_usage(void) __NORETURN;

/** run as daemon  */
void init_daemonizing(void);

int main(int argc, char *argv[]) {
    int opt;
    int port = 0;
    int daemonize = 0;
    char path[BUFLEN];

    while ((opt = getopt(argc, argv, "dhp:")) != -1) {
        switch (opt) {
            // -h
            case 'h':
                echo_usage();

                // -p port
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                daemonize = 1;
                break;
            default:
                break;
        }
    }

    opts = (options_t){.daemonize=daemonize};
    strcpy(path, argv[optind]);

    if (port <= 0 || argv[optind] == NULL || strlen(argv[optind]) == 0)
        echo_usage();

    if (opts.daemonize)
        init_daemonizing();

    init_server(port, path);
    return 0;
}


void echo_usage(void){
    const char *usage = "Usage: ./twes -p [port] path/html/files\nOptions: -d [run as daemon]\n\t";
    printf("%s\n",usage);
    exit(1);
}

void init_daemonizing(void) {
    char cwd[BUFLEN];
    pid_t chpid;

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
