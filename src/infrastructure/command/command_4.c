/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_4.c
*/

#include "infrastructure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int parse_two_args(payload_cmd_t *payload_cmd,
    char **arg1, char **arg2)
{
    if (!payload_cmd->parameter)
        return 501;
    split_with_space(payload_cmd->parameter, arg1, arg2);
    if (!*arg1 || !*arg2) {
        free(*arg1);
        *arg1 = NULL;
        return 501;
    }
    return 0;
}

static int parse_one_arg(payload_cmd_t *payload_cmd, char **arg1)
{
    if (!payload_cmd->parameter || !*payload_cmd->parameter)
        return 501;
    *arg1 = strdup(payload_cmd->parameter);
    if (!*arg1)
        return 500;
    return 0;
}

static void log_create_event(payload_cmd_t *payload_cmd,
    const char *arg1, const char *arg2, const char *new_uid)
{
    client_t *client = payload_cmd->client;
    logger_functions_t *log = payload_cmd->logger_functions;

    if (client->current_thread_uid)
        return;
    if (client->current_channel_uid) {
        log->server_event_thread_created(client->current_channel_uid,
            new_uid, client->user_uid, arg1, arg2);
        return;
    }
    if (client->current_team_uid) {
        log->server_event_channel_created(client->current_team_uid,
            new_uid, arg1);
        return;
    }
    log->server_event_team_created(new_uid, arg1, client->user_uid);
    log->server_event_user_subscribed(new_uid, client->user_uid);
}

static int send_create_success(int client_fd, const char *new_uid)
{
    char response[128];
    int len = snprintf(response, sizeof(response), "200 %s\r\n", new_uid);

    if (write(client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}

int cmd_use(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

static int parse_create_args(payload_cmd_t *payload_cmd,
    char **arg1, char **arg2)
{
    client_t *client = payload_cmd->client;

    if (client->current_channel_uid && !client->current_thread_uid)
        return parse_two_args(payload_cmd, arg1, arg2);
    if (client->current_team_uid)
        return parse_one_arg(payload_cmd, arg1);
    return parse_two_args(payload_cmd, arg1, arg2);
}

int cmd_create(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *new_uid = NULL;
    int status = 0;

    status = parse_create_args(payload_cmd, &arg1, &arg2);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = cmd_job_create(payload_cmd, arg1, arg2, &new_uid);
    if (status != 0) {
        free(arg1);
        return send_simple_response(client_fd, status);
    }
    log_create_event(payload_cmd, arg1, arg2, new_uid);
    free(arg1);
    status = send_create_success(client_fd, new_uid);
    free(new_uid);
    return status;
}

int cmd_list(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_info(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}
