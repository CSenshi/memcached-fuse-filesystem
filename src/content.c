#include "content.h"
#include "chunk.h"
#include "utils.h"
#include <math.h>

void _content_get_chunk(int index, content *cn, chunk *c, memcached *m);
void _content_to_str(int n, content *cn, char *buf);
void _create_ex_str(const char *val1, int len1, const char *val2, int len2, char *buf);

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

int content_append(content *cn, int size, const char *buf, memcached *m)
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

    if (!chunk_mmch_getchunk(str, m, c))
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

int content_setxattr(content *cn, const char *name, const char *value, int size, memcached *m)
{
    int to_alloc = 2 * OFF_LEN + strlen(name) + size;
    char str[to_alloc];

    _create_ex_str(name, strlen(name), value, size, str);
    content_append(cn, to_alloc, str, m);
    return 0;
}

int content_getxattr(content *cn, const char *name, char *buf, size_t size, memcached *m)
{
    char s[2 * DATA_SIZE] = {0};
    int read_data = content_read_full_chunk(cn, 0, s, m);
    if (cn->size > DATA_SIZE)
        read_data += content_read_full_chunk(cn, 1, s + DATA_SIZE, m);
    int ind = 0;

    int chunk_ind = 1;
    while (ind < read_data)
    {
        // get length 1
        char cp_len1[OFF_LEN + 1];
        memcpy(cp_len1, s + ind, OFF_LEN);
        cp_len1[OFF_LEN] = '\0';
        int len1 = str_to_int(cp_len1);
        ind += OFF_LEN;

        // get value 1;
        char val1[len1 + 1];
        memcpy(val1, s + ind, len1);
        val1[len1] = '\0';
        ind += len1;

        // get length 2
        char cp_len2[OFF_LEN + 1];
        memcpy(cp_len2, s + ind, OFF_LEN);
        cp_len2[OFF_LEN] = '\0';
        int len2 = str_to_int(cp_len2);
        ind += OFF_LEN;

        // get value 2;
        char val2[len2 + 1];
        memcpy(val2, s + ind, len2);
        val2[len2] = '\0';
        ind += len2;

        if (!strcmp(val1, name))
        {
            if (buf)
                memcpy(buf, val2, size);
            return len2;
        }

        if (DATA_SIZE < ind)
        {
            chunk_ind++;
            memmove(s, s + DATA_SIZE, DATA_SIZE);
            ind -= DATA_SIZE;
            read_data -= DATA_SIZE;
            read_data += content_read_full_chunk(cn, chunk_ind, s + DATA_SIZE, m);
        }
    }
    return -2;
}

int content_remxattr(content *cn, const char *name, memcached *m, parse_val *pv)
{
    char s[2 * DATA_SIZE] = {0};
    int read_data = content_read_full_chunk(cn, 0, s, m);
    if (cn->size > DATA_SIZE)
        read_data += content_read_full_chunk(cn, 1, s + DATA_SIZE, m);
    int ind = 0;

    pv->alloc = 10;
    pv->n = 0;
    pv->arr = malloc(sizeof(char *) * pv->alloc);
    int found = -1;
    int chunk_ind = 1;
    while (ind < read_data)
    {
        pv->n++;
        int cur = ind;
        // get length 1
        char cp_len1[OFF_LEN + 1];
        memcpy(cp_len1, s + ind, OFF_LEN);
        cp_len1[OFF_LEN] = '\0';
        int len1 = str_to_int(cp_len1);
        ind += OFF_LEN;

        // get value 1;
        char val1[len1 + 1];
        memcpy(val1, s + ind, len1);
        val1[len1] = '\0';
        ind += len1;

        // get length 2
        char cp_len2[OFF_LEN + 1];
        memcpy(cp_len2, s + ind, OFF_LEN);
        cp_len2[OFF_LEN] = '\0';
        int len2 = str_to_int(cp_len2);
        ind += OFF_LEN;

        // get value 2;
        char val2[len2 + 1];
        memcpy(val2, s + ind, len2);
        val2[len2] = '\0';
        ind += len2;

        int end = ind;
        pv->arr[pv->n - 1] = malloc(end - cur + 1);
        memcpy(pv->arr[pv->n - 1], s + cur, end - cur);
        pv->arr[pv->n - 1][end - cur] = '\0';

        if (pv->alloc == pv->n)
        {
            pv->alloc += 10;
            pv->arr = realloc(pv->arr, sizeof(char *) * pv->alloc);
        }

        if (!strcmp(val1, name))
            found = pv->n - 1;

        if (DATA_SIZE < ind)
        {
            chunk_ind++;
            memmove(s, s + DATA_SIZE, DATA_SIZE);
            ind -= DATA_SIZE;
            read_data -= DATA_SIZE;
            read_data += content_read_full_chunk(cn, chunk_ind, s + DATA_SIZE, m);
        }
    }
    return found;
}

int content_listxattr(content *cn, char *list, size_t size, memcached *m)
{
    char s[2 * DATA_SIZE] = {0};
    int read_data = content_read_full_chunk(cn, 0, s, m);
    if (cn->size > DATA_SIZE)
        read_data += content_read_full_chunk(cn, 1, s + DATA_SIZE, m);
    int ind = 0;

    int total = 0;
    int chunk_ind = 1;
    while (ind < read_data)
    {
        // get length 1
        char cp_len1[OFF_LEN + 1];
        memcpy(cp_len1, s + ind, OFF_LEN);
        cp_len1[OFF_LEN] = '\0';
        int len1 = str_to_int(cp_len1);
        ind += OFF_LEN;

        // get value 1;
        char val1[len1 + 1];
        memcpy(val1, s + ind, len1);
        val1[len1] = '\0';
        ind += len1;

        if (list)
            memcpy(list + total, val1, len1 + 1);
        total += len1 + 1;

        // get length 2
        char cp_len2[OFF_LEN + 1];
        memcpy(cp_len2, s + ind, OFF_LEN);
        cp_len2[OFF_LEN] = '\0';
        int len2 = str_to_int(cp_len2);
        ind += OFF_LEN;

        // get value 2;
        char val2[len2 + 1];
        memcpy(val2, s + ind, len2);
        val2[len2] = '\0';
        ind += len2;
        if (DATA_SIZE < ind)
        {
            chunk_ind++;
            memmove(s, s + DATA_SIZE, DATA_SIZE);
            ind -= DATA_SIZE;
            read_data -= DATA_SIZE;
            read_data += content_read_full_chunk(cn, chunk_ind, s + DATA_SIZE, m);
        }
    }

    return total;
}

int content_create_symlink(content *cn, const char *to_link, memcached *m)
{
    int bytes = content_append(cn, strlen(to_link), to_link, m);
    return bytes;
}

int content_read_symlink(content *cn, char *buf, size_t size, memcached *m)
{
    return content_read(cn, 0, size, buf, m);
}

void _content_to_str(int n, content *cn, char *buf)
{
    char *s = int_to_str(n);
    memcpy(buf, s, strlen(s));
    memcpy(buf + strlen(s), cn->path, strlen(cn->path) + 1);
    free(s);
}

void _create_ex_str(const char *val1, int len1, const char *val2, int len2, char *buf)
{
    // val1
    char *len1_str = int_to_str(len1);
    memset(buf, '0', OFF_LEN);
    memcpy(buf + OFF_LEN - strlen(len1_str), len1_str, strlen(len1_str));
    memcpy(buf + OFF_LEN, val1, len1);

    // val2
    char *len2_str = int_to_str(len2);
    memset(buf + OFF_LEN + len1, '0', OFF_LEN);
    memcpy(buf + OFF_LEN + len1 + OFF_LEN - strlen(len2_str), len2_str, strlen(len2_str));
    memcpy(buf + OFF_LEN + len1 + OFF_LEN, val2, len2);
}