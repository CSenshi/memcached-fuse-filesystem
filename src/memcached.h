#ifndef MEMCACHED_H
#define MEMCACHED_H

typedef struct memcached
{
    char *addr;
    int port;
    int fd;
} memcached;

void memcached_init(struct memcached *m);

void memcached_exit(struct memcached *m);

#endif // !MEMCACHED_H