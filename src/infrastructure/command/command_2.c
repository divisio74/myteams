/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_2.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int cmd_help(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_login(payload_cmd_t *payload_cmd)
{
    client_t *client = payload_cmd->client;
    user_t *user = NULL;
    int status = 0;

    status = cmd_job_login(payload_cmd);
    if (status != 0)
        return send_simple_response(client->client_fd, status);
    user = my_find_node(payload_cmd->teams->users, client->user_uid,
        compare_user_uid);
    if (!user || !user->uid) {
        return send_simple_response(client->client_fd, 500);
    }
    return send_login_succes(payload_cmd, user);
}

int cmd_logout(payload_cmd_t *payload_cmd)
{
    client_t *client = payload_cmd->client;
    char *logged_out_uid = NULL;
    char response[128];
    int len = 0;
    int status = 0;

    status = cmd_job_logout(payload_cmd, &logged_out_uid);
    if (status != 0)
        return send_simple_response(client->client_fd, status);
    payload_cmd->logger_functions->
    server_event_user_logged_out(logged_out_uid);
    len = snprintf(response, sizeof(response), "200 %s\r\n", logged_out_uid);
    free(logged_out_uid);
    if (len < 0 || write(client->client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}

int cmd_users(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *users_output = NULL;
    size_t out_len = 0;
    int status = 0;

    status = cmd_job_users(payload_cmd, &users_output);
    if (status != 0)
        return send_simple_response(client_fd, status);
    out_len = users_output ? strlen(users_output) : 0;
    if (write(client_fd, "200 [\r\n", 7) == -1
        || (out_len > 0 && write(client_fd, users_output, out_len) == -1)
        || write(client_fd, "]\r\n", 3) == -1) {
        free(users_output);
        return -1;
    }
    free(users_output);
    return 0;
}

int cmd_user(payload_cmd_t *payload_cmd)
{
    int client_fd = payload_cmd->client->client_fd;
    char *user_output = NULL;
    char response[512];
    int len = 0;
    int status = 0;

    status = cmd_job_user(payload_cmd, &user_output);
    if (status != 0)
        return send_simple_response(client_fd, status);
    len = snprintf(response, sizeof(response), "200 %s\r\n",
        user_output ? user_output : "");
    free(user_output);
    if (len < 0 || write(client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}
