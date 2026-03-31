/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** server_function
*/

#include "infrastructure.h"
#include <stdlib.h>
#include <string.h>

static void free_server_allocations(server_t *server)
{
    free(server->client_fds);
    free(server->poll_fds);
    server->client_fds = NULL;
    server->poll_fds = NULL;
}

static int allocate_server_arrays(server_t *server)
{
    server->client_fds = calloc(MAX_SOCKET, sizeof(client_t));
    server->poll_fds = calloc(MAX_SOCKET, sizeof(struct pollfd));
    if (!server->client_fds || !server->poll_fds) {
        free_server_allocations(server);
        return -1;
    }
    return 0;
}

static void init_payload_cmd(server_t *server)
{
    server->payload_cmd.teams = &server->teams;
    server->payload_cmd.logger = NULL;
    server->payload_cmd.client = NULL;
    server->payload_cmd.args = NULL;
    server->payload_cmd.current_command = NULL;
    server->payload_cmd.parameter = NULL;
    server->payload_cmd.logger_functions = NULL;
}

static int load_server_logger(server_t *server, char *path)
{
    if (!path || path[0] == '\0')
        return 0;
    server->logger = load(path);
    if (!server->logger)
        return -1;
    return 0;
}

static int init_server_resources(server_t *server)
{
    if (allocate_server_arrays(server) == -1)
        return -1;
    if (teams_init(&server->teams) == -1) {
        free_server_allocations(server);
        return -1;
    }
    init_payload_cmd(server);
    return 0;
}

int init_logger(server_t *server)
{
    server->payload_cmd.logger = server->logger;
    if (init_logger_functions(&server->payload_cmd) == -1)
        return -1;
    return 0;
}

int server_init(server_t *server, char *path)
{
    if (!server)
        return -1;
    memset(server, 0, sizeof(*server));
    if (init_server_resources(server) == -1)
        return -1;
    if (load_server_logger(server, path) == -1) {
        return -1;
    }
    if (init_logger(server) == -1) {
        return -1;
    }
    return 0;
}

void server_free(server_t *server)
{
    if (!server)
        return;
    unload(server->logger);
    teams_free(&server->teams);
    free(server->client_fds);
    free(server->poll_fds);
    free(server->payload_cmd.logger_functions);
}
