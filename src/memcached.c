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

char *_create_str_request3(char *command, char *str1, char *str2);
char *_create_struct_request(char *command, mm_data_info info);
char *_create_str_request(char *command, char *str);
mm_data_info _parse_resp(char *resp);
void _send_mm_req(int fd, char *req, int size);
void _append_ending(char **str, int size);
char *_recv_mm_resp(int fd);

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

    char *req = _create_struct_request("add", info);

    char *val = malloc(info.size);
    memcpy(val, info.value, info.size);

    _append_ending(&req, strlen(req));
    _append_ending(&val, info.size);

    _send_mm_req(m->fd, req, strlen(req));
    _send_mm_req(m->fd, val, info.size + 2);

    char *res = _recv_mm_resp(m->fd);

    int return_val = ERROR;
    if (!strncmp(res, STORED_STR, strlen(STORED_STR)))
        return_val = STORED;
    else if (!strncmp(res, NOT_STORED_STR, strlen(NOT_STORED_STR)))
        return_val = NOT_STORED;

    free(res);
    return return_val;
}

int memcached_set(struct memcached *m, struct mm_data_info info)
{
    _debug_print("\n");

    char *req = _create_struct_request("set", info);
    char *val = strdup(info.value);

    _append_ending(&req, strlen(req));
    _append_ending(&val, info.size);

    _send_mm_req(m->fd, req, strlen(req));
    _send_mm_req(m->fd, val, info.size + 2);

    char *res = _recv_mm_resp(m->fd);

    int return_val = ERROR;
    if (!strncmp(res, STORED_STR, strlen(STORED_STR)))
        return_val = STORED;
    else if (!strncmp(res, NOT_STORED_STR, strlen(NOT_STORED_STR)))
        return_val = NOT_STORED;

    free(res);
    return return_val;
}

int memcached_increment(struct memcached *m, char *key, int n)
{
    _debug_print("\n");

    char *req = _create_str_request3("incr", key, int_to_str(n));
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));
    char *res = _recv_mm_resp(m->fd);

    return str_to_int(res);
}

int memcached_delete(struct memcached *m, char *key)
{
    _debug_print("\n");

    char *req = _create_str_request("delete", key);
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));
    char *res = _recv_mm_resp(m->fd);

    int result = ERROR;
    if (!strncmp(res, DELETED_STR, strlen(DELETED_STR)))
        result = DELETED;
    else if (!strncmp(res, NOT_FOUND_STR, strlen(NOT_FOUND_STR)))
        result = NOT_FOUND;

    return result;
}

mm_data_info memcached_get(struct memcached *m, char *key)
{
    _debug_print("\n");

    char *req = _create_str_request("get", key);
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));
    char *res = _recv_mm_resp(m->fd);

    mm_data_info result = _parse_resp(res);
    return result;
}

void memcached_flush(struct memcached *m)
{
    _debug_print("\n");

    char *req = strdup("flush_all");
    _append_ending(&req, strlen(req));

    _send_mm_req(m->fd, req, strlen(req));
    free(req);
    char *resp = _recv_mm_resp(m->fd);
}

int memcached_add_struct(struct memcached *m, char *key, void *src, int size)
{
    mm_data_info chunck_info;

    chunck_info.key = key;
    chunck_info.ttl = 0;
    chunck_info.flags = 0;
    chunck_info.size = size;

    chunck_info.value = malloc(size + 1);
    memcpy(chunck_info.value, src, chunck_info.size);

    return memcached_add(m, chunck_info);
}

void memcached_exit(struct memcached *m)
{
    _debug_print("\n");
    _debug_print("Closing Connection...\n");

    char *req = strdup("quit");
    _append_ending(&req, strlen(req));
    _send_mm_req(m->fd, req, strlen(req));
    char *resp = _recv_mm_resp(m->fd);

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

char *_recv_mm_resp(int fd)
{
    int buff_size = 4096;
    char *buffer = malloc(sizeof(char) * buff_size);

    int len = read(fd, buffer, buff_size);
    buffer[len] = '\0';

    _debug_print("Response Recieved : %s", buffer);
    return buffer;
}

char *_create_str_request(char *command, char *str)
{
    int to_alloc = strlen(command) + strlen(str) + 1;
    char *req = malloc(to_alloc);

    strcat(req, command);
    strcat(req, " ");
    strcat(req, str);

    return req;
}

char *_create_str_request3(char *command, char *str1, char *str2)
{
    int to_alloc = strlen(command) + strlen(str1) + strlen(str2) + 1;
    char *req = malloc(to_alloc);

    strcat(req, command);
    strcat(req, " ");
    strcat(req, str1);
    strcat(req, " ");
    strcat(req, str2);

    return req;
}

char *_create_struct_request(char *command, mm_data_info info)
{
    char flags_str[50];
    sprintf(flags_str, "%d", info.flags);

    char ttl_str[50];
    sprintf(ttl_str, "%d", info.ttl);

    char size_str[50];
    sprintf(size_str, "%d", info.size);

    int to_alloc = strlen(command) + strlen(info.key) + strlen(flags_str) + strlen(ttl_str) + strlen(size_str) + 7;
    char *req = malloc(to_alloc);

    strcat(req, command);
    strcat(req, " ");

    strcat(req, info.key);
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
    *str = realloc(*str, size + 2);
    (*str)[size] = '\r';
    (*str)[size + 1] = '\n';
}

mm_data_info _parse_resp(char *resp)
{
    mm_data_info res = {NULL, 0, 0, 0, NULL};
    char *type = strtok(resp, " ");
    if (strncmp(type, "END", strlen("END")))
    {
        res.key = strtok(NULL, " ");
        res.flags = atoi(strtok(NULL, " "));
        res.size = atoi(strtok(NULL, "\n"));
        res.value = strtok(NULL, "\n");
    }
    return res;
}