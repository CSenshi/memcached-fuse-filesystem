#include "../memcached.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TESTS 9

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
    {test_8}};

int main()
{
    memcached *m = malloc(sizeof(memcached));
    memcached_init(m);
    int total = 0;
    for (int i = 7; i < NUM_TESTS; i++)
    {
        memcached_flush(m);

        int res = test_table[i].fun(m);
        if (res == 0)
            printf("Passed : Test %d\n", i);
        else
            printf("Failed : Test %d\n", i);
        total += res == 0 ? 1 : 0;
    }
    printf("Total Tests Passed : %d/%d", total, NUM_TESTS);
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
    char *key_test = "test_key";
    char *value_test = "5";
    mm_data_info info = {key_test, 0, 0, strlen(value_test), value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    res = memcached_increment(m, key_test, 1);

    mm_data_info info_res = memcached_get(m, key_test);
    if (res != 6)
        return -1;

    res = memcached_increment(m, key_test, 10);

    info_res = memcached_get(m, key_test);
    if (res != 16)
        return -1;

    return 0;
}

int test_6(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "s\0\0aa";
    mm_data_info info = {key_test, 0, 0, 5, value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    mm_data_info info_res = memcached_get(m, key_test);

    for (int i = 0; i < 5; i++)
    {
        if (info_res.value[i] != value_test[i])
            return -1;
    }

    return 0;
}
int test_7(memcached *m)
{
    char *key_test = "test_key";
    char *value_test = "QWERTY";
    mm_data_info info = {key_test, 0, 0, 6, value_test};

    int res = memcached_add(m, info);
    if (res != STORED)
        return -1;

    value_test = "A\0\0\0\0\0";
    mm_data_info new_info = {key_test, 0, 0, 6, value_test};
    res = memcached_set(m, new_info);
    if (res != STORED)
        return -1;

    mm_data_info info_res = memcached_get(m, key_test);
    for (int i = 0; i < 6; i++)
        if (info_res.value[i] != value_test[i])
            return -1;

    return 0;
}

int test_8(memcached *m)
{
    typedef struct test_struct
    {
        int val1;
        int val2;
        char arr[7];
    } test_struct;

    char *key_test = "test_key";

    test_struct *str = malloc(sizeof(struct test_struct));
    str->val1 = 1;
    str->val2 = 2;
    str->arr[0] = 'S';
    str->arr[1] = 'E';
    str->arr[2] = 'N';
    str->arr[3] = 'S';
    str->arr[4] = 'H';
    str->arr[5] = 'I';
    str->arr[6] = '\0';

    // ADD
    memcached_add_struct(m, key_test, str, sizeof(struct test_struct));
    mm_data_info info = memcached_get(m, key_test);

    test_struct *str2 = malloc(sizeof(test_struct));
    memcpy(str2, info.value, sizeof(test_struct));
    printf("AAAA %s\n", str2->arr);
    if (str2->val1 != str->val1 || str2->val2 != str->val2 || strcmp("SENSHI", str2->arr))
        return -1;

    // REPLACE
    memcached_replace_struct(m, key_test, str, sizeof(struct test_struct));
    info = memcached_get(m, key_test);

    memset(str2, 0, sizeof(test_struct));
    memcpy(str2, info.value, sizeof(test_struct));

    if (str2->val1 != str->val1 || str2->val2 != str->val2 || strcmp("SENSHI", str2->arr))
        return -1;

    // set
    str->val1 = 100;
    str->val2 = 200;

    str->arr[0] = 'O';
    str->arr[1] = 'S';
    str->arr[2] = 'O';
    str->arr[3] = 'S';
    str->arr[4] = 'O';
    str->arr[5] = 'S';
    str->arr[6] = '\0';

    memcached_replace_struct(m, key_test, str, sizeof(struct test_struct));
    info = memcached_get(m, key_test);

    memset(str2, 0, sizeof(test_struct));
    memcpy(str2, info.value, sizeof(test_struct));

    if (str2->val1 != str->val1 || str2->val2 != str->val2 || strcmp("OSOSOS", str2->arr))
        return -1;

    return 0;
}