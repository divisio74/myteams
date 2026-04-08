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

int cmd_job_subscribe(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_job_subscribed(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_job_unsubscribe(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}
