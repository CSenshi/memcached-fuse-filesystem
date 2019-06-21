#ifndef MEMCACHED_H
#define MEMCACHED_H

#include <stdio.h>

typedef struct memcached
{
    char *addr;
    int port;
    int fd;
} memcached;

typedef struct mm_data_info
{
    char *key;
    int flags;
    int ttl;
    int size;
} mm_data_info;

void memcached_init(struct memcached *m);

/* Set a key unconditionally */
int memcached_add(struct memcached *m, struct mm_data_info, char *value);

/* Reads a value */
char *memcached_get(struct memcached *m, char *key);

void memcached_exit(struct memcached *m);

#endif // !MEMCACHED_H