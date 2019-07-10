#include "content.h"
#include "chunk.h"
#include "utils.h"

chunk *_content_get_chunk(int n, content *cn, memcached *m);

int content_create(memcached *m)
{
    content *cn = malloc(sizeof(struct content));
    content_init(cn, m);

    if (!cn->DIRR_inode[0])
        return -1;

    int res = memcached_add_struct(m, int_to_str(cn->inode), cn, sizeof(struct content), 0, MM_CON);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    return cn->inode;
}

void content_init(content *cn, memcached *m)
{
    // fill with zeros
    memset(cn, 0, sizeof(struct content));

    // fill with ones
    cn->_NOT_USED = -1;

    // get next identificator
    cn->inode = get_next_index(m);

    // set size to zero
    cn->size = 0;

    // set initial adresses to 0
    memset(cn->DIRR_inode, 0, sizeof(int) * C_IND1);
    memset(cn->IND1_inode, 0, sizeof(int) * C_IND1);
    memset(cn->IND2_inode, 0, sizeof(int) * C_IND2);
    memset(cn->IND3_inode, 0, sizeof(int) * C_IND3);

    cn->DIRR_inode[0] = chunk_create(m);
}

int content_read(content *cn, int off_t, int size, char *buf, memcached *m)
{
    if (cn->size < off_t)
        return 0;

    // Read Data From Direct Blocks
    if (off_t < C_DIRR * DATA_SIZE)
    {
        int dirr_ind = off_t / C_DIRR, ind = off_t % C_DIRR;
        chunk *c = chunk_mmch_getchunk(cn->DIRR_inode[dirr_ind], m);

        // while ()
    }
}

int content_write(content *cn, int off_t, int size, char *buf, memcached *m)
{
    int dirr_ind = off_t / C_DIRR, ind = off_t % C_DIRR;
    chunk *c = _content_get_chunk(dirr_ind, cn, m);
    return chunk_write(c, buf, size, m);
}
int content_append(content *cn, int size, char *buf, memcached *m)
{
    return content_write(cn, cn->size, size, buf, m);
}

int content_read_full_chunk(content *cn, int ch_num, char *buf, memcached *m)
{
    chunk *ch = _content_get_chunk(ch_num, cn, m);
    memcpy(buf, ch->data, ch->ind);
    return ch->ind;
}

content *content_mmch_getcontent(int inode, memcached *m)
{
    char *key = int_to_str(inode);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    content *cn = malloc(sizeof(struct content));
    memcpy(cn, info.value, sizeof(struct content));

    return cn;
}

chunk *_content_get_chunk(int n, content *cn, memcached *m)
{
    if (n < C_DIRR)
    {
        if (!cn->DIRR_inode[n])
            cn->DIRR_inode[n] = chunk_create(m);
        return chunk_mmch_getchunk(cn->DIRR_inode[n], m);
    }

    return NULL;
}
