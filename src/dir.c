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

    int res = memcached_add_struct(m, path, new_dir, sizeof(struct dir), 0, MM_DIR);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    char *par_path = get_par_path(path);

    if (strcmp(path, "/"))
        dir_append(par_path, new_dir, m);

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

    d->chunk_inode = chunk_create(m);
    d->mode = mode;
}

void dir_append(char *path, dir *new_dir, memcached *m)
{
    char *str = _create_dir_entry_str(new_dir);
    dir *par_dir = dir_mmch_getdir(path, m);
    chunk *c = chunk_mmch_getchunk(par_dir->chunk_inode, m);

    chunk_write(c, str, strlen(str), m);
    free(par_dir);
    free(c);
}

dir *dir_mmch_getdir(const char *path, memcached *m)
{
    char *key = strdup(path);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    dir *dir = malloc(sizeof(struct dir));
    memcpy(dir, info.value, sizeof(struct dir));

    // free(key);
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
        ind += len1;

        dc->n++;
        dc->arr = realloc(dc->arr, sizeof(char *) * dc->n);
        dc->arr[dc->n - 1] = strdup(val1);

        free(cp_len1);
        free(val1);
    }
    // free(s);
    return dc;
}

char *_create_dir_entry_str(dir *dir)
{
    // value 1 : inode
    char *val1 = dir->dir_name;
    int len1 = strlen(val1);

    int to_alloc = len1 + OFF_LEN + 1;
    char *req = malloc(to_alloc);
    memset(req, '0', to_alloc);

    // fill inode parameter
    char *len1_str = int_to_str(len1);
    memcpy(req + OFF_LEN - strlen(len1_str), len1_str, strlen(len1_str));
    memcpy(req + OFF_LEN, val1, len1);

    req[to_alloc - 1] = '\0';
    return req;
}