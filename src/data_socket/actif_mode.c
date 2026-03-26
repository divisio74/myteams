/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** passive_mode.c
*/

#define _GNU_SOURCE

#include "socket.h"
#include "struct.h"
#include "handling_connection.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int connect_to_client(int fd, client_state_t *client_state)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(client_state->data_port);
    addr.sin_addr.s_addr = inet_addr(client_state->data_ip);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Erreur: Impossible de se connecter au client: %s\n",
            strerror(errno));
        close(fd);
        return -1;
    }
    return 0;
}

int active_mode(client_state_t *client_state)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "Erreur: Impossible de créer le socket: %s\n",
            strerror(errno));
        return -1;
    }
    if (socket_bind(fd, client_state->data_port) == -1) {
        close(fd);
        return -1;
    }
    if (connect_to_client(fd, client_state) == -1)
        return -1;
    if (client_state->data_fd > 0)
        close(client_state->data_fd);
    client_state->data_fd = fd;
    return 0;
}
