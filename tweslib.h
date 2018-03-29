#ifndef TWES_LIB_H
#define TWES_LIB_H

#define __NORETURN __attribute__ ((noreturn))

#define OUT(fd, opts) ((opts).daemonize ? (fd):stdout)

#define LOG(request, status, opts, fd) { \
    if ((opts).verbose) \
        fprintf(OUT(fd, opts),"%s",(request)->headers); \
    else fprintf(OUT(fd, opts),"%s %s %s %s\n",(request)->method,(request)->path,(request)->protocol,status); \
}


/**
 * Get mime type of file
 */
char *get_mime_type(char *filename);

/**
 * Get Greenwich Mean Time
 */
char *get_gmt();

/**
 * Allocate
 */
void *tw_alloc(size_t size);

#endif //TWES_LIB_H
