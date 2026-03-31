/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** send_function.c
*/

#include "infrastructure.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int send_simple_response(int client_fd, int code)
{
    char *response = call_server_response(code);

    if (write(client_fd, response, strlen(response)) == -1)
        return -1;
    return 0;
}

int send_login_succes(payload_cmd_t *payload_cmd, user_t *user)
{
    char response[128];
    int len = 0;

    if (!payload_cmd->logger_functions ||
        !payload_cmd->logger_functions->server_event_user_logged_in) {
        fprintf(stderr, "Error: Invalid logger in login success\n");
        return -1;
    }
    payload_cmd->logger_functions->server_event_user_logged_in(user->uid);
    len = snprintf(response, sizeof(response), "200 %s\r\n", user->uid);
    if (write(payload_cmd->client->client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}

int send_lougout_sucess(payload_cmd_t *payload_cmd)
{
    client_t *client = payload_cmd->client;
    char response[128];
    int len = 0;

    if (!payload_cmd->logger_functions ||
        !payload_cmd->logger_functions->server_event_user_logged_out) {
        fprintf(stderr, "Error: Invalid logger in logout success\n");
        return -1;
    }
    payload_cmd->logger_functions->server_event_user_logged_out(client->user_uid);
    len = snprintf(response, sizeof(response), "200 %s\r\n", client->user_uid);
    if (write(client->client_fd, response, (size_t)len) == -1)
        return -1;
    return 0;
}
