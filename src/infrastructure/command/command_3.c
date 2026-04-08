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

static int write_message_line(int client_fd, message_t *msg)
{
    char line[1024];
    int len = snprintf(line, sizeof(line), "  %s %s\r\n",
        msg->sender_uid, msg->body);

    if (len < 0 || write(client_fd, line, (size_t)len) == -1)
        return -1;
    return 0;
}

static int write_filtered_messages(int client_fd, linked_list_t *filtered)
{
    if (write(client_fd, "200 [\r\n", 7) == -1)
        return -1;
    for (linked_list_t *node = filtered; node && node->data;
            node = node->next) {
        if (write_message_line(client_fd, node->data) == -1)
            return -1;
    }
    if (write(client_fd, "]\r\n", 3) == -1)
        return -1;
    return 0;
}

static void free_filtered_list(linked_list_t *filtered)
{
    linked_list_t *current = filtered;
    linked_list_t *next = NULL;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

int cmd_messages(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    linked_list_t *filtered = NULL;
    int status = 0;

    if (!payload_cmd->parameter)
        return send_simple_response(client_fd, 501);
    status = cmd_job_messages(payload_cmd, payload_cmd->parameter, &filtered);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = write_filtered_messages(client_fd, filtered);
    free_filtered_list(filtered);
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
