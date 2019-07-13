#ifndef UTILS_H
#define UTILS_H

#include "memcached.h"
#include "string.h"
#include "utils.h"

#define OFF_LEN 3

#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef struct parse_val
{
    int n;
    char **arr;
} parse_val;

parse_val parse_path(const char *);

int get_next_index(struct memcached *m);

char *int_to_str(int k);

int str_to_int(char *str);

int count_char(const char *str, char c);

char *get_par_path(const char *path);

char *get_cur_path(const char *path);

#endif // !UTILS_H