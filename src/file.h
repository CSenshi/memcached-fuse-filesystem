#ifndef FILE_H
#define FILE_H

#include "content.h"
#include "memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct file
{
    int _NOT_USED;
    char file_name[MAX_FNAME];
    content ex_cn;
    content cn;
    mode_t mode;
} file;

int file_create(const char *, mode_t, memcached *m);

void file_init(file *, const char *, mode_t, memcached *);

int file_write(file *, const char *, size_t, off_t, memcached *);

int file_read(file *, char *, size_t, off_t, memcached *);

int file_get_size(file *, memcached *m);

int file_setxattr(file *, const char *name, const char *value, size_t size, memcached *m);

int file_getxattr(file *, const char *name, char *buf, size_t size, memcached *m);

#endif // FILE_H