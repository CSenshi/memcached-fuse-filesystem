#ifndef DATA_H
#define DATA_H

#include "memcached.h"

#define DATA_SIZE 1024

typedef struct chunk
{
    int inode;
    int ind;
    char data[DATA_SIZE];
    int next_chunk_inode;
} chunk;

int chunk_create(memcached *);
void chunk_init(chunk *, memcached *);

#endif // !DATA_H