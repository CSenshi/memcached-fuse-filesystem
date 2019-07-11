#ifndef DIR_H
#define DIR_H

#define CHILD_C 100
#define MAX_FNAME 256

#include "memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INIT_INODE 2322
#define ROOT_DIR_INODE (INIT_INODE + 1)

typedef struct dir
{
    int _NOT_USED;
    char dir_name[MAX_FNAME];
    int content_inode;
    mode_t mode;
} dir;

typedef struct dir_childs
{
    int n;
    char **arr;
} dir_childs;

int dir_create(const char *, mode_t, memcached *);

void dir_init(dir *, const char *, mode_t, memcached *);

void dir_append(char *, dir *, memcached *);

int dir_rmdir(const char *path, memcached *m);

dir *dir_mmch_getdir(const char *, memcached *);

dir_childs dir_get_childs(dir *d, memcached *m);

#endif // !DIR_H
