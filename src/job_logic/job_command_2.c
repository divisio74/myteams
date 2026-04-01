/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** job_command_2.c
*/

#include "infrastructure.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_job_help(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_job_login(payload_cmd_t *payload_cmd)
{
    user_t *user = NULL;

    if (!payload_cmd->parameter)
        return 501;
    user = find_or_create_user(payload_cmd->teams, payload_cmd->parameter);
    if (!user)
        return 500;
    free(payload_cmd->client->user_uid);
    payload_cmd->client->user_uid = strdup(user->uid);
    if (!payload_cmd->client->user_uid)
        return 500;
    return 0;
}

int cmd_job_logout(payload_cmd_t *payload_cmd, char **logged_out_uid)
{
    *logged_out_uid = NULL;
    if (!payload_cmd->client->user_uid)
        return 300;
    *logged_out_uid = strdup(payload_cmd->client->user_uid);
    if (!*logged_out_uid)
        return 500;
    free(payload_cmd->client->user_uid);
    payload_cmd->client->user_uid = NULL;
    return 0;
}

int cmd_job_users(payload_cmd_t *payload_cmd, char **users_output)
{
    user_t *user = NULL;
    char buffer[4048];

    buffer[0] = '\0';
    for (linked_list_t *node = payload_cmd->teams->users;
            node; node = node->next) {
        user = node->data;
        if (!user || !user->uid || !user->name)
            return 500;
        strcat(buffer, "  ");
        strcat(buffer, user->name);
        strcat(buffer, " ");
        strcat(buffer, user->uid);
    }
    strcat(buffer, " \r\n");
    *users_output = strdup(buffer);
    return 0;
}

int cmd_job_user(payload_cmd_t *payload_cmd, char **user_output)
{
    char buffer[4048];
    user_t *user = NULL;

    user = my_find_node(payload_cmd->teams->users, payload_cmd->parameter,
        compare_user_uid);
    if (!user || !user->uid || !user->name)
        return 400;
    buffer[0] = '\0';
    strcat(buffer, user->name);
    strcat(buffer, " ");
    strcat(buffer, user->uid);
    *user_output = strdup(buffer);
    if (!*user_output)
        return 500;
    return 0;
}
