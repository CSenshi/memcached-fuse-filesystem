#define FUSE_USE_VERSION 31

#include "filesys.h"
#include "memcached.h"
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static struct fuse_operations operations = {
    .init = FS_init,
    .destroy = FS_destroy,
    .statfs = FS_statfs,
    .mkdir = FS_mkdir,
    .rmdir = FS_rmdir,
    .opendir = FS_opendir,
    .readdir = FS_readdir,
    .releasedir = FS_releasedir,
    .fsyncdir = FS_fsyncdir,
    .unlink = FS_unlink,
    .create = FS_create,
    .open = FS_open,
    .read = FS_read,
    .write = FS_write,
    .release = FS_release,
    .flush = FS_flush,
    .fsync = FS_fsync,
    .getattr = FS_getattr,
    .access = FS_access,
    .setxattr = FS_setxattr,
    .getxattr = FS_getxattr,
    .listxattr = FS_listxattr,
    .removexattr = FS_removexattr,
    .chmod = FS_chmod,
    .chown = FS_chown,
    .link = FS_link,
    .symlink = FS_symlink,
    .readlink = FS_readlink,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &operations, NULL);
}