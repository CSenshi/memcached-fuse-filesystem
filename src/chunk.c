#include "chunk.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

int chunk_create(memcached *m)
{
    chunk *new_chunk = malloc(sizeof(chunk));
    chunk_init(new_chunk, m);

    int res = memcached_add_struct(m, int_to_str(new_chunk->inode), new_chunk, sizeof(struct chunk), 0, MM_CHUNK);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    return new_chunk->inode;
}

void chunk_init(chunk *chunk, memcached *m)
{
    // fill with zeros
    memset(chunk, 0, sizeof(struct chunk));

    chunk->_NOT_USED = -1;
    chunk->inode = get_next_index(m);
    chunk->ind = 0;
    chunk->next_chunk_inode = -1;
}

int chunk_write(chunk *c, void *data, int size, memcached *m)
{
    if (c->ind + size < DATA_SIZE)
    {
        memcpy(c->data + c->ind, data, size);
        c->ind += size;
    }
    else
    {
        // ToDo: Implement linked list chunk
    }

    int res = memcached_replace_struct(m, int_to_str(c->inode),
                                       c, sizeof(struct chunk), 0, MM_CHUNK);
    return 0;
}

/* Writes data into chunk */
char *chunk_read(int inode, memcached *m)
{
    // TODO IMPORTANT : CHANGE READING!!!!!!!
    mm_data_info info = memcached_get(m, int_to_str(inode));

    chunk *c = malloc(sizeof(struct chunk));
    memcpy(c, info.value, sizeof(struct chunk));

    return c->data;
}

chunk *chunk_mmch_getchunk(int inode, memcached *m)
{
    char *key = int_to_str(inode);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    chunk *c = malloc(sizeof(struct chunk));
    memcpy(c, info.value, sizeof(struct chunk));

    return c;
}