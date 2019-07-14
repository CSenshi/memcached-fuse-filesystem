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
    content ex_cn;
    content cn;
    int is_linked;
    uid_t uid;
    gid_t gid;
    mode_t mode;
} dir;

typedef struct dir_childs
{
    int _alloc;
    int n;
    char **arr;
} dir_childs;

int dir_create(const char *, mode_t, uid_t, gid_t, memcached *);

void dir_init(dir *, const char *, mode_t, uid_t, gid_t, memcached *);

void dir_append(dir *par_dir, const char *elem, memcached *m);

int dir_rmdir(const char *path, memcached *m);

void dir_mmch_getdir(const char *, memcached *, dir *);

void dir_get_childs(dir *d, memcached *m, dir_childs *dc);

int dir_setxattr(dir *, const char *name, const char *value, size_t size, memcached *m);

int dir_getxattr(dir *, const char *name, char *buf, size_t size, memcached *m);

int dir_remxattr(dir *, const char *name, memcached *m);

int dir_listxattr(dir *d, char *list, size_t size, memcached *m);

int dir_create_symlink(dir *dir, const char *to_link, memcached *m);

int dir_read_symlink(dir *d, char *buf, size_t size, memcached *m);

int dir_change_mode(dir *d, mode_t mode, memcached *m);

#endif // !DIR_H
