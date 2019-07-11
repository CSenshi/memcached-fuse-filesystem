#include "dir.h"
#include "chunk.h"
#include "content.h"
#include "utils.h"

#define OFF_LEN 10

char *_create_dir_entry_str(dir *);

int dir_create(const char *path, mode_t mode, memcached *m)
{
    dir *new_dir = malloc(sizeof(dir));

    dir_init(new_dir, path, mode, m);
    if (new_dir->content_inode == -1)
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
    // d->child_count = 0;

    //parse
    parse_val prs = parse_path(path);
    memcpy(d->dir_name, prs.arr[prs.n - 1], strlen(prs.arr[prs.n - 1]));

    d->content_inode = content_create(m);
    d->mode = mode;
}

void dir_append(char *path, dir *new_dir, memcached *m)
{
    char *str = _create_dir_entry_str(new_dir);
    dir *par_dir = dir_mmch_getdir(path, m);
    content *cn = content_mmch_getcontent(par_dir->content_inode, m);
    content_append(cn, strlen(str), str, m);

    free(str);
    free(par_dir);
    free(cn);
}

dir *dir_mmch_getdir(const char *path, memcached *m)
{
    char *key = strdup(path);
    mm_data_info info = memcached_get(m, key);

    // copy given value into
    dir *dir = malloc(sizeof(struct dir));
    memcpy(dir, info.value, info.size);

    // free(key);
    return dir;
}

int dir_rmdir(const char *path, memcached *m)
{
    //     char *par_path = get_par_path(path);
    //     char *cur_path = get_cur_path(path);
    //     dir *pd = dir_mmch_getdir(par_path, m);
    //     dir_childs *dc = dir_get_childs(pd, m);

    //     int res = -1;
    //     char *str;
    //     dir *d = malloc(sizeof(struct dir));
    //     for (int i = 0; i < dc->n; i++)
    //     {
    //         char *cur_child = dc->arr[i];
    //         printf("EEEEEEEEEEEEEn\n\n%s\n%s\n", cur_child, cur_path);
    //         if (!strcmp(cur_child, cur_path))
    //         {
    //             res = 0;
    //         }
    //         else
    //         {
    //             memcpy(d->dir_name, cur_child, strlen(cur_child) + 1);
    //             char *child_str = _create_dir_entry_str(d);
    //             int len = str ? strlen(str) : 0;
    //             str = realloc(str, len + strlen(child_str));
    //             memcpy(str + len, child_str, strlen(child_str));
    //         }
    //     }

    //     chunk *c = chunk_mmch_getchunk(pd->content_inode, m);

    //     chunk_reset(c, m);
    //     chunk_write(c, str, strlen(str), m);
    //     return res;
}

dir_childs dir_get_childs(dir *d, memcached *m)
{
    dir_childs dc;
    memset(&dc, 0, sizeof(struct dir_childs));
    dc.n = 0;

    content *cn = content_mmch_getcontent(d->content_inode, m);
    char s[2 * DATA_SIZE] = {0};
    int read_data = content_read_full_chunk(cn, 0, s, m);
    read_data += content_read_full_chunk(cn, 1, s + DATA_SIZE, m);
    int ind = 0;

    int chunk_ind = 1;
    while (ind < read_data)
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
        dc.n++;
        dc.arr = realloc(dc.arr, sizeof(char *) * dc.n);
        dc.arr[dc.n - 1] = strdup(val1);

        if (DATA_SIZE < ind)
        {
            chunk_ind++;
            memmove(s, s + DATA_SIZE, DATA_SIZE);
            ind -= DATA_SIZE;
            read_data -= DATA_SIZE;
            read_data += content_read_full_chunk(cn, chunk_ind, s + DATA_SIZE, m);
        }
        free(cp_len1);
        free(val1);
    }
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