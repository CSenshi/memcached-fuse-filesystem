#ifndef DATA_H
#define DATA_H

#include "memcached.h"

#define DATA_SIZE 1024

typedef struct chunk
{
    int _NOT_USED;

    int next_chunk_inode;
    char data[DATA_SIZE];
    int ind;
    int inode;
} chunk;

/* Creates empty chunk */
int chunk_create(memcached *);

/* Initializes chunk */
void chunk_init(chunk *, memcached *);

/* Writes data into chunk */
int chunk_write(chunk *, void *, int, memcached *);

/* Writes data into chunk */
char *chunk_read(int, memcached *);

/* Return chunk with given inode */
chunk *chunk_mmch_getchunk(int inode, memcached *m);

#endif // !DATA_H