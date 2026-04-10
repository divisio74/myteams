/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_5.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int write_subscriber_line(int client_fd, char *uid)
{
    char line[256];
    int len = snprintf(line, sizeof(line), "  %s\r\n", uid);

    if (len < 0 || write(client_fd, line, (size_t)len) == -1)
        return -1;
    return 0;
}

static int write_team_line(int client_fd, team_t *team)
{
    char line[512];
    int len = snprintf(line, sizeof(line), "  %s %s\r\n",
        team->uid, team->name);

    if (len < 0 || write(client_fd, line, (size_t)len) == -1)
        return -1;
    return 0;
}

static int write_subscribed_list(int client_fd,
    linked_list_t *filtered, int is_team_list)
{
    if (write(client_fd, "200 [\r\n", 7) == -1)
        return -1;
    for (linked_list_t *node = filtered; node && node->data;
            node = node->next) {
        if (is_team_list) {
            if (write_team_line(client_fd, node->data) == -1)
                return -1;
        } else {
            if (write_subscriber_line(client_fd, node->data) == -1)
                return -1;
        }
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

int cmd_subscribe(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    int status = 0;

    if (!payload_cmd->parameter)
        return send_simple_response(client_fd, 501);
    status = cmd_job_subscribe(payload_cmd, payload_cmd->parameter);
    if (status != 0)
        return send_simple_response(client_fd, status);
    payload_cmd->logger_functions->server_event_user_subscribed(
        payload_cmd->parameter, payload_cmd->client->user_uid);
    return send_simple_response(client_fd, 200);
}

int cmd_subscribed(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    linked_list_t *filtered = NULL;
    int is_team_list = (payload_cmd->parameter == NULL);
    int status = 0;

    status = cmd_job_subscribed(payload_cmd, payload_cmd->parameter,
        &filtered);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = write_subscribed_list(client_fd, filtered, is_team_list);
    if (is_team_list)
        free_filtered_list(filtered);
    return status;
}

int cmd_unsubscribe(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    int status = 0;

    if (!payload_cmd->parameter)
        return send_simple_response(client_fd, 501);
    status = cmd_job_unsubscribe(payload_cmd, payload_cmd->parameter);
    if (status != 0)
        return send_simple_response(client_fd, status);
    payload_cmd->logger_functions->server_event_user_unsubscribed(
        payload_cmd->parameter, payload_cmd->client->user_uid);
    return send_simple_response(client_fd, 200);
}
