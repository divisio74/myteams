/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** connection.c
*/

#include "client.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int build_addr(const char *ip, int port, struct sockaddr_in *addr)
{
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr->sin_addr) != 1) {
        fprintf(stdout, "Error: invalid ip address '%s'.\n", ip);
        return -1;
    }
    return 0;
}

int connect_to_server(const char *ip, int port)
{
    int fd = 0;
    struct sockaddr_in addr;

    if (build_addr(ip, port, &addr) == -1)
        return -1;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stdout, "Error: socket failed: %s\n", strerror(errno));
        return -1;
    }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        fprintf(stdout, "Error: connect failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

int client_send_line(int fd, const char *line)
{
    size_t len = line ? strlen(line) : 0;

    if (len == 0)
        return 0;
    if (write(fd, line, len) == -1) {
        fprintf(stdout, "Error: write failed: %s\n", strerror(errno));
        return -1;
    }
    if (write(fd, "\r\n", 2) == -1) {
        fprintf(stdout, "Error: write failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
