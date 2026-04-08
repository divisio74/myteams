/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_4.c
*/

#include "infrastructure.h"

int cmd_use(payload_cmd_t *payload_cmd)
{
    (void)payload_cmd;
    return 0;
}

int cmd_create(payload_cmd_t *payload_cmd)
{
    int status = cmd_job_create(payload_cmd);

    return send_simple_response(payload_cmd->client->client_fd,
        status == 0 ? 200 : status);
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
