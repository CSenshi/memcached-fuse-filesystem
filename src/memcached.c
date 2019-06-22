#include "memcached.h"
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "debug.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADD_STORED_STR "STORED"
#define ADD_NOT_STORED_STR "NOT_STORED"

char *_create_struct_request(char *command, mm_data_info info);
char *_create_str_request(char *command, char *str);

void _append_ending(char **str);

void _send_mm_req(int fd, char *req);
char *_recv_mm_resp(int fd);

mm_data_info _parse_resp(char *resp);

void memcached_init(struct memcached *m)
{
    _debug_print("\n");

    m->addr = "127.0.0.1";
    m->port = 11211;
    _debug_print("Trying to connect to %s:%d...\n", m->addr, m->port);

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

int memcached_add(struct memcached *m, struct mm_data_info info, char *value)
{
    _debug_print("\n");

    char *req = _create_struct_request("add", info);
    char *val = strdup(value);

    _append_ending(&req);
    _append_ending(&val);

    _send_mm_req(m->fd, req);
    _send_mm_req(m->fd, val);

    char *res = _recv_mm_resp(m->fd);

    int return_val = ADD_ERROR;
    if (!strncmp(res, ADD_STORED_STR, strlen(ADD_STORED_STR)))
        return_val = ADD_STORED;
    else if (!strncmp(res, ADD_NOT_STORED_STR, strlen(ADD_NOT_STORED_STR)))
        return_val = ADD_NOT_STORED;

    free(res);
    return return_val;
}

mm_data_info memcached_get(struct memcached *m, char *key)
{
    _debug_print("\n");

    char *req = _create_str_request("get", key);
    _append_ending(&req);

    _send_mm_req(m->fd, req);
    char *res = _recv_mm_resp(m->fd);

    mm_data_info result = _parse_resp(res);
    return result;
}

void memcached_flush(struct memcached *m)
{
    _debug_print("\n");

    char *req = strdup("flush_all");
    _append_ending(&req);

    _send_mm_req(m->fd, req);
    free(req);
    char *resp = _recv_mm_resp(m->fd);
}

void memcached_exit(struct memcached *m)
{
    _debug_print("\n");
    _debug_print("Closing Connection...\n");
    close(m->fd);
}

/* Private Functions */

void _send_mm_req(int fd, char *req)
{
    write(fd, req, strlen(req));
    _debug_print("Request Sent      : %s", req);
}

char *_recv_mm_resp(int fd)
{
    int buff_size = 100;
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

void _append_ending(char **str)
{
    *str = realloc(*str, strlen(*str) + 2);
    strcat(*str, "\r\n");
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