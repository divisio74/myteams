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
#include <time.h>
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

    if (client->current_thread_uid) {
        log->server_event_reply_created(client->current_thread_uid,
            client->user_uid, arg1);
        return;
    }
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

static int send_create_response(payload_cmd_t *payload_cmd,
    const char *arg1, const char *arg2, const char *new_uid)
{
    client_t *c = payload_cmd->client;
    char resp[2048];
    int len = 0;

    if (c->current_thread_uid)
        len = snprintf(resp, sizeof(resp), "200 %s %s %ld %s\r\n",
            c->current_thread_uid, c->user_uid, (long)time(NULL), arg1);
    else if (c->current_channel_uid)
        len = snprintf(resp, sizeof(resp), "200 %s %s %ld %s %s\r\n",
            new_uid, c->user_uid, (long)time(NULL), arg1, arg2);
    else
        len = snprintf(resp, sizeof(resp), "200 %s %s %s\r\n",
            new_uid, arg1, arg2);
    if (write(c->client_fd, resp, (size_t)len) == -1)
        return -1;
    return 0;
}

static void parse_use_args(payload_cmd_t *payload_cmd,
    char **team_uid, char **channel_uid, char **thread_uid)
{
    char *rest = NULL;

    *team_uid = NULL;
    *channel_uid = NULL;
    *thread_uid = NULL;
    if (!payload_cmd->parameter)
        return;
    split_with_space(payload_cmd->parameter, team_uid, &rest);
    if (!rest)
        return;
    split_with_space(rest, channel_uid, thread_uid);
}

static int parse_create_args(payload_cmd_t *payload_cmd,
    char **arg1, char **arg2)
{
    client_t *client = payload_cmd->client;

    if (client->current_thread_uid)
        return parse_one_arg(payload_cmd, arg1);
    if (client->current_channel_uid)
        return parse_two_args(payload_cmd, arg1, arg2);
    if (client->current_team_uid)
        return parse_two_args(payload_cmd, arg1, arg2);
    return parse_two_args(payload_cmd, arg1, arg2);
}

static int write_list_line(int fd, void *data, client_t *client)
{
    char line[1024];
    int len = 0;

    if (client->current_thread_uid) {
        message_t *msg = data;
        len = snprintf(line, sizeof(line), "  %s %s %ld %s\r\n",
            client->current_thread_uid, msg->sender_uid,
            (long)msg->timestamp, msg->body);
    } else if (client->current_channel_uid) {
        thread_t *th = data;
        len = snprintf(line, sizeof(line), "  %s %s %ld %s %s\r\n",
            th->uid, th->creator_uid, (long)th->timestamp,
            th->name, th->body);
    } else if (client->current_team_uid) {
        channel_t *ch = data;
        len = snprintf(line, sizeof(line), "  %s %s %s\r\n",
            ch->uid, ch->name, ch->description);
    } else {
        team_t *team = data;
        len = snprintf(line, sizeof(line), "  %s %s %s\r\n",
            team->uid, team->name, team->description);
    }
    if (len < 0 || write(fd, line, (size_t)len) == -1)
        return -1;
    return 0;
}

static int write_list_response(int fd, linked_list_t *list, client_t *client)
{
    if (write(fd, "200 [\r\n", 7) == -1)
        return -1;
    for (linked_list_t *node = list; node && node->data; node = node->next) {
        if (write_list_line(fd, node->data, client) == -1)
            return -1;
    }
    if (write(fd, "]\r\n", 3) == -1)
        return -1;
    return 0;
}

static void free_filtered_list_nodes(linked_list_t *list)
{
    linked_list_t *current = list;
    linked_list_t *next = NULL;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

int cmd_use(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *team_uid = NULL;
    char *channel_uid = NULL;
    char *thread_uid = NULL;
    int status = 0;

    parse_use_args(payload_cmd, &team_uid, &channel_uid, &thread_uid);
    status = cmd_job_use(payload_cmd,
        team_uid, channel_uid, thread_uid);
    free(team_uid);
    return send_simple_response(client_fd, status == 0 ? 200 : status);
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
    status = send_create_response(payload_cmd, arg1, arg2, new_uid);
    free(arg1);
    free(new_uid);
    return status;
}

int cmd_list(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    linked_list_t *list = NULL;
    int status = 0;

    status = cmd_job_list(payload_cmd, &list);
    if (status != 0)
        return send_simple_response(client_fd, status);
    status = write_list_response(client_fd, list, payload_cmd->client);
    if (!payload_cmd->client->current_team_uid)
        free_filtered_list_nodes(list);
    return status;
}

int cmd_info(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *info = NULL;
    char response[2048];
    int status = 0;
    int len = 0;

    status = cmd_job_info(payload_cmd, &info);
    if (status != 0)
        return send_simple_response(client_fd, status);
    len = snprintf(response, sizeof(response), "200 %s\r\n", info);
    free(info);
    if (write(client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}
