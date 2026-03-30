/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** server_function
*/

#include "server_function.h"
#include "loader.h"
#include "teams_function.h"
#include <stdlib.h>
#include <string.h>

int server_init(server_t *server, char *path)
{
    if (!server)
        return -1;
    memset(server, 0, sizeof(*server));
    server->client_fds = calloc(MAX_SOCKET, sizeof(client_t));
    server->poll_fds = calloc(MAX_SOCKET, sizeof(struct pollfd));
    if (!server->client_fds || !server->poll_fds) {
        free(server->client_fds);
        free(server->poll_fds);
        server->client_fds = NULL;
        server->poll_fds = NULL;
        return -1;
    }
    if (teams_init(&server->teams) == -1) {
        free(server->client_fds);
        free(server->poll_fds);
        server->client_fds = NULL;
        server->poll_fds = NULL;
        return -1;
    }
    if (path && path[0] != '\0') {
        server->handle = load(path);
        if (!server->handle) {
            teams_free(&server->teams);
            free(server->client_fds);
            free(server->poll_fds);
            server->client_fds = NULL;
            server->poll_fds = NULL;
            return -1;
        }
    }
    return 0;
}

void server_free(server_t *server)
{
    if (!server)
        return;
    unload(server->handle);
    teams_free(&server->teams);
    free(server->client_fds);
    free(server->poll_fds);
    server->handle = NULL;
    server->client_fds = NULL;
    server->poll_fds = NULL;
}
