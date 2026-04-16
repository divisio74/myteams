/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** main.c
*/

#include "client.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char HELP_MSG[] = "USAGE: ./myteams_cli ip port\n";

static int parse_port(const char *port_str, int *port_out)
{
    char *end = NULL;
    long port = 0;

    errno = 0;
    port = strtol(port_str, &end, 10);
    if (errno != 0 || !end || *end != '\0' || port <= 0 || port > 65535) {
        fprintf(stdout, "Error: invalid port '%s'.\n", port_str);
        return -1;
    }
    *port_out = (int)port;
    return 0;
}

static int do_poll(client_t *client, struct pollfd *pfds)
{
    int ret = 0;

    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    pfds[1].fd = client->socket_fd;
    pfds[1].events = POLLIN;
    ret = poll(pfds, 2, -1);
    if (ret == -1) {
        fprintf(stdout, "Error: poll failed: %s\n", strerror(errno));
        return -1;
    }
    return ret;
}

static int check_events(client_t *client, struct pollfd *pfds)
{
    if (pfds[1].revents & POLLIN) {
        if (handle_socket_read(client) == -1)
            return -1;
    }
    if (!client->running)
        return 0;
    if (pfds[0].revents & POLLIN) {
        if (handle_stdin_read(client) == -1)
            return -1;
    }
    return 0;
}

static void free_client(client_t *client)
{
    if (client->socket_fd != -1)
        close(client->socket_fd);
}

int run_client(const char *ip, int port)
{
    client_t client;
    struct pollfd pfds[2];

    memset(&client, 0, sizeof(client));
    client.socket_fd = connect_to_server(ip, port);
    if (client.socket_fd == -1)
        return 84;
    client.running = true;
    while (client.running) {
        if (do_poll(&client, pfds) == -1)
            break;
        if (check_events(&client, pfds) == -1)
            break;
    }
    free_client(&client);
    return 0;
}

int main(int argc, char **argv)
{
    int port = 0;

    if (argc == 2 && (strcmp(argv[1], "-h") == 0
            || strcmp(argv[1], "--help") == 0)) {
        fprintf(stdout, "%s", HELP_MSG);
        return 0;
    }
    if (argc != 3) {
        fprintf(stdout, "%s", HELP_MSG);
        return 84;
    }
    if (parse_port(argv[2], &port) == -1)
        return 84;
    return run_client(argv[1], port);
}
