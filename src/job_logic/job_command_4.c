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

static int auto_subscribe_creator(team_t *team, const char *creator_uid)
{
    char *creator_copy = strdup(creator_uid);

    if (!creator_copy)
        return 500;
    add_node(team->subscribers, creator_copy);
    return 0;
}

int cmd_job_use(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

static int create_team_ctx(payload_cmd_t *payload_cmd, const char *name,
    const char *description, char **out_uid)
{
    team_t *team = NULL;
    int status = 0;

    if (!name || !description)
        return 501;
    team = init_team(name, description, payload_cmd->client->user_uid);
    if (!team)
        return 500;
    status = auto_subscribe_creator(team, payload_cmd->client->user_uid);
    if (status != 0) {
        free_team(team);
        return status;
    }
    add_node(payload_cmd->teams->teams, team);
    if (out_uid)
        *out_uid = strdup(team->uid);
    return 0;
}

static int create_channel_ctx(payload_cmd_t *payload_cmd, const char *name,
    char **out_uid)
{
    team_t *team = NULL;
    channel_t *channel = NULL;

    if (!name)
        return 501;
    team = my_find_node(payload_cmd->teams->teams,
        payload_cmd->client->current_team_uid, compare_team_uid);
    if (!team)
        return 400;
    channel = init_channel(name);
    if (!channel)
        return 500;
    add_node(team->channels, channel);
    if (out_uid)
        *out_uid = strdup(channel->uid);
    return 0;
}

static int create_thread_ctx(payload_cmd_t *payload_cmd, const char *title,
    const char *body, char **out_uid)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    thread_t *thread = NULL;

    if (!title || !body)
        return 501;
    team = my_find_node(payload_cmd->teams->teams,
        payload_cmd->client->current_team_uid, compare_team_uid);
    if (!team)
        return 400;
    channel = my_find_node(team->channels,
        payload_cmd->client->current_channel_uid, compare_channel_uid);
    if (!channel)
        return 400;
    thread = init_thread(title, body, payload_cmd->client->user_uid);
    if (!thread)
        return 500;
    add_node(channel->threads, thread);
    if (out_uid)
        *out_uid = strdup(thread->uid);
    return 0;
}

int cmd_job_create(payload_cmd_t *payload_cmd, const char *arg1,
    const char *arg2, char **out_uid)
{
    client_t *client = payload_cmd->client;

    if (!client->user_uid)
        return 300;
    if (client->current_thread_uid)
        return 501;
    if (client->current_channel_uid)
        return create_thread_ctx(payload_cmd, arg1, arg2, out_uid);
    if (client->current_team_uid)
        return create_channel_ctx(payload_cmd, arg1, out_uid);
    return create_team_ctx(payload_cmd, arg1, arg2, out_uid);
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
