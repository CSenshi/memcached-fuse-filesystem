#include "dir.h"
#include "chunk.h"
#include "content.h"
#include "utils.h"

void _create_dir_entry_str(const char *, char *);

int dir_create(const char *path, mode_t mode, uid_t uid, gid_t gid, memcached *m)
{
    dir new_dir;
    dir_init(&new_dir, path, mode, uid, gid, m);

    int res = memcached_add_struct(m, path, &new_dir, sizeof(struct dir), 0, MM_DIR);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    char *par_path = get_par_path(path);
    dir pd;
    dir_mmch_getdir(par_path, m, &pd);
    if (strcmp(path, "/"))
        dir_append(&pd, new_dir.dir_name, m);

    return 0;
}

void dir_init(dir *d, const char *path, mode_t mode, uid_t uid, gid_t gid, memcached *m)
{
    // fill with zeros
    memset(d, 0, sizeof(struct dir));

    d->_NOT_USED = -1;
    d->gid = gid;
    d->uid = uid;
    d->is_linked = 0;

    //parse
    parse_val prs = parse_path(path);
    memcpy(d->dir_name, prs.arr[prs.n - 1], strlen(prs.arr[prs.n - 1]));

    content_init(&d->cn, path, m);

    char ex_path[MAX_FNAME];
    create_ex_name(ex_path, path);
    content_init(&d->ex_cn, ex_path, m);

    d->mode = mode;
}

void dir_append(dir *par_dir, const char *elem, memcached *m)
{
    char str[300];
    _create_dir_entry_str(elem, str);

    content_append(&par_dir->cn, strlen(str), str, m);
    memcached_replace_struct(m, par_dir->cn.path, par_dir, sizeof(struct dir), 0, MM_DIR);
}

void dir_mmch_getdir(const char *path, memcached *m, dir *d)
{
    char key[strlen(path) + 1];
    memcpy(key, path, strlen(path) + 1);
    mm_data_info info;
    memcached_get(m, key, &info);

    // copy given value into
    memcpy(d, info.value, info.size);
}

int dir_rmdir(const char *path, memcached *m)
{
    char *par_path = get_par_path(path);
    char *cur_path = get_cur_path(path);

    dir pd;
    dir_mmch_getdir(par_path, m, &pd);

    dir_childs dc;
    dir_get_childs(&pd, m, &dc);
    int ind = -1;

    for (int i = 0; i < dc.n; i++)
        if (!strcmp(dc.arr[i], cur_path))
        {
            dir d;
            dir_mmch_getdir(path, m, &d);
            content dir_con = d.cn;
            if (dir_con.size != 0)
                return -1;

            memcached_delete(m, path);
            ind = i;
            break;
        }

    if (ind == -1)
        return -1;

    content_free(&pd.cn, m);
    content_init(&pd.cn, par_path, m);
    memcached_set_struct(m, pd.dir_name, &pd, sizeof(struct dir), 0, MM_DIR);
    char str[1500] = {0};
    dir d;
    for (int i = 0; i < dc.n; i++)
    {
        if (ind == i)
            continue;

        char *cur_child = dc.arr[i];
        dir_append(&pd, cur_child, m);
    }
    return 0;
}

void dir_get_childs(dir *d, memcached *m, dir_childs *dc)
{
    memset(dc, 0, sizeof(struct dir_childs));
    dc->n = 0;
    dc->_alloc = 10;
    dc->arr = malloc(sizeof(char *) * dc->_alloc);

    char s[2 * DATA_SIZE] = {0};
    int read_data = content_read_full_chunk(&d->cn, 0, s, m);
    if (d->cn.size > DATA_SIZE)
        read_data += content_read_full_chunk(&d->cn, 1, s + DATA_SIZE, m);
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
        dc->n++;
        if (dc->n == dc->_alloc)
        {
            dc->_alloc += 10;
            dc->arr = realloc(dc->arr, sizeof(char *) * dc->_alloc);
        }
        dc->arr[dc->n - 1] = strdup(val1);
        if (DATA_SIZE < ind)
        {
            chunk_ind++;
            memmove(s, s + DATA_SIZE, DATA_SIZE);
            ind -= DATA_SIZE;
            read_data -= DATA_SIZE;
            read_data += content_read_full_chunk(&d->cn, chunk_ind, s + DATA_SIZE, m);
        }
    }
}

int dir_setxattr(dir *d, const char *name, const char *value, size_t size, memcached *m)
{
    int exists = content_getxattr(&d->ex_cn, name, NULL, size, m);
    int res;
    if (exists != -2) // if exists
        dir_remxattr(d, name, m);

    res = content_setxattr(&d->ex_cn, name, value, size, m);

    memcached_replace_struct(m, d->cn.path, d, sizeof(struct dir), 0, MM_DIR);
    return res;
}

int dir_getxattr(dir *d, const char *name, char *buf, size_t size, memcached *m)
{
    return content_getxattr(&d->ex_cn, name, buf, size, m);
}

int dir_remxattr(dir *d, const char *name, memcached *m)
{
    parse_val pv;
    int res = content_remxattr(&d->ex_cn, name, m, &pv);
    if (res == -1)
        return -2;

    content_free(&d->ex_cn, m);
    memcached_replace_struct(m, d->dir_name, d, sizeof(struct dir), 0, MM_DIR);

    char ex_path[MAX_FNAME];
    create_ex_name(ex_path, d->dir_name);
    content_init(&d->ex_cn, strdup(d->ex_cn.path), m);
    for (int i = 0; i < pv.n; i++)
    {
        if (res == i)
            continue;
        content_append(&d->ex_cn, strlen(pv.arr[i]), pv.arr[i], m);
    }

    memcached_replace_struct(m, d->dir_name, d, sizeof(struct dir), 0, MM_DIR);

    return 0;
}

int dir_listxattr(dir *d, char *list, size_t size, memcached *m)
{
    return content_listxattr(&d->ex_cn, list, size, m);
}

int dir_create_symlink(dir *dir, const char *to_link, memcached *m)
{
    dir->is_linked = -1;
    content_create_symlink(&dir->cn, to_link, m);
    memcached_replace_struct(m, dir->cn.path, dir, sizeof(struct dir), 0, MM_FIL);
    return 0;
}

int dir_read_symlink(dir *d, char *buf, size_t size, memcached *m)
{
    int read_bytes = content_read_symlink(&d->cn, buf, size, m);
    if (read_bytes > 0)
        return 0;
    return -1;
}

int dir_change_mode(dir *d, mode_t mode, memcached *m)
{
    d->mode = mode;
    memcached_replace_struct(m, d->cn.path, d, sizeof(struct dir), 0, MM_DIR);
}

void _create_dir_entry_str(const char *elem, char *buf)
{
    int len1 = strlen(elem);

    int to_alloc = len1 + OFF_LEN + 1;
    memset(buf, '0', to_alloc);

    // fill inode parameter
    char *len1_str = int_to_str(len1);
    memcpy(buf + OFF_LEN - strlen(len1_str), len1_str, strlen(len1_str));
    memcpy(buf + OFF_LEN, elem, len1);

    buf[to_alloc - 1] = '\0';
    free(len1_str);
}