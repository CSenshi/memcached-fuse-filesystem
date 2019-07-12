#include "chunk.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

void chunk_create(char *key, chunk *c, memcached *m)
{
    chunk_init(c, key, m);

    int res = memcached_set_struct(m, key, c, sizeof(struct chunk), 0, MM_CHN);
}

void chunk_init(chunk *chunk, char *key, memcached *m)
{
    // fill with zeros
    memset(chunk, 0, sizeof(struct chunk));

    chunk->_NOT_USED = -1;
    memcpy(chunk->key, key, strlen(key) + 1);
    chunk->ind = 0;
}

/* Writes data into chunk */
int chunk_write(chunk *c, const void *data, int size, memcached *m)
{
    int writeen_bytes = c->ind + size > DATA_SIZE ? DATA_SIZE - c->ind : size;

    memcpy(c->data + c->ind, data, writeen_bytes);
    c->ind += writeen_bytes;
    int res = memcached_replace_struct(m, c->key, c, sizeof(struct chunk), 0, MM_CHN);
    return writeen_bytes;
}

int chunk_read(chunk *c, int off_set, char *buf, int size, memcached *m)
{
    int read_bytes = (off_set + size) > c->ind ? (c->ind - off_set) : size;
    memcpy(buf, c->data + off_set, DATA_SIZE);
    return read_bytes;
}

void chunk_mmch_getchunk(char *key, memcached *m, chunk *c)
{
    mm_data_info info;
    memcached_get(m, key, &info);

    // copy given value into
    if (info.value)
        memcpy(c, info.value, sizeof(struct chunk));
    else
        memset(c, 0, sizeof(struct chunk));
}