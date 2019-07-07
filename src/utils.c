#include "utils.h"
#include "string.h"
#include <stdlib.h>

char *parse_path(const char *path)
{
    // ToDo parse
    return strdup(path);
}

int get_next_index(struct memcached *m)
{
    int ind_value = memcached_increment(m, INDEX_KEY_STR, 1);
    return ind_value;
}

char *int_to_str(int k)
{
    int len = 20;
    char *str = malloc(len);
    sprintf(str, "%d", k);
    return str;
}

int str_to_int(char *str)
{
    return atoi(str);
}