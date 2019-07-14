#ifndef CONTENT_H
#define CONTENT_H

#include "chunk.h"
#include "memcached.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct content
{
    int _NOT_USED;
    char path[MAX_FNAME];
    int size;
} content;

void content_init(content *, const char *, memcached *);

int content_read(content *, int, int, char *, memcached *);

int content_write(content *, int, int, const char *, memcached *);

int content_append(content *, int, char *, memcached *);

int content_read_full_chunk(content *, int, char *, memcached *);

void content_free(content *, memcached *);

int content_setxattr(content *cn, const char *name, const char *value, int size, memcached *m);

int content_getxattr(content *cn, const char *name, char *buf, size_t size, memcached *m);

int content_remxattr(content *cn, const char *name, memcached *m, parse_val *pv);

int content_listxattr(content *cn, char *list, size_t size, memcached *m);

#endif // !CONTENT_H