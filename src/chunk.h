#ifndef DATA_H
#define DATA_H

#include "memcached.h"

#define DATA_SIZE 4096
#define MAX_FNAME 250

typedef struct chunk
{
    int _NOT_USED;
    int ind;
    char data[DATA_SIZE];
    char key[MAX_FNAME];
} chunk;

/* Creates empty chunk */
void chunk_create(char *key, chunk *c, memcached *m);

/* Initializes chunk */
void chunk_init(chunk *, char *key, memcached *);

/* Writes data into chunk */
int chunk_write(chunk *, const void *, int, int, memcached *);

/* Reads data from chunk to buf */
int chunk_read(chunk *c, int, char *, int, memcached *);

/* Return chunk with given inode */
void chunk_mmch_getchunk(char *key, memcached *m, chunk *c);

#endif // !DATA_H