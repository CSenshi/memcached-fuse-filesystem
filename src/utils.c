#include "utils.h"
#include "string.h"
#include <stdlib.h>

parse_val parse_path(const char *path)
{
    parse_val prs;
    prs.n = count_char(path, '/') + 1;
    prs.arr = malloc(sizeof(char *) * prs.n);

    int i = 0;
    prs.arr[i++] = strdup("/");
    prs.arr[i++] = strdup("/");

    char *tok = strdup(path);

    tok = strtok(tok, "/");
    while (tok)
    {
        prs.arr[i++] = strdup(tok);
        tok = strtok(NULL, "/");
    }

    return prs;
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

int count_char(const char *str, char c)
{
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
        count += (str[i] == c) ? 1 : 0;
    return count;
}