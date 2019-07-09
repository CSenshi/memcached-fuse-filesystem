#include "dir.h"
#include "chunk.h"
#include "utils.h"

#define OFF_LEN 10

char *_create_dir_entry_str(dir *);

int dir_create(const char *path, mode_t mode, memcached *m)
{
    dir *new_dir = malloc(sizeof(dir));

    dir_init(new_dir, path, mode, m);
    if (new_dir->chunk_inode == -1)
        return -1;

    int res = memcached_add_struct(m, int_to_str(new_dir->inode), new_dir, sizeof(dir), 0, MM_DIR);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    dir_append(new_dir->par_inode, new_dir, m);

    return 0;
}

void dir_init(dir *d, const char *path, mode_t mode, memcached *m)
{
    // fill with zeros
    memset(d, 0, sizeof(struct dir));

    d->_NOT_USED = -1;

    //parse
    parse_val prs = parse_path(path);
    memcpy(d->dir_name, prs.arr[prs.n - 1], strlen(prs.arr[prs.n - 1]));

    // get next identifier
    d->inode = get_next_index(m);

    // ToDo: implement subdirectories
    d->par_inode = ROOT_DIR_INODE;

    d->chunk_inode = chunk_create(m);
    d->mode = mode;
}

void dir_append(int par_inode, dir *new_dir, memcached *m)
{
    char *str = _create_dir_entry_str(new_dir);
    dir *par_dir = dir_mmch_getdir(par_inode, m);
    chunk *c = chunk_mmch_getchunk(par_dir->chunk_inode, m);

    chunk_write(c, str, strlen(str), m);
}

dir *dir_mmch_getdir(int inode, memcached *m)
{
    char *key = int_to_str(inode);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    dir *dir = malloc(sizeof(struct dir));
    memcpy(dir, info.value, sizeof(struct dir));

    return dir;
}

dir_childs *dir_get_childs(dir *d, memcached *m)
{
    dir_childs *dc = malloc(sizeof(struct dir_childs));
    memset(dc, 0, sizeof(struct dir_childs));
    dc->n = 0;

    char *s = chunk_read(d->chunk_inode, m);

    int ind = 0;
    while (s[ind] != '\0')
    {
        // get length 1
        char *cp_len1 = malloc(OFF_LEN + 1);
        memcpy(cp_len1, s + ind, OFF_LEN);
        cp_len1[OFF_LEN] = '\0';
        int len1 = str_to_int(cp_len1);
        ind += OFF_LEN;

        // get value 1;
        char *val1 = malloc(len1 + 1);
        memcpy(val1, s + ind, len1);
        val1[len1] = '\0';
        int inode = str_to_int(val1);
        ind += len1;

        // get length 2
        char *cp_len2 = malloc(OFF_LEN + 1);
        memcpy(cp_len2, s + ind, OFF_LEN);
        cp_len2[OFF_LEN] = '\0';
        int len2 = str_to_int(cp_len2);
        ind += OFF_LEN;

        // get value 2
        char *val2 = malloc(len2 + 1);
        memcpy(val2, s + ind, len2);
        val2[len2] = '\0';
        // printf("\n\n AAAAAAAAAAAAAA\n%s\n\n", val2);
        ind += len2;

        dir_entry *entry = malloc(sizeof(struct dir_entry));
        entry->inode = inode;
        entry->name = strdup(val2);

        dc->n++;
        dc->arr = realloc(dc->arr, sizeof(struct dir_entry *) * dc->n);
        dc->arr[dc->n - 1] = entry;

        free(cp_len1);
        free(cp_len2);
        free(val1);
        free(val2);
    }
    return dc;
}

char *_create_dir_entry_str(dir *dir)
{
    // value 1 : inode
    char *val1 = int_to_str(dir->inode);
    int len1 = strlen(val1);

    // value 2 : file name
    char *val2 = dir->dir_name;
    int len2 = strlen(val2);

    int to_alloc = len1 + len2 + 2 * OFF_LEN + 1;
    char *req = malloc(to_alloc);
    memset(req, '0', to_alloc);

    // fill inode parameter
    char *len1_str = int_to_str(len1);
    memcpy(req + OFF_LEN - strlen(len1_str), len1_str, strlen(len1_str));
    memcpy(req + OFF_LEN, val1, len1);

    // fill dir name parameter
    char *len2_str = int_to_str(len2);
    memcpy(req + 2 * OFF_LEN + len1 - strlen(len2_str), len2_str, strlen(len2_str));
    memcpy(req + 2 * OFF_LEN + len1, val2, len2);

    req[to_alloc - 1] = '\0';
    return req;
}