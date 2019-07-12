#include "file.h"
#include "content.h"
#include "dir.h"
#include "utils.h"

int file_create(const char *path, mode_t mode, memcached *m)
{
    file f;
    file_init(&f, path, mode, m);

    int res = memcached_add_struct(m, path, &f, sizeof(struct dir), 0, MM_FIL);

    if (res == ERROR || res == NOT_STORED)
        return -1;

    char *par_path = get_par_path(path);
    dir d = dir_mmch_getdir(par_path, m);
    dir_append(&d, f.file_name, m);
    return 0;
}

void file_init(file *f, const char *path, mode_t mode, memcached *m)
{
    // fil with zeros
    memset(f, 0, sizeof(struct file));

    f->_NOT_USED = -1;

    //parse
    memcpy(f->file_name, path, strlen(path));

    content_init(&f->cn, f->file_name, m);

    f->mode = mode;
}

int file_write(file *f, const char *buff, size_t size, off_t off, memcached *m)
{
    int bytes = content_write(&f->cn, off, size, buff, m);
    memcached_replace_struct(m, f->file_name, f, sizeof(struct file), 0, MM_FIL);
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