#ifndef FILESYS_H
#define FILESYS_H

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Task 1 : init, destroy, statfs
void *FS_init(struct fuse_conn_info *, struct fuse_config *);
void FS_destroy(void *);
int FS_statfs(const char *, struct statvfs *);

// Task 2 : mkdir, rmdir, opendir, readdir, releasedir, fsyncdir
int FS_mkdir(const char *, mode_t);
int FS_rmdir(const char *);
int FS_opendir(const char *, struct fuse_file_info *);
int FS_readdir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *, enum fuse_readdir_flags);
int FS_releasedir(const char *, struct fuse_file_info *);
int FS_fsyncdir(const char *, int, struct fuse_file_info *);

// Task 3 : unlink, create, open, read, write, release, flush, fsync
int FS_unlink(const char *);
int FS_create(const char *, mode_t, struct fuse_file_info *);
int FS_open(const char *, struct fuse_file_info *);
int FS_read(const char *, char *, size_t, off_t, struct fuse_file_info *);
int FS_write(const char *, const char *, size_t, off_t, struct fuse_file_info *);
int FS_release(const char *, struct fuse_file_info *);
int FS_flush(const char *, struct fuse_file_info *);
int FS_fsync(const char *, int, struct fuse_file_info *);

// Task 5 : getattr, access, setxattr, getxattr, listxattr, removexattr
int FS_getattr(const char *, struct stat *, struct fuse_file_info *);
int FS_access(const char *, int);
int FS_setxattr(const char *, const char *, const char *, size_t, int);
int FS_getxattr(const char *, const char *, char *, size_t);
int FS_listxattr(const char *, char *, size_t);
int FS_removexattr(const char *, const char *);

// Task 6 : chmod, chown
int FS_chmod(const char *, mode_t, struct fuse_file_info *);
int FS_chown(const char *, uid_t, gid_t, struct fuse_file_info *);

// Task 7 : link, symlink, readlink
int FS_link(const char *, const char *);
int FS_symlink(const char *, const char *);
int FS_readlink(const char *, char *, size_t);

#endif // !FILESYS_H