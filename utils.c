#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"


char *get_mime_type(const char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot)
        return "text/plain";
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
    else
        return "text/plain";
}


char *get_time(type t) {
    struct tm *t_info;
    static char date[50];

    time_t rawtime = time(NULL);
    switch (t) {
        case GMT:
            t_info = gmtime(&rawtime);
            strftime(date, sizeof(date), "%a, %d %b %Y %T GMT", t_info);
            break;
        case CURRENT:
            t_info = localtime(&rawtime);
            strftime(date, sizeof(date), "%d/%b/%Y:%T", t_info);
            break;
    }

    return date;
}


int fd_isreg(const char *filename) {
    struct stat st;

    if (stat(filename, &st))
        return -1;

    return (st.st_mode & S_IFMT) == S_IFREG ? 0 : -1;
}





