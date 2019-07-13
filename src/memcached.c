#include "memcached.h"
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "debug.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MMCH_ADDR "127.0.0.1"
#define MMCH_PORT 11211

#define STORED_STR "STORED"
#define NOT_STORED_STR "NOT_STORED"
#define DELETED_STR "DELETED"
#define NOT_FOUND_STR "NOT_FOUND"

char *_create_str_request3(char *command, const char *str1, char *str2);
char *_create_struct_request(char *command, mm_data_info *info);
char *_create_str_request(char *command, const char *str);
void _parse_resp(char *resp, mm_data_info *);
void _send_mm_req(int fd, char *req, int size);
void _append_ending(char **str, int size);
void _recv_mm_resp(int fd, char *buf);

void memcached_init(struct memcached *m)
{
    _debug_print("\n");
    _debug_print("Trying to connect to %s:%d...\n", MMCH_ADDR, MMCH_PORT);

    m->addr = MMCH_ADDR;
    m->port = MMCH_PORT;

    m->fd = socket(AF_INET, SOCK_STREAM, 0);

    struct in_addr s_addr;
    inet_pton(AF_INET, m->addr, &s_addr.s_addr);

    struct sockaddr_in addr;
    addr.sin_addr = s_addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m->port);
    connect(m->fd, (const struct sockaddr *)&addr, sizeof(addr));
    _debug_print("Connection Succesful!\n");
}

int memcached_add(struct memcached *m, struct mm_data_info info)
{
    _debug_print("\n");

    char *command = "add";
    char *flag_str = int_to_str(info.flags);
    char *ttl_str = int_to_str(info.ttl);
    char *size_str = int_to_str(info.size);

    int command_len = strlen(command);
    int key_len = strlen(info.key);
    int flag_len = strlen(flag_str);
    int ttl_len = strlen(ttl_str);
    int size_len = strlen(size_str);

    int to_alloc = command_len + key_len + flag_len + ttl_len + size_len + info.size + 8;

    char req2[to_alloc];
    int n = 0;

    memcpy(req2, command + n, command_len);
    n += command_len;
    req2[n++] = ' ';

    memcpy(req2 + n, info.key, key_len);
    n += key_len;
    req2[n++] = ' ';

    memcpy(req2 + n, flag_str, flag_len);
    n += flag_len;
    req2[n++] = ' ';

    memcpy(req2 + n, ttl_str, ttl_len);
    n += ttl_len;
    req2[n++] = ' ';

    memcpy(req2 + n, size_str, size_len);
    n += size_len;

    free(flag_str);
    free(ttl_str);
    free(size_str);

    req2[n++] = '\r';
    req2[n++] = '\n';

    memcpy(req2 + n, info.value, info.size);
    n += info.size;

    req2[n++] = '\r';
    req2[n++] = '\n';
    _send_mm_req(m->fd, req2, n);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    int return_val = ERROR;
    if (!strncmp(buffer, STORED_STR, strlen(STORED_STR)))
        return_val = STORED;
    else if (!strncmp(buffer, NOT_STORED_STR, strlen(NOT_STORED_STR)))
        return_val = NOT_STORED;

    return return_val;
}

int memcached_set(struct memcached *m, struct mm_data_info info)
{
    _debug_print("\n");

    char *command = "set";
    char *flag_str = int_to_str(info.flags);
    char *ttl_str = int_to_str(info.ttl);
    char *size_str = int_to_str(info.size);

    int command_len = strlen(command);
    int key_len = strlen(info.key);
    int flag_len = strlen(flag_str);
    int ttl_len = strlen(ttl_str);
    int size_len = strlen(size_str);

    int to_alloc = command_len + key_len + flag_len + ttl_len + size_len + info.size + 8;

    char req2[to_alloc];
    int n = 0;

    memcpy(req2, command + n, command_len);
    n += command_len;
    req2[n++] = ' ';

    memcpy(req2 + n, info.key, key_len);
    n += key_len;
    req2[n++] = ' ';

    memcpy(req2 + n, flag_str, flag_len);
    n += flag_len;
    req2[n++] = ' ';

    memcpy(req2 + n, ttl_str, ttl_len);
    n += ttl_len;
    req2[n++] = ' ';

    memcpy(req2 + n, size_str, size_len);
    n += size_len;

    free(flag_str);
    free(ttl_str);
    free(size_str);

    req2[n++] = '\r';
    req2[n++] = '\n';

    memcpy(req2 + n, info.value, info.size);
    n += info.size;

    req2[n++] = '\r';
    req2[n++] = '\n';
    _send_mm_req(m->fd, req2, n);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    int return_val = ERROR;
    if (!strncmp(buffer, STORED_STR, strlen(STORED_STR)))
        return_val = STORED;
    else if (!strncmp(buffer, NOT_STORED_STR, strlen(NOT_STORED_STR)))
        return_val = NOT_STORED;

    return return_val;
}

int memcached_replace(struct memcached *m, struct mm_data_info info)
{
    _debug_print("\n");

    char *command = "replace";
    char *flag_str = int_to_str(info.flags);
    char *ttl_str = int_to_str(info.ttl);
    char *size_str = int_to_str(info.size);

    int command_len = strlen(command);
    int key_len = strlen(info.key);
    int flag_len = strlen(flag_str);
    int ttl_len = strlen(ttl_str);
    int size_len = strlen(size_str);

    int to_alloc = command_len + key_len + flag_len + ttl_len + size_len + info.size + 8;

    char req2[to_alloc];
    int n = 0;

    memcpy(req2, command + n, command_len);
    n += command_len;
    req2[n++] = ' ';

    memcpy(req2 + n, info.key, key_len);
    n += key_len;
    req2[n++] = ' ';

    memcpy(req2 + n, flag_str, flag_len);
    n += flag_len;
    req2[n++] = ' ';

    memcpy(req2 + n, ttl_str, ttl_len);
    n += ttl_len;
    req2[n++] = ' ';

    memcpy(req2 + n, size_str, size_len);
    n += size_len;

    req2[n++] = '\r';
    req2[n++] = '\n';

    memcpy(req2 + n, info.value, info.size);
    n += info.size;

    free(flag_str);
    free(ttl_str);
    free(size_str);

    req2[n++] = '\r';
    req2[n++] = '\n';
    _send_mm_req(m->fd, req2, n);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    int return_val = ERROR;
    if (!strncmp(buffer, STORED_STR, strlen(STORED_STR)))
        return_val = STORED;
    else if (!strncmp(buffer, NOT_STORED_STR, strlen(NOT_STORED_STR)))
        return_val = NOT_STORED;

    return return_val;
}

int memcached_increment(struct memcached *m, const char *key, int n)
{
    _debug_print("\n");

    char *req = _create_str_request3("incr", key, int_to_str(n));
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    return str_to_int(buffer);
}

int memcached_delete(struct memcached *m, const char *key)
{
    char *command = "delete";
    int command_len = 6;
    int key_len = strlen(key);
    int to_alloc = command_len + key_len + 3;

    char req2[to_alloc];
    int n = 0;

    memcpy(req2, command + n, command_len);
    n += command_len;
    req2[n++] = ' ';

    memcpy(req2 + n, key, key_len);
    n += key_len;

    req2[n++] = '\r';
    req2[n++] = '\n';

    _send_mm_req(m->fd, req2, n);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    int result = ERROR;
    if (!strncmp(buffer, DELETED_STR, strlen(DELETED_STR)))
        result = DELETED;
    else if (!strncmp(buffer, NOT_FOUND_STR, strlen(NOT_FOUND_STR)))
        result = NOT_FOUND;

    return result;
}

void memcached_get(struct memcached *m, const char *key, mm_data_info *info)
{
    _debug_print("\n");

    char *command = "get";
    int command_len = 3;
    int key_len = strlen(key);
    int to_alloc = command_len + key_len + 3;

    char req2[to_alloc];
    int n = 0;

    memcpy(req2, command + n, command_len);
    n += command_len;
    req2[n++] = ' ';

    memcpy(req2 + n, key, key_len);
    n += key_len;

    req2[n++] = '\r';
    req2[n++] = '\n';

    _send_mm_req(m->fd, req2, n);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    _parse_resp(buffer, info);
}

void memcached_flush(struct memcached *m)
{
    _debug_print("\n");

    char *req = strdup("flush_all");
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));
    free(req);

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);
}

int memcached_add_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags)
{
    mm_data_info info;

    info.key = strdup(key);
    info.ttl = ttl;
    info.flags = flags;
    info.size = size;

    info.value = malloc(size + 1);
    memset(info.value, 0, size + 1);
    memcpy(info.value, src, info.size);

    int res = memcached_add(m, info);
    free(info.value);
    return res;
}

int memcached_set_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags)
{
    mm_data_info info;

    info.key = strdup(key);
    info.ttl = ttl;
    info.flags = flags;
    info.size = size;

    info.value = malloc(size + 1);
    memset(info.value, 0, size + 1);
    memcpy(info.value, src, info.size);

    int res = memcached_set(m, info);
    free(info.value);
    return res;
}

int memcached_replace_struct(struct memcached *m, const char *key, void *src, int size, int ttl, int flags)
{
    mm_data_info info;

    info.key = strdup(key);
    info.ttl = ttl;
    info.flags = flags;
    info.size = size;

    info.value = malloc(size + 1);
    memset(info.value, 0, size + 1);
    memcpy(info.value, src, info.size);

    int res = memcached_replace(m, info);
    free(info.value);
    return res;
}

void memcached_exit(struct memcached *m)
{
    _debug_print("\n");
    _debug_print("Closing Connection...\n");

    char *req = strdup("quit");
    _append_ending(&req, strlen(req));
    _send_mm_req(m->fd, req, strlen(req));

    char buffer[MAX_READ_BYTES];
    _recv_mm_resp(m->fd, buffer);

    close(m->fd);

    _debug_print("\n");
    _debug_print("Connection Closed Succesfully!\n");
}

/* Private Functions */
void _send_mm_req(int fd, char *req, int size)
{
    write(fd, req, size);
    _debug_print("Request Sent      : %s", req);
}

void _recv_mm_resp(int fd, char *buffer)
{

    int read_bytes = read(fd, buffer, MAX_READ_BYTES);
    buffer[read_bytes] = '\0';

    _debug_print("Response Recieved : %s", buffer);
}

char *_create_str_request(char *command, const char *str)
{
    int to_alloc = strlen(command) + strlen(str) + 2;
    char *req = malloc(to_alloc);
    memset(req, 0, to_alloc);

    strcat(req, command);
    strcat(req, " ");
    strcat(req, str);

    return req;
}

char *_create_str_request3(char *command, const char *str1, char *str2)
{
    int to_alloc = strlen(command) + strlen(str1) + strlen(str2) + 1;
    char *req = malloc(to_alloc);
    memset(req, 0, to_alloc);

    strcat(req, command);
    strcat(req, " ");
    strcat(req, str1);
    strcat(req, " ");
    strcat(req, str2);

    return req;
}

char *_create_struct_request(char *command, mm_data_info *info)
{
    char flags_str[10];
    sprintf(flags_str, "%d", info->flags);

    char ttl_str[10];
    sprintf(ttl_str, "%d", info->ttl);

    char size_str[10];
    sprintf(size_str, "%d", info->size);

    int to_alloc = strlen(command) + strlen(info->key) + strlen(flags_str) + strlen(ttl_str) + strlen(size_str) + 7;
    char *req = malloc(to_alloc);
    memset(req, 0, to_alloc);

    strcat(req, command);
    strcat(req, " ");

    strcat(req, info->key);
    strcat(req, " ");

    strcat(req, flags_str);
    strcat(req, " ");

    strcat(req, ttl_str);
    strcat(req, " ");

    strcat(req, size_str);
    return req;
}

void _append_ending(char **str, int size)
{
    *str = realloc(*str, size + 3);
    (*str)[size] = '\r';
    (*str)[size + 1] = '\n';
    (*str)[size + 2] = '\0';
}

void _parse_resp(char *resp, mm_data_info *res)
{
    memset(res, 0, sizeof(struct mm_data_info));
    char *type = strtok(resp, " ");
    if (strncmp(type, "END", strlen("END")))
    {
        res->key = strtok(NULL, " ");
        res->flags = atoi(strtok(NULL, " "));
        res->size = atoi(strtok(NULL, "\n"));
        res->value = strtok(NULL, "\n");
    }
}