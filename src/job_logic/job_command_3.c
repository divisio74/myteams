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

static int is_exchange_between(message_t *msg,
    const char *uid_a, const char *uid_b)
{
    return (strcmp(msg->sender_uid, uid_a) == 0
        && strcmp(msg->receiver_uid, uid_b) == 0)
        || (strcmp(msg->sender_uid, uid_b) == 0
        && strcmp(msg->receiver_uid, uid_a) == 0);
}

static void message_builder(char *buffer, message_t *msg)
{
    strcat(buffer, "  ");
    strcat(buffer, msg->sender_uid);
    strcat(buffer, " ");
    strcat(buffer, msg->body);
    strcat(buffer, "\r\n");
}

static int build_messages_buffer(linked_list_t *messages,
    const char *uid_a, const char *uid_b, char *buffer)
{
    message_t *msg = NULL;

    buffer[0] = '\0';
    for (linked_list_t *node = messages; node; node = node->next) {
        msg = node->data;
        if (!msg)
            return 500;
        if (is_exchange_between(msg, uid_a, uid_b))
            message_builder(buffer, msg);
    }
    return 0;
}

int cmd_job_messages(payload_cmd_t *payload_cmd, const char *other_uid,
    char **messages_output)
{
    user_t *other = NULL;
    char buffer[4048];
    int status = 0;

    if (!payload_cmd->client->user_uid)
        return 300;
    other = my_find_node(payload_cmd->teams->users, (void *)other_uid,
        compare_user_uid);
    if (!other)
        return 400;
    status = build_messages_buffer(payload_cmd->teams->messages,
        payload_cmd->client->user_uid, other->uid, buffer);
    if (status != 0)
        return status;
    *messages_output = strdup(buffer);
    if (!*messages_output)
        return 500;
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
