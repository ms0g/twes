#ifndef TWES_LIB_H
#define TWES_LIB_H

#define LOG(request, status, opts) { \
    if ((opts).verbose) \
        printf("%s",(request)->headers); \
    else printf("%s %s %s %s\n",(request)->method,(request)->path,(request)->protocol,status); \
}

void error(char *msg);
char *get_mime(char *filename);
void *tmalloc(size_t size);

#endif //TWES_LIB_H
