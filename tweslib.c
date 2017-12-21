#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include "tweslib.h"
#include "server.h"


void error(char *msg) {
    fprintf(OUT(logfd, opts), "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}


char *get_mime(char *filename) {
    char *dot = strrchr(filename, '.') + 1;
    if (!dot)
        return "text/plain";
    else if (!strcasecmp(dot, "html"))
        return "text/html";
    else if (!strcasecmp(dot, "jpg"))
        return "image/jpeg";
    else if (!strcasecmp(dot, "css"))
        return "text/css";
    else if (!strcasecmp(dot, "gif"))
        return "image/gif";
    else if (!strcasecmp(dot, "ico"))
        return "image/x-icon";
    else if (!strcasecmp(dot, "png"))
        return "image/png";
    else if (!strcasecmp(dot, "js"))
        return "application/javascript";
}


void *tmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        error("unable to allocate memory");
    }
    return p;
}
