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

static int auto_subscribe_creator(team_t *team, const char *creator_uid)
{
    char *creator_copy = strdup(creator_uid);

    if (!creator_copy)
        return 500;
    add_node(team->subscribers, creator_copy);
    return 0;
}

int cmd_job_create(payload_cmd_t *payload_cmd, const char *name,
    const char *description, team_t **out_team)
{
    team_t *team = NULL;
    int status = 0;

    if (!payload_cmd->client->user_uid)
        return 300;
    team = init_team(name, description, payload_cmd->client->user_uid);
    if (!team)
        return 500;
    status = auto_subscribe_creator(team, payload_cmd->client->user_uid);
    if (status != 0) {
        free_team(team);
        return status;
    }
    add_node(payload_cmd->teams->teams, team);
    if (out_team)
        *out_team = team;
    return 0;
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
