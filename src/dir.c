#include "dir.h"
#include "chunk.h"
#include "utils.h"

int dir_create(const char *path, mode_t mode, memcached *m)
{
    dir *new_dir = malloc(sizeof(dir));
    dir_init(new_dir, path, mode, m);
    int res = memcached_add_struct(m, int_to_str(new_dir->inode), new_dir, sizeof(dir));
    return new_dir->inode;
}

void dir_init(dir *dir, const char *path, mode_t mode, memcached *m)
{
    //parse
    char *dir_name = parse_path(path);
    memcpy(dir->dir_name, dir_name, strlen(dir_name));

    dir->inode = get_next_index(m);

    // ToDo: implement subdirectories
    dir->par_inode = ROOT_DIR_INODE;

    dir->chunk_inode = chunk_create(m);
    dir->mode = mode;
}