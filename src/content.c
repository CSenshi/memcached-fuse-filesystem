#include "content.h"
#include "chunk.h"
#include "utils.h"
#include <math.h>

void _content_get_chunk(int index, content *cn, chunk *c, memcached *m);
void _content_to_str(int n, content *cn, char *buf);

void content_init(content *cn, const char *path, memcached *m)
{
    // fill with zeros
    memset(cn, 0, sizeof(struct content));

    // fill with ones
    cn->_NOT_USED = -1;

    // set size to zero
    cn->size = 0;

    memcpy(cn->path, path, strlen(path) + 1);
}

int content_read(content *cn, int off_t, int size, char *buf, memcached *m)
{
    if (cn->size < off_t)
        return 0;

    // Read Data From Direct Blocks
    chunk c;
    _content_get_chunk(off_t / DATA_SIZE, cn, &c, m);

    int read_bytes = chunk_read(&c, off_t % DATA_SIZE, buf, size, m);
    int b = read_bytes;
    while (read_bytes < size && c.ind == DATA_SIZE)
    {
        _content_get_chunk((off_t + read_bytes) / DATA_SIZE, cn, &c, m);
        b = chunk_read(&c, 0, buf + read_bytes, size - read_bytes, m);
        read_bytes += b;
    }
    return read_bytes;
}

int content_write(content *cn, int off_t, int size, const char *buf, memcached *m)
{
    if (size == 0)
        return 0;

    chunk c;
    _content_get_chunk(off_t / DATA_SIZE, cn, &c, m);
    int written_bytes = chunk_write(&c, buf, off_t % DATA_SIZE, size, m);

    cn->size = max(off_t + written_bytes, cn->size);
    return written_bytes;
}

int content_append(content *cn, int size, char *buf, memcached *m)
{
    int total = 0, written_bytes = 0;
    while (written_bytes = content_write(cn, cn->size, size - total, buf + total, m),
           written_bytes > 0)
    {
        total += written_bytes;
    }
    return total;
}

int content_read_full_chunk(content *cn, int ch_num, char *buf, memcached *m)
{
    chunk ch;
    _content_get_chunk(ch_num, cn, &ch, m);
    memcpy(buf, ch.data, ch.ind);
    return ch.ind;
}

void _content_get_chunk(int index, content *cn, chunk *c, memcached *m)
{
    char str[300];
    _content_to_str(index, cn, str);
    int total_ind = (cn->size / DATA_SIZE);

    chunk_mmch_getchunk(str, m, c);
    if (c->_NOT_USED)
        return;
    chunk_create(str, c, m);
}

void content_free(content *cn, memcached *m)
{
    int res = -1;
    char str[300];
    for (int i = 0; res != NOT_FOUND; i++)
    {
        _content_to_str(i, cn, str);
        res = memcached_delete(m, str);
    }
    return;
}
void _content_to_str(int n, content *cn, char *buf)
{
    char *s = int_to_str(n);
    memcpy(buf, s, strlen(s));
    memcpy(buf + strlen(s), cn->path, strlen(cn->path) + 1);
    // free(s);
}