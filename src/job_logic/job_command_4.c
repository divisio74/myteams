/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** job_command_4.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdlib.h>
#include <string.h>

int cmd_job_use(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

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

static int store_and_log_team(payload_cmd_t *payload_cmd,
    const char *name, const char *description)
{
    team_t *team = init_team(name, description,
        payload_cmd->client->user_uid);
    char *creator_copy = NULL;

    if (!team)
        return 500;
    creator_copy = strdup(payload_cmd->client->user_uid);
    if (!creator_copy) {
        free_team(team);
        return 500;
    }
    add_node(team->subscribers, creator_copy);
    add_node(payload_cmd->teams->teams, team);
    payload_cmd->logger_functions->server_event_team_created(
        team->uid, team->name, team->creator_uid);
    payload_cmd->logger_functions->server_event_user_subscribed(
        team->uid, team->creator_uid);
    return 0;
}

int cmd_job_create(payload_cmd_t *payload_cmd)
{
    char *name = NULL;
    char *description = NULL;
    int status = 0;

    if (!payload_cmd->client->user_uid)
        return 300;
    status = parse_create_params(payload_cmd, &name, &description);
    if (status != 0)
        return status;
    status = store_and_log_team(payload_cmd, name, description);
    free(name);
    return status;
}

int cmd_job_list(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_job_info(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}
