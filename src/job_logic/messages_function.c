/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** messages_function.c
*/

#include "job_logic.h"
#include <stdlib.h>
#include <string.h>

message_t *init_message(const char *sender_uid, const char *receiver_uid,
    const char *body)
{
    message_t *msg = malloc(sizeof(message_t));

    if (!msg)
        return NULL;
    msg->sender_uid = NULL;
    msg->receiver_uid = NULL;
    msg->body = NULL;
    msg->timestamp = time(NULL);
    if (sender_uid)
        msg->sender_uid = strdup(sender_uid);
    if (receiver_uid)
        msg->receiver_uid = strdup(receiver_uid);
    if (body)
        msg->body = strdup(body);
    if ((sender_uid && !msg->sender_uid)
        || (receiver_uid && !msg->receiver_uid)
        || (body && !msg->body)) {
        free_message(msg);
        return NULL;
    }
    return msg;
}

void free_message(void *data)
{
    message_t *msg = data;

    if (!msg)
        return;
    free(msg->sender_uid);
    free(msg->receiver_uid);
    free(msg->body);
    free(msg);
}

int message_is_exchange_between(message_t *msg,
    const char *uid_a, const char *uid_b)
{
    int a_is_sender = 0;
    int b_is_sender = 0;
    int result = 0;

    if (!msg || !msg->sender_uid || !msg->receiver_uid || !uid_a || !uid_b)
        return 0;
    if (strcmp(msg->sender_uid, uid_a) == 0 &&
        strcmp(msg->receiver_uid, uid_b) == 0)
        a_is_sender = 1;
    if (strcmp(msg->sender_uid, uid_b) == 0
        && strcmp(msg->receiver_uid, uid_a) == 0)
        b_is_sender = 1;
    result = a_is_sender || b_is_sender;
    return result;
}
