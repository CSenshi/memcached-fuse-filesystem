#ifndef CONTENT_H
#define CONTENT_H

#include "memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define C_DIRR 50
#define C_IND1 10
#define C_IND2 30

typedef struct content
{
    int _NOT_USED;
    int inode;
    int size;

    /* Direct Addressing of chunks */
    int DIRR_inode[C_DIRR];

    /* Indirect Addressing of chunks */
    int IND1_inode[C_IND1];
    int IND2_inode[C_IND2];
} content;

int content_create(memcached *);

void content_init(content *, memcached *);

int content_read(content *, int, int, char *, memcached *);

int content_write(content *, int, int, char *, memcached *);

int content_append(content *, int, char *, memcached *);

int content_read_full_chunk(content *, int, char *, memcached *);

content content_mmch_getcontent(int inode, memcached *m);

void content_free(int inode, memcached *m);

#endif // !CONTENT_H