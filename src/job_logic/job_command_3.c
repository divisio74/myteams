/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** job_command_3.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void filter_exchange(linked_list_t *messages,
    const char *uid_a, const char *uid_b, linked_list_t *out)
{
    message_t *msg = NULL;

    for (linked_list_t *node = messages; node && node->data;
            node = node->next) {
        msg = node->data;
        if (message_is_exchange_between(msg, uid_a, uid_b))
            add_node(out, msg);
    }
}

static int is_already_subscribed(team_t *team, const char *user_uid)
{
    return my_find_node(team->subscribers, (void *)user_uid,
        compare_string) != NULL;
}

static void filter_subscribed_teams(linked_list_t *teams,
    const char *user_uid, linked_list_t *out)
{
    team_t *team = NULL;

    for (linked_list_t *node = teams; node && node->data;
            node = node->next) {
        team = node->data;
        if (is_already_subscribed(team, user_uid))
            add_node(out, team);
    }
}

int cmd_job_send(payload_cmd_t *payload_cmd, const char *receiver_uid,
    const char *body)
{
    user_t *receiver = NULL;
    message_t *msg = NULL;

    if (!payload_cmd->client->user_uid)
        return 300;
    receiver = my_find_node(payload_cmd->teams->users, (void *)receiver_uid,
        compare_user_uid);
    if (!receiver)
        return 400;
    msg = init_message(payload_cmd->client->user_uid, receiver->uid, body);
    if (!msg)
        return 500;
    add_node(payload_cmd->teams->messages, msg);
    return 0;
}

int cmd_job_messages(payload_cmd_t *payload_cmd, const char *other_uid,
    linked_list_t **out_filtered)
{
    user_t *other = NULL;
    linked_list_t *filtered = NULL;

    if (!payload_cmd->client->user_uid)
        return 300;
    other = my_find_node(payload_cmd->teams->users, (void *)other_uid,
        compare_user_uid);
    if (!other)
        return 400;
    filtered = instantiate_linked_list();
    if (!filtered)
        return 500;
    filter_exchange(payload_cmd->teams->messages,
        payload_cmd->client->user_uid, other->uid, filtered);
    *out_filtered = filtered;
    return 0;
}

int cmd_job_subscribe(payload_cmd_t *payload_cmd, const char *team_uid)
{
    team_t *team = NULL;
    char *uid_copy = NULL;

    if (!payload_cmd->client->user_uid)
        return 300;
    team = my_find_node(payload_cmd->teams->teams, (void *)team_uid,
        compare_team_uid);
    if (!team)
        return 400;
    if (is_already_subscribed(team, payload_cmd->client->user_uid))
        return 0;
    uid_copy = strdup(payload_cmd->client->user_uid);
    if (!uid_copy)
        return 500;
    add_node(team->subscribers, uid_copy);
    return 0;
}

int cmd_job_subscribed(payload_cmd_t *payload_cmd, const char *team_uid,
    linked_list_t **out_filtered)
{
    team_t *team = NULL;
    linked_list_t *filtered = NULL;

    if (!payload_cmd->client->user_uid)
        return 300;
    filtered = instantiate_linked_list();
    if (!filtered)
        return 500;
    if (team_uid) {
        team = my_find_node(payload_cmd->teams->teams, (void *)team_uid,
            compare_team_uid);
        if (!team) {
            free(filtered);
            return 400;
        }
        *out_filtered = team->subscribers;
        free(filtered);
        return 0;
    }
    filter_subscribed_teams(payload_cmd->teams->teams,
        payload_cmd->client->user_uid, filtered);
    *out_filtered = filtered;
    return 0;
}

int cmd_job_unsubscribe(payload_cmd_t *payload_cmd, const char *team_uid)
{
    team_t *team = NULL;

    if (!payload_cmd->client->user_uid)
        return 300;
    team = my_find_node(payload_cmd->teams->teams, (void *)team_uid,
        compare_team_uid);
    if (!team)
        return 400;
    if (!is_already_subscribed(team, payload_cmd->client->user_uid))
        return 400;
    my_delete_nodes(team->subscribers,
        (void *)payload_cmd->client->user_uid, compare_string);
    return 0;
}
