#ifndef UTILS_H
#define UTILS_H

#include "memcached.h"
#include "string.h"
#include "utils.h"

char *parse_path(const char *);

int get_next_index(struct memcached *m);

char *int_to_str(int k);

int str_to_int(char *str);

#endif // !UTILS_H