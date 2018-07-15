#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include "tweslib.h"


char *get_mime_type(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!strcasecmp(dot, ".html"))
        return "text/html";
    else if (!strcasecmp(dot, ".jpg"))
        return "image/jpeg";
    else if (!strcasecmp(dot, ".css"))
        return "text/css";
    else if (!strcasecmp(dot, ".gif"))
        return "image/gif";
    else if (!strcasecmp(dot, ".ico"))
        return "image/x-icon";
    else if (!strcasecmp(dot, ".png"))
        return "image/png";
    else if (!strcasecmp(dot, ".js"))
        return "application/javascript";
    return "text/html";
}


char *get_gmt() {
    struct tm *gm;
    static char date[50];

    time_t tm = time(NULL);
    gm = gmtime(&tm);
    strftime(date, sizeof(date), "%a, %d %b %Y %T GMT", gm);
    return date;
}


void *tws_calloc(size_t size) {
    void *p = calloc(size, sizeof(char));
    if (!p) {
        perror("unable to allocate memory");
    }
    return p;
}
