/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** job_command_4.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_already_subscribed(team_t *team, const char *user_uid)
{
    return my_find_node(team->subscribers, (void *)user_uid,
        compare_string) != NULL;
}

static int auto_subscribe_creator(team_t *team, const char *creator_uid)
{
    char *creator_copy = strdup(creator_uid);

    if (!creator_copy)
        return 500;
    add_node(team->subscribers, creator_copy);
    return 0;
}

static int set_context(client_t *client, const char *team_uid,
    const char *channel_uid, const char *thread_uid)
{
    free(client->current_team_uid);
    free(client->current_channel_uid);
    free(client->current_thread_uid);
    client->current_team_uid = NULL;
    client->current_channel_uid = NULL;
    client->current_thread_uid = NULL;
    if (team_uid)
        client->current_team_uid = strdup(team_uid);
    if (channel_uid)
        client->current_channel_uid = strdup(channel_uid);
    if (thread_uid)
        client->current_thread_uid = strdup(thread_uid);
    return 0;
}

static int validate_use_context(payload_cmd_t *payload_cmd,
    const char *team_uid, const char *channel_uid, const char *thread_uid)
{
    team_t *team = NULL;
    channel_t *channel = NULL;

    if (!team_uid)
        return 0;
    team = my_find_node(payload_cmd->teams->teams,
        (void *)team_uid, compare_team_uid);
    if (!team)
        return 400;
    if (!channel_uid)
        return 0;
    channel = my_find_node(team->channels,
        (void *)channel_uid, compare_channel_uid);
    if (!channel)
        return 400;
    if (!thread_uid)
        return 0;
    if (!my_find_node(channel->threads,
        (void *)thread_uid, compare_thread_uid))
        return 400;
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
    const char *description, char **out_uid)
{
    team_t *team = NULL;
    channel_t *channel = NULL;

    if (!name || !description)
        return 501;
    team = my_find_node(payload_cmd->teams->teams,
        payload_cmd->client->current_team_uid, compare_team_uid);
    if (!team)
        return 400;
    channel = init_channel(name, description);
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

static int find_team_and_channel(payload_cmd_t *payload_cmd,
    team_t **team, channel_t **channel)
{
    *team = my_find_node(payload_cmd->teams->teams,
        (void *)payload_cmd->client->current_team_uid, compare_team_uid);
    if (!*team)
        return 400;
    if (!payload_cmd->client->current_channel_uid)
        return 0;
    *channel = my_find_node((*team)->channels,
        (void *)payload_cmd->client->current_channel_uid,
        compare_channel_uid);
    if (!*channel)
        return 400;
    return 0;
}

static int create_thread_message(payload_cmd_t *payload_cmd, const char *body, char **out_uid)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    thread_t *thread = NULL;
    message_t *message = NULL;
    int status = find_team_and_channel(payload_cmd, &team, &channel);

    (void)out_uid;
    if (status != 0)
        return status;
    thread = my_find_node(channel->threads,
        (void *)payload_cmd->client->current_thread_uid,
        compare_thread_uid);
    if (!thread)
        return 400;
    message = init_message(payload_cmd->client->user_uid, NULL, body);
    if (!message)
        return 500;
    add_node(thread->messages, message);
    return 0;
}

static int info_thread(payload_cmd_t *payload_cmd, char **out_info)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    thread_t *thread = NULL;
    char buffer[2048];
    int status = find_team_and_channel(payload_cmd, &team, &channel);

    if (status != 0)
        return status;
    thread = my_find_node(channel->threads,
        (void *)payload_cmd->client->current_thread_uid,
        compare_thread_uid);
    if (!thread)
        return 400;
    snprintf(buffer, sizeof(buffer), "%s %s %ld %s %s",
        thread->uid, thread->creator_uid, (long)thread->timestamp,
        thread->name, thread->body);
    *out_info = strdup(buffer);
    return *out_info ? 0 : 500;
}

static int info_channel(payload_cmd_t *payload_cmd, char **out_info)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    char buffer[2048];
    int status = find_team_and_channel(payload_cmd, &team, &channel);

    if (status != 0)
        return status;
    snprintf(buffer, sizeof(buffer), "%s %s %s",
        channel->uid, channel->name, channel->description);
    *out_info = strdup(buffer);
    return *out_info ? 0 : 500;
}

static int info_team(payload_cmd_t *payload_cmd, char **out_info)
{
    team_t *team = NULL;
    char buffer[2048];

    team = my_find_node(payload_cmd->teams->teams,
        (void *)payload_cmd->client->current_team_uid, compare_team_uid);
    if (!team)
        return 400;
    snprintf(buffer, sizeof(buffer), "%s %s %s",
        team->uid, team->name, team->description);
    *out_info = strdup(buffer);
    return *out_info ? 0 : 500;
}

static int list_subscribed_teams(payload_cmd_t *payload_cmd,
    linked_list_t **out_list)
{
    linked_list_t *filtered = instantiate_linked_list();
    linked_list_t *node = NULL;
    team_t *team = NULL;

    if (!filtered)
        return 500;
    for (node = payload_cmd->teams->teams; node && node->data;
            node = node->next) {
        team = node->data;
        if (is_already_subscribed(team, payload_cmd->client->user_uid))
            add_node(filtered, team);
    }
    *out_list = filtered;
    return 0;
}

static int list_team_channels(payload_cmd_t *payload_cmd,
    linked_list_t **out_list)
{
    team_t *team = NULL;

    team = my_find_node(payload_cmd->teams->teams,
        (void *)payload_cmd->client->current_team_uid, compare_team_uid);
    if (!team)
        return 400;
    *out_list = team->channels;
    return 0;
}

static int list_channel_threads(payload_cmd_t *payload_cmd,
    linked_list_t **out_list)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    int status = find_team_and_channel(payload_cmd, &team, &channel);

    if (status != 0)
        return status;
    *out_list = channel->threads;
    return 0;
}

static int list_thread_messages(payload_cmd_t *payload_cmd,
    linked_list_t **out_list)
{
    team_t *team = NULL;
    channel_t *channel = NULL;
    thread_t *thread = NULL;
    int status = find_team_and_channel(payload_cmd, &team, &channel);

    if (status != 0)
        return status;
    thread = my_find_node(channel->threads,
        (void *)payload_cmd->client->current_thread_uid,
        compare_thread_uid);
    if (!thread)
        return 400;
    *out_list = thread->messages;
    return 0;
}

int cmd_job_use(payload_cmd_t *payload_cmd, const char *team_uid,
    const char *channel_uid, const char *thread_uid)
{
    int status = 0;

    if (!payload_cmd->client->user_uid)
        return 300;
    status = validate_use_context(payload_cmd,
        team_uid, channel_uid, thread_uid);
    if (status != 0)
        return status;
    return set_context(payload_cmd->client,
        team_uid, channel_uid, thread_uid);
}

int cmd_job_create(payload_cmd_t *payload_cmd, const char *arg1,
    const char *arg2, char **out_uid)
{
    client_t *client = payload_cmd->client;

    if (!client->user_uid)
        return 300;
    if (client->current_thread_uid)
        return create_thread_message(payload_cmd, arg1, out_uid);
    if (client->current_channel_uid)
        return create_thread_ctx(payload_cmd, arg1, arg2, out_uid);
    if (client->current_team_uid)
        return create_channel_ctx(payload_cmd, arg1, arg2, out_uid);
    return create_team_ctx(payload_cmd, arg1, arg2, out_uid);
}

int cmd_job_list(payload_cmd_t *payload_cmd, linked_list_t **out_list)
{
    client_t *client = payload_cmd->client;

    if (!client->user_uid)
        return 300;
    if (client->current_thread_uid)
        return list_thread_messages(payload_cmd, out_list);
    if (client->current_channel_uid)
        return list_channel_threads(payload_cmd, out_list);
    if (client->current_team_uid)
        return list_team_channels(payload_cmd, out_list);
    return list_subscribed_teams(payload_cmd, out_list);
}

int cmd_job_info(payload_cmd_t *payload_cmd, char **out_info)
{
    client_t *client = payload_cmd->client;

    if (!client->user_uid)
        return 300;
    if (!client->current_team_uid)
        return 501;
    if (client->current_thread_uid)
        return info_thread(payload_cmd, out_info);
    if (client->current_channel_uid)
        return info_channel(payload_cmd, out_info);
    return info_team(payload_cmd, out_info);
}
