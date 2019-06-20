#include "memcached.h"
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void memcached_init(struct memcached *m)
{
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

void memcached_exit(struct memcached *m)
{
    _debug_print("Closing Connection...\n");
    close(m->fd);
}
