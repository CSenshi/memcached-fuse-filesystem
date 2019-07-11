#ifndef MEMCACHED_H
#define MEMCACHED_H

#include <stdio.h>

#define INDEX_KEY_STR "_key_current_index_"

#define MM_DIR 1 << 1
#define MM_FIL 1 << 2
#define MM_CHN 1 << 3
#define MM_CON 1 << 4

typedef struct memcached
{
    char *addr;
    int port;
    int fd;
} memcached;

typedef struct mm_data_info
{
    char *key;
    int flags;
    int ttl;
    int size;
    char *value;
} mm_data_info;

enum ENUM
{
    STORED,
    DELETED,
    NOT_FOUND,
    NOT_STORED,
    ERROR
};
void memcached_init(struct memcached *m);

/* Add a new key */
int memcached_add(struct memcached *m, struct mm_data_info info);

/* Set a key unconditionally */
int memcached_set(struct memcached *m, struct mm_data_info info);

/* Overwrite existing key */
int memcached_replace(struct memcached *m, struct mm_data_info info);

/* Increments key by one */
int memcached_increment(struct memcached *m, const char *key, int n);

/* Deletes an existing key */
int memcached_delete(struct memcached *m, const char *key);

/* Reads a value */
mm_data_info memcached_get(struct memcached *m, const char *key);

/* Invalidate all items immediately */
void memcached_flush(struct memcached *m);

/* Saves new data in memcached */
int memcached_add_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags);

/* Sets new data in memcached */
int memcached_set_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags);

/* Replaces new data in memcached */
int memcached_replace_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags);

void memcached_exit(struct memcached *m);

#endif // !MEMCACHED_H