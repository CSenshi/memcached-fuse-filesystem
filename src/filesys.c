#include "filesys.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "dir.h"
#include "file.h"
#include "memcached.h"
#include "utils.h"

int FS_COUNT = 0;

int _FS_check(memcached *m);
void _FS_new(memcached *m);
void _FS_recreate(memcached *m);

/*  Initialize filesystem
 *
 *  The return value will passed in the private_data field of struct fuse_context to all file operations,
 *  and as a parameter to the destroy() method.
 *  It overrides the initial value provided to fuse_main() / fuse_new().*/
void *FS_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    _debug_print_FS("%d FS : Called init\n", FS_COUNT++);

    (void)conn;

    // Initialize connection
    struct memcached *m = malloc(sizeof(struct memcached));
    memcached_init(m);

    if (_FS_check(m))
        _FS_recreate(m);
    else
        _FS_new(m);

    return m;
}

/* Clean up filesystem
 *
 * Called on filesystem exit. */
void FS_destroy(void *private_data)
{
    _debug_print_FS("%d FS : Called destroy\n", FS_COUNT++);

    memcached *m = (memcached *)private_data;
    memcached_exit(m);
}

/* Get file system statistics
 *
 * The 'f_favail', 'f_fsid' and 'f_flag' fields are ignored */
int FS_statfs(const char *path, struct statvfs *buf)
{
    _debug_print_FS("%d FS : Called statfs\n", FS_COUNT++);

    return 0;
}

/* Create a directory
 *
 * Note that the mode argument may not have the type specification bits set,
 * i.e. S_ISDIR(mode) can be false.
 * To obtain the correct directory type bits use mode|S_IFDIR 
 * 
 * RETURN VALUE
 *      mkdir() return  zero  on  success,  or  -1  if  an  error */
int FS_mkdir(const char *path, mode_t mode)
{
    _debug_print_FS("%d FS : Called mkdir\n", FS_COUNT++);

    struct fuse_context *context = (struct fuse_context *)fuse_get_context();
    struct memcached *m = (struct memcached *)(context->private_data);

    int res = dir_create(path, mode, m);
    return res;
}

/* Remove a directory */
int FS_rmdir(const char *path)
{
    _debug_print_FS("%d FS : Called rmdir\n", FS_COUNT++);

    return 0;
}

/* Open directory
 *
 * Unless the 'default_permissions' mount option is given,
 * this method should check if opendir is permitted for this directory. 
 * Optionally opendir may also return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to readdir, releasedir and fsyncdir. */
int FS_opendir(const char *path, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called opendir\n", FS_COUNT++);

    return 0;
}

/* Read directory
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, 
 *    and passes zero to the filler function's offset. 
 *    The filler function will not return '1' (unless an error happens), 
 *    so the whole directory is read in a single readdir operation.
 *
 * 2) The readdir implementation keeps track of the offsets of the directory entries. 
 *    It uses the offset parameter and always passes non-zero offset to the filler function. 
 *    When the buffer is full (or an error happens) the filler function will return '1'. */
int FS_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t off, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    _debug_print_FS("%d FS : Called readdir\n", FS_COUNT++);

    struct fuse_context *context = (struct fuse_context *)fuse_get_context();
    struct memcached *m = (struct memcached *)(context->private_data);

    // bad path name
    if (path[0] != '/')
        return -ENOENT;

    mm_data_info info = memcached_get(m, path);
    // file was not found
    if (info.value == NULL)
        return -ENOENT;

    if (info.flags & MM_DIR)
    {
        dir *d = dir_mmch_getdir(path, m);
        dir_childs *dc = dir_get_childs(d, m);

        filler(buf, "..", NULL, 0, 0);
        filler(buf, ".", NULL, 0, 0);

        for (int i = 0; i < dc->n; i++)
            filler(buf, strdup(dc->arr[i]), NULL, 0, 0);

        free(dc);
        free(d);

        return 0;
    }

    return -1;
}

/* Release directory */
int FS_releasedir(const char *path, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called releasedir\n", FS_COUNT++);

    return 0;
}

/* Synchronize directory contents
 *
 * If the datasync parameter is non-zero, then only the user data should be flushed, not the meta data */
int FS_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called fsyncdir\n", FS_COUNT++);

    return 0;
}

/* Remove a file */
int FS_unlink(const char *path)
{
    _debug_print_FS("%d FS : Called unlink\n", FS_COUNT++);

    return 0;
}

/* Create and open a file
 *
 * If the file does not exist, first create it with the specified mode, and then open it.
 *
 * If this method is not implemented or under Linux kernel versions earlier than 2.6.15, 
 * the mknod() and open() methods will be called instead. */
int FS_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called create\n", FS_COUNT++);

    return 0;
}

/* Open a file
 *
 * Open flags are available in fi->flags. The following rules apply.
 *
 * 1). Creation (O_CREAT, O_EXCL, O_NOCTTY) flags will be filtered out / handled by the kernel.
 * 2). Access modes (O_RDONLY, O_WRONLY, O_RDWR, O_EXEC, O_SEARCH) 
 *     should be used by the filesystem to check if the operation is permitted.
 *     If the -o default_permissions mount option is given, this check is already done by the kernel
 *     before calling open() and may thus be omitted by the filesystem.
 * 3). When writeback caching is enabled, the kernel may send read requests even for files opened with O_WRONLY.
 *     The filesystem should be prepared to handle this.
 * 4). When writeback caching is disabled, the filesystem is expected to properly handle the O_APPEND flag 
 *     and ensure that each write is appending to the end of the file.
 * 5). When writeback caching is enabled, the kernel will handle O_APPEND.
 *     However, unless all changes to the file come through the kernel this will not work reliably.
 *     The filesystem should thus either ignore the O_APPEND flag (and let the kernel handle it), 
 *     or return an error (indicating that reliably O_APPEND is not available).
 * 6). Filesystem may store an arbitrary file handle (pointer, index, etc) in fi->fh,
 *     and use this in other all other file operations (read, write, flush, release, fsync).
 * 
 * Filesystem may also implement stateless file I/O and not store anything in fi->fh.
 * 
 * There are also some flags (direct_io, keep_cache) which the filesystem may set in fi, 
 * to change the way the file is opened. See fuse_file_info structure in <fuse_common.h> for more details.
 * 
 * If this request is answered with an error code of ENOSYS and FUSE_CAP_NO_OPEN_SUPPORT is set in fuse_conn_info.capable, 
 * this is treated as success and future calls to open will also succeed without being send to the filesystem process. */
int FS_open(const char *path, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called open\n", FS_COUNT++);

    return 0;
}

/* Read data from an open file
 *
 * Read should return exactly the number of bytes requested except on EOF or error, 
 * otherwise the rest of the data will be substituted with zeroes.
 * An exception to this is when the 'direct_io' mount option is specified, 
 * in which case the return value of the read system call will reflect the return value of this operation. */
int FS_read(const char *path, char *buf, size_t size, off_t off, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called read\n", FS_COUNT++);

    return 0;
}

/* Write data to an open file
 *
 * Write should return exactly the number of bytes requested except on error. 
 * An exception to this is when the 'direct_io' mount option is specified (see read operation).
 *
 * Unless FUSE_CAP_HANDLE_KILLPRIV is disabled, this method is expected to reset the setuid and setgid bits. */
int FS_write(const char *path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called write\n", FS_COUNT++);

    return 0;
}

/* Release an open file
 *
 * Release is called when there are no more references to an open file:
 *      all file descriptors are closed and all memory mappings are unmapped.
 *
 * For every open() call there will be exactly one release() call with the same flags and file handle.
 * It is possible to have a file opened more than once, in which case only the last release will mean, 
 * that no more reads/writes will happen on the file. The return value of release is ignored. */
int FS_release(const char *path, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called release\n", FS_COUNT++);

    return 0;
}

/* Possibly flush cached data
 *
 * BIG NOTE: 
 *          This is not equivalent to fsync(). It's not a request to sync dirty data.
 * 
 * Flush is called on each close() of a file descriptor,
 * as opposed to release which is called on the close of the last file descriptor for a file.
 * Under Linux, errors returned by flush() will be passed to userspace as errors from close(),
 * so flush() is a good place to write back any cached dirty data.
 * 
 * However, many applications ignore errors on close(), and on non-Linux systems, 
 * close() may succeed even if flush() returns an error. For these reasons, 
 * filesystems should not assume that errors returned by flush will ever be noticed or even delivered.
 * 
 * NOTE: 
 *      The flush() method may be called more than once for each open(). 
 *      This happens if more than one file descriptor refers to an open file handle,
 *      e.g. due to dup(), dup2() or fork() calls. 
 *      It is not possible to determine if a flush is final, so each flush should be treated equally.
 *      Multiple write-flush sequences are relatively rare, so this shouldn't be a problem.
 * 
 * Filesystems shouldn't assume that flush will be called at any particular point. 
 * It may be called more times than expected, or not at all. */
int FS_flush(const char *path, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called flush\n", FS_COUNT++);

    return 0;
}

/* Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data should be flushed, not the meta data. */
int FS_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called fsync\n", FS_COUNT++);

    return 0;
}

/* Get file attributes.
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored. 
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given.
 * In that case it is passed to userspace, but libfuse and the kernel will still assign
 * a different inode for internal use (called the "nodeid").
 *
 * fi will always be NULL if the file is not currently open, but may also be NULL if the file is open. */
int FS_getattr(const char *path, struct stat *buf, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called getattr\n", FS_COUNT++);

    struct fuse_context *context = (struct fuse_context *)fuse_get_context();
    struct memcached *m = (struct memcached *)(context->private_data);

    mm_data_info info = memcached_get(m, path);

    // file was not found
    if (info.value == NULL)
        return -ENOENT;

    buf->st_uid = context->uid; // The owner of the file/directory is the user who mounted the filesystem
    buf->st_gid = context->gid; // The group of the file/directory is the same as the group of the user who mounted the filesystem
    buf->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
    buf->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now

    int err = 0;
    if (info.flags & MM_DIR) // check if directory
    {
        dir *d = dir_mmch_getdir(path, m);
        memset(buf, 0, sizeof(struct stat));
        buf->st_mode = S_IFDIR | 0755;
        buf->st_nlink = 2;
        buf->st_size = strlen(d->dir_name);
        free(d);
    }
    else if (info.flags & MM_FILE) // check if file
    {
        buf->st_mode = S_IFREG | 0644;
        buf->st_nlink = 1;
        buf->st_size = 1024;
    }
    else //error
        err = -1;

    return err;
}

/* Check file access permissions
 *
 * This will be called for the access() system call. 
 * If the 'default_permissions' mount option is given, this method is not called.
 * 
 * This method is not called under Linux kernel versions 2.4.x */
int FS_access(const char *path, int mask)
{
    _debug_print_FS("%d FS : Called access\n", FS_COUNT++);

    return 0;
}

/* Set extended attributes */
int FS_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    _debug_print_FS("%d FS : Called setxattr\n", FS_COUNT++);

    return 0;
}
/* Get extended attributes */
int FS_getxattr(const char *path, const char *name, char *value, size_t size)
{
    _debug_print_FS("%d FS : Called getxattr\n", FS_COUNT++);

    return 0;
}

/* List extended attributes */
int FS_listxattr(const char *path, char *lsit, size_t size)
{
    _debug_print_FS("%d FS : Called listxattr\n", FS_COUNT++);

    return 0;
}

/* Remove extended attributes */
int FS_removexattr(const char *path, const char *name)
{
    _debug_print_FS("%d FS : Called removexattr\n", FS_COUNT++);

    return 0;
}

/* Change the permission bits of a file
 * 
 * fi will always be NULL if the file is not currenlty open, 
 * but may also be NULL if the file is open. */
int FS_chmod(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called chmod\n", FS_COUNT++);

    return 0;
}

/* Change the owner and group of a file
 * 
 * fi will always be NULL if the file is not currenlty open, 
 * but may also be NULL if the file is open.
 * 
 * Unless FUSE_CAP_HANDLE_KILLPRIV is disabled, 
 * this method is expected to reset the setuid and setgid bits. */
int FS_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi)
{
    _debug_print_FS("%d FS : Called chown\n", FS_COUNT++);

    return 0;
}

/* Create a hard link to a file */
int FS_link(const char *oldpath, const char *newpath)
{
    _debug_print_FS("%d FS : Called link\n", FS_COUNT++);

    return 0;
}

/* Create a symbolic link */
int FS_symlink(const char *linkname, const char *path)
{
    _debug_print_FS("%d FS : Called symlink\n", FS_COUNT++);

    return 0;
}

/* Read the target of a symbolic link
 *
 * The buffer should be filled with a null terminated string. 
 * The buffer size argument includes the space for the terminating null character. 
 * If the linkname is too long to fit in the buffer, it should be truncated. 
 * The return value should be 0 for success. */
int FS_readlink(const char *path, char *buf, size_t size)
{
    _debug_print_FS("%d FS : Called readlink\n", FS_COUNT++);

    return 0;
}

/* Private Functions */
int _FS_check(memcached *m)
{
    struct mm_data_info info = memcached_get(m, INDEX_KEY_STR);
    if (!info.value)
        return 0;

    return 1;
}

void _FS_new(memcached *m)
{
    memcached_flush(m);

    // Insert inode counter
    char *index_key = int_to_str(INIT_INODE);
    struct mm_data_info index_info = {INDEX_KEY_STR, 0, 0, strlen(index_key), index_key};
    memcached_add(m, index_info);

    // Add Root Directory
    mode_t root_mode = 0;
    dir_create("/", root_mode, m);
}

void _FS_recreate(memcached *m)
{
    (void)m;
}