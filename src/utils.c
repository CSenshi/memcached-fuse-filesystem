#include "utils.h"
#include "dir.h"
#include "string.h"
#include <stdlib.h>

parse_val parse_path(const char *path)
{
    parse_val prs;

    // if making root
    if (strlen(path) == 1)
    {
        prs.n = 1;
        prs.arr = malloc(sizeof(char *) * prs.n);
        prs.arr[0] = "/";
        return prs;
    }

    prs.n = count_char(path, '/') + 1;
    prs.arr = malloc(sizeof(char *) * prs.n);

    int i = 0;
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

char *get_par_path(const char *path)
{
    int n = strlen(path);

    n--;
    while (path[n] != '/')
        n--;

    if (n == 0)
        return "/";
    char *par_path = malloc(n + 1);
    memcpy(par_path, path, n);
    par_path[n] = '\0';
    return par_path;
}

char *get_cur_path(const char *path)
{
    int n = strlen(path);

    n--;
    while (path[n] != '/')
        n--;

    char *cur_path = malloc(strlen(path) - n + 1);
    memcpy(cur_path + n, path, strlen(path) - n);
    cur_path[strlen(path) - n] = '\0';
    return cur_path + 1;
}

void create_ex_name(char *ex_path, const char *path)
{
    ex_path[0] = 'X';
    memcpy(ex_path + 1, path, strlen(path) + 1);
}