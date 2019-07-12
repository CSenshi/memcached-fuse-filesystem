#ifndef DIR_H
#define DIR_H

#include "content.h"
#include "memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct dir
{
    int _NOT_USED;
    char dir_name[MAX_FNAME];
    mode_t mode;
    content cn;
} dir;

typedef struct dir_childs
{
    int _alloc;
    int n;
    char **arr;
} dir_childs;

int dir_create(const char *, mode_t, memcached *);

void dir_init(dir *, const char *, mode_t, memcached *);

void dir_append(dir *par_dir, char *elem, memcached *m);

int dir_rmdir(const char *path, memcached *m);

dir dir_mmch_getdir(const char *, memcached *);

dir_childs dir_get_childs(dir *d, memcached *m);

#endif // !DIR_H
