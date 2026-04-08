/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_3.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int cmd_send(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    int status = 0;

    status = cmd_job_send(payload_cmd);
    if (status != 0)
        return send_simple_response(client_fd, status);
    return send_simple_response(client_fd, 200);
}

int cmd_messages(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *messages_output = NULL;
    size_t out_len = 0;
    int status = 0;

    status = cmd_job_messages(payload_cmd, &messages_output);
    if (status != 0)
        return send_simple_response(client_fd, status);
    out_len = messages_output ? strlen(messages_output) : 0;
    if (write(client_fd, "200 [\r\n", 7) == -1
        || (out_len > 0 && write(client_fd, messages_output, out_len) == -1)
        || write(client_fd, "\r\n]\r\n", 5) == -1) {
        free(messages_output);
        return -1;
    }
    free(messages_output);
    return 0;
}

int cmd_subscribe(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_subscribed(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_unsubscribe(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}
