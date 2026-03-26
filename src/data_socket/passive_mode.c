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

static int create_data_listener(size_t port)
{
    int data_listen_fd;

    data_listen_fd = socket_initiation((int)port);
    if (data_listen_fd == -1) {
        fprintf(stderr,
            "Erreur: Impossible de passer en mode passif sur le port %zu\n",
            port);
        return -1;
    }
    return data_listen_fd;
}

static int get_real_port(int data_listen_fd, int *real_port)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    if (data_listen_fd == -1 || real_port == NULL)
        return -1;
    if (getsockname(data_listen_fd, (struct sockaddr *)&addr, &len) == -1) {
        fprintf(stderr,
            "Erreur: Impossible de récupérer le vrai port passif: %s\n",
            strerror(errno));
        return -1;
    }
    *real_port = ntohs(addr.sin_port);
    return 0;
}

static char *get_ip(int sock)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char *ip_str;
    char *result_ip;

    if (getsockname(sock, (struct sockaddr *)&addr, &len) == -1) {
        fprintf(stderr, "Erreur: Impossible de récupérer l'IP du socket: %s\n",
            strerror(errno));
        return NULL;
    }
    ip_str = (char *)&addr.sin_addr;
    result_ip = malloc(5 * sizeof(char));
    result_ip[0] = ip_str[0];
    result_ip[1] = ip_str[1];
    result_ip[2] = ip_str[2];
    result_ip[3] = ip_str[3];
    result_ip[4] = '\0';
    return result_ip;
}

static int accept_data_client(int data_listen_fd, int *data_client_fd)
{
    struct sockaddr_in client;
    socklen_t len;

    if (data_client_fd == NULL)
        return -1;
    len = sizeof(client);
    *data_client_fd = accept(data_listen_fd,
        (struct sockaddr *) &client, &len);
    if (*data_client_fd == -1) {
        fprintf(stderr,
            "Erreur: Impossible d'accepter la connexion client: %s\n",
            strerror(errno));
        return -1;
    }
    return 0;
}

static int build_passive_response(char **response,
    char *client_ip, size_t port)
{
    if (asprintf(response, "227 Entering Passive Mode "
        "(%i,%i,%i,%i,%zu,%zu).\n"
        , client_ip[0], client_ip[1],
        client_ip[2], client_ip[3], port / 256, port % 256) == -1) {
        free(client_ip);
        return -1;
    }
    return 0;
}

static int send_passive_response(int client_fd, size_t port)
{
    char *client_ip;
    char *response = NULL;

    client_ip = get_ip(client_fd);
    if (!client_ip) {
        fprintf(stderr,
            "Erreur: Impossible de récupérer l'IP du "
            "client pour le mode passif.\n");
        return -1;
    }
    if (build_passive_response(&response, client_ip, port) == -1)
        return -1;
    if (write(client_fd, response, strlen(response)) == -1) {
        free(response);
        free(client_ip);
        return -1;
    }
    free(response);
    free(client_ip);
    return 0;
}

static int fetch_and_send_passive_response(int data_listen_fd, int client_fd)
{
    int real_port = 0;

    if (get_real_port(data_listen_fd, &real_port) == -1) {
        close(data_listen_fd);
        return -1;
    }
    if (send_passive_response(client_fd, (size_t)real_port) == -1) {
        close(data_listen_fd);
        return -1;
    }
    return 0;
}

int passive_mode(client_state_t *client_state, size_t port, int client_fd)
{
    int data_listen_fd;

    data_listen_fd = create_data_listener(port);
    if (data_listen_fd == -1)
        return -1;
    if (fetch_and_send_passive_response(data_listen_fd, client_fd) == -1)
        return -1;
    if (client_state->data_fd > 0) {
        close(client_state->data_fd);
        client_state->data_fd = 0;
    }
    if (accept_data_client(
        data_listen_fd, &client_state->data_fd) == -1) {
        close(data_listen_fd);
        return -1;
    }
    return 0;
}
