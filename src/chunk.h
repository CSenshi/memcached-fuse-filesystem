#ifndef DATA_H
#define DATA_H

#include "memcached.h"

#define DATA_SIZE 4096

typedef struct chunk
{
    int _NOT_USED;

    char data[DATA_SIZE];
    int ind;
    int inode;
} chunk;

/* Creates empty chunk */
int chunk_create(chunk *, memcached *);

/* Initializes chunk */
void chunk_init(chunk *, memcached *);

/* Writes data into chunk */
int chunk_write(chunk *, const void *, int, memcached *);

/* Reads data from chunk to buf */
int chunk_read(chunk *c, int, char *, int, memcached *);

/* Return chunk with given inode */
chunk chunk_mmch_getchunk(int inode, memcached *m);

#endif // !DATA_H