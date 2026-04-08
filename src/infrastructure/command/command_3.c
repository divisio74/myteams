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

static int parse_send_params(payload_cmd_t *payload_cmd,
    char **operant, char **parameters)
{
    if (!payload_cmd->parameter)
        return 501;
    split_with_space(payload_cmd->parameter, operant, parameters);
    if (!*operant || !*parameters) {
        free(*operant);
        return 501;
    }
    return 0;
}

int cmd_send(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *operant = NULL;
    char *parameters = NULL;
    int status = 0;

    status = parse_send_params(payload_cmd, &operant, &parameters);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = cmd_job_send(payload_cmd, operant, parameters);
    if (status == 0)
        payload_cmd->logger_functions->server_event_private_message_sended(
            payload_cmd->client->user_uid, operant, parameters);
    free(operant);
    if (status != 0)
        return send_simple_response(client_fd, status);
    return send_simple_response(client_fd, 200);
}

static int write_messages_response(int client_fd, char *messages_output)
{
    size_t out_len = messages_output ? strlen(messages_output) : 0;

    if (write(client_fd, "200 [\r\n", 7) == -1
        || (out_len > 0 && write(client_fd, messages_output, out_len) == -1)
        || write(client_fd, "\r\n]\r\n", 5) == -1)
        return -1;
    return 0;
}

int cmd_messages(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *messages_output = NULL;
    int status = 0;

    if (!payload_cmd->parameter)
        return send_simple_response(client_fd, 501);
    status = cmd_job_messages(payload_cmd, payload_cmd->parameter,
        &messages_output);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = write_messages_response(client_fd, messages_output);
    free(messages_output);
    return status;
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
