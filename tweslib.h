#ifndef TWES_LIB_H
#define TWES_LIB_H

#define __NORETURN __attribute__ ((noreturn))

#define OUT(fd, opts) ((opts).daemonize ? (fd):stdout)


/**
 * Get mime type of file.
 * Default html
 */
char *get_mime_type(char *filename);

/**
 * Get Greenwich Mean Time
 */
char *get_gmt();

/**
 * Allocate
 */
void *tws_calloc(size_t size);

#endif //TWES_LIB_H
