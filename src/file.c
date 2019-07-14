#include "file.h"
#include "content.h"
#include "dir.h"
#include "utils.h"

void _create_ex_name(char *ex_path, const char *path);

int file_create(const char *path, mode_t mode, memcached *m)
{
    file f;
    file_init(&f, path, mode, m);

    int res = memcached_add_struct(m, path, &f, sizeof(struct file), 0, MM_FIL);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    // path = get_cur_path(path);
    char *par_path = get_par_path(path);
    dir d;
    dir_mmch_getdir(par_path, m, &d);
    dir_append(&d, f.file_name, m);
    return 0;
}

void file_init(file *f, const char *path, mode_t mode, memcached *m)
{
    // fil with zeros
    memset(f, 0, sizeof(struct file));

    f->_NOT_USED = -1;

    //parse
    parse_val prs = parse_path(path);
    memcpy(f->file_name, prs.arr[prs.n - 1], strlen(prs.arr[prs.n - 1]));

    content_init(&f->cn, path, m);

    char ex_path[MAX_FNAME];

    _create_ex_name(ex_path, path);
    content_init(&f->ex_cn, ex_path, m);

    f->mode = mode;
}

int file_write(file *f, const char *buff, size_t size, off_t off, memcached *m)
{
    int bytes = content_write(&f->cn, off, size, buff, m);
    memcached_replace_struct(m, f->cn.path, f, sizeof(struct file), 0, MM_FIL);
    return bytes;
}

int file_read(file *f, char *buff, size_t size, off_t off, memcached *m)
{
    content cn = f->cn;
    int read_bytes = content_read(&cn, off, size, buff, m);
    return read_bytes;
}

int file_get_size(file *f, memcached *m)
{
    return f->cn.size;
}

int file_setxattr(file *f, const char *name, const char *value, size_t size, memcached *m)
{
    int exists = content_getxattr(&f->ex_cn, name, NULL, size, m);
    int res;
    if (exists != -2) // if exists
        file_remxattr(f, name, m);

    res = content_setxattr(&f->ex_cn, name, value, size, m);
    memcached_replace_struct(m, f->cn.path, f, sizeof(struct file), 0, MM_FIL);
    return res;
}

int file_getxattr(file *f, const char *name, char *buf, size_t size, memcached *m)
{
    return content_getxattr(&f->ex_cn, name, buf, size, m);
}

int file_remxattr(file *f, const char *name, memcached *m)
{
    parse_val pv;
    int res = content_remxattr(&f->ex_cn, name, m, &pv);
    if (res == -1)
        return -2;

    content_free(&f->ex_cn, m);
    memcached_replace_struct(m, f->cn.path, f, sizeof(struct file), 0, MM_FIL);
    content_init(&f->ex_cn, f->ex_cn.path, m);
    for (int i = 0; i < pv.n; i++)
    {
        if (res == i)
            continue;
        content_append(&f->ex_cn, strlen(pv.arr[i]), pv.arr[i], m);
    }

    memcached_replace_struct(m, f->cn.path, f, sizeof(struct file), 0, MM_FIL);

    return 0;
}

int file_listxattr(file *f, char *list, size_t size, memcached *m)
{
    return content_listxattr(&f->ex_cn, list, size, m);
}

void _create_ex_name(char *ex_path, const char *path)
{
    ex_path[0] = 'X';
    memcpy(ex_path + 1, path, strlen(path) + 1);
}
