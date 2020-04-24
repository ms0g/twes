#ifndef TWES_LIB_H
#define TWES_LIB_H

#define OUT(fd, opts) ((opts).daemonize ? (fd):stdout)

typedef enum time_t {
    GMT,
    CURRENT
} srv_time_t;


/**
 * Get mime type of file.
 * Default html
 */
char *get_mime_type(const char *filename);

/**
 * Get Greenwich Mean Time
 */
char *get_time(srv_time_t t);

/**
 * Check out if it's a regular file
 */
int fd_isreg(const char *filename);

#endif //TWES_LIB_H
