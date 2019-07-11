#include "chunk.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

int chunk_create(chunk *c, memcached *m)
{
    chunk_init(c, m);

    int res = memcached_add_struct(m, int_to_str(c->inode), c, sizeof(struct chunk), 0, MM_CHN);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    return c->inode;
}

void chunk_init(chunk *chunk, memcached *m)
{
    // fill with zeros
    memset(chunk, 0, sizeof(struct chunk));

    chunk->_NOT_USED = -1;
    chunk->inode = get_next_index(m);
    chunk->ind = 0;
}

/* Writes data into chunk */
int chunk_write(chunk *c, const void *data, int size, memcached *m)
{
    int writeen_bytes = c->ind + size > DATA_SIZE ? DATA_SIZE - c->ind : size;

    memcpy(c->data + c->ind, data, writeen_bytes);
    c->ind += writeen_bytes;
    int res = memcached_replace_struct(m, int_to_str(c->inode),
                                       c, sizeof(struct chunk), 0, MM_CHN);
    return writeen_bytes;
}

int chunk_read(chunk *c, int off_set, char *buf, int size, memcached *m)
{
    int read_bytes = (off_set + size) > c->ind ? (c->ind - off_set) : size;
    memcpy(buf, c->data + off_set, DATA_SIZE);
    return read_bytes;
}

chunk chunk_mmch_getchunk(int inode, memcached *m)
{
    char *key = int_to_str(inode);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    chunk c;
    memcpy(&c, info.value, sizeof(struct chunk));

    return c;
}