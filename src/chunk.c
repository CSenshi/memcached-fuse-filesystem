#include "chunk.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

int chunk_create(memcached *m)
{
    chunk *new_chunk = malloc(sizeof(chunk));
    chunk_init(new_chunk, m);

    int res = memcached_add_struct(m, int_to_str(new_chunk->inode), new_chunk, sizeof(chunk));

    return new_chunk->inode;
}

void chunk_init(chunk *chunk, memcached *m)
{
    chunk->inode = get_next_index(m);
    chunk->ind = 0;
    chunk->next_chunk_inode = 0;

    memset(chunk->data, 0, DATA_SIZE);
}