/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_4.c
*/

#include "infrastructure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int parse_create_params(payload_cmd_t *payload_cmd,
    char **name, char **description)
{
    if (!payload_cmd->parameter)
        return 501;
    split_with_space(payload_cmd->parameter, name, description);
    if (!*name || !*description) {
        free(*name);
        return 501;
    }
    return 0;
}

static int log_team_creation(payload_cmd_t *payload_cmd, team_t *team)
{
    payload_cmd->logger_functions->server_event_team_created(
        team->uid, team->name, team->creator_uid);
    payload_cmd->logger_functions->server_event_user_subscribed(
        team->uid, team->creator_uid);
    return 0;
}

static int send_create_success(int client_fd, team_t *team)
{
    char response[128];
    int len = snprintf(response, sizeof(response), "200 %s\r\n", team->uid);

    if (write(client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}

int cmd_use(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_create(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *name = NULL;
    char *description = NULL;
    team_t *team = NULL;
    int status = 0;

    status = parse_create_params(payload_cmd, &name, &description);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = cmd_job_create(payload_cmd, name, description, &team);
    free(name);
    if (status != 0)
        return send_simple_response(client_fd, status);
    log_team_creation(payload_cmd, team);
    return send_create_success(client_fd, team);
}

int cmd_list(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_info(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}
