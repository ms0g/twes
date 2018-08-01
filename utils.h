#ifndef TWES_LIB_H
#define TWES_LIB_H

#define OUT(fd, opts) ((opts).daemonize ? (fd):stdout)

#define ALLOC(buffer, type, size)               \
if(((buffer) = (type *) malloc(size)) == NULL)  \
    perror("unable to allocate memory");


/**
 * Get mime type of file.
 * Default html
 */
char *get_mime_type(const char *);

/**
 * Get Greenwich Mean Time
 */
char *get_gmt();

/**
 * Check out if it's a regular file
 */
int fd_isreg(const char *);

#endif //TWES_LIB_H
