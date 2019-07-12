#ifndef DATA_H
#define DATA_H

#include "memcached.h"

#define DATA_SIZE 4096
#define MAX_FNAME 256

typedef struct chunk
{
    int _NOT_USED;
    char data[DATA_SIZE];
    char key[MAX_FNAME];
    int ind;
} chunk;

/* Creates empty chunk */
chunk chunk_create(char *key, memcached *m);

/* Initializes chunk */
void chunk_init(chunk *, char *key, memcached *);

/* Writes data into chunk */
int chunk_write(chunk *, const void *, int, memcached *);

/* Reads data from chunk to buf */
int chunk_read(chunk *c, int, char *, int, memcached *);

/* Return chunk with given inode */
chunk chunk_mmch_getchunk(char *key, memcached *m);

#endif // !DATA_H