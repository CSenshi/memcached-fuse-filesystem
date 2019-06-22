#include "../memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TESTS 10

typedef int test_fun(memcached *m);

int test_0(memcached *m);
int test_1(memcached *m);
int test_2(memcached *m);
int test_3(memcached *m);
int test_4(memcached *m);
int test_5(memcached *m);
int test_6(memcached *m);
int test_7(memcached *m);
int test_8(memcached *m);
int test_9(memcached *m);

typedef struct fun_desc
{
    test_fun *fun;
} fun_desc;

fun_desc test_table[] = {
    {test_0},
    {test_1},
    {test_2},
    {test_3},
    {test_4},
    {test_5},
    {test_6},
    {test_7},
    {test_8},
    {test_9}};

int main()
{
    memcached *m = malloc(sizeof(memcached));
    memcached_init(m);
    for (int i = 0; i < NUM_TESTS; i++)
    {
        memcached_flush(m);

        int res = test_table[i].fun(m);
        if (res == 0)
            printf("Passed : Test %d\n", i);
        else
            printf("Failed : Test %d\n", i);
    }
    memcached_exit(m);
}

int test_0(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "test_value";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;
    return 0;
}

int test_1(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "test_value";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;
    res = memcached_add(m, info);
    if (res != NOT_STORED)
        return -1;
    return 0;
}

int test_2(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "test_value";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    value_test = "test_value_changed";
    res = memcached_set(m, info);
    if (res != STORED)
        return -1;

    res = memcached_set(m, info);
    if (res != STORED)
        return -1;

    return 0;
}
int test_3(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "test_value";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    mm_data_info info_res = memcached_get(m, key_test);
    if (!strcmp(info_res.value, info.value))
        return -1;

    return 0;
}
int test_4(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "test_value";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    res = memcached_delete(m, key_test);
    if (res != DELETED)
        return -1;

    res = memcached_delete(m, key_test);
    if (res != NOT_FOUND)
        return -1;

    mm_data_info info_res = memcached_get(m, key_test);
    if (info_res.value)
        return -1;

    res = memcached_set(m, info);
    if (res != STORED)
        return -1;

    return 0;
}
int test_5(memcached *m)
{
    return 0;
}
int test_6(memcached *m)
{
    return 0;
}
int test_7(memcached *m)
{
    return 0;
}
int test_8(memcached *m)
{
    return 0;
}
int test_9(memcached *m)
{
    return 0;
}
