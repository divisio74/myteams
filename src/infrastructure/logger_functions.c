/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** logger_functions
*/

#include "infrastructure.h"
#include <stdlib.h>

static int load_logger_symbol(payload_cmd_t *payload_cmd,
    void **destination, const char *symbol)
{
    *destination = getfunction_by_name(&payload_cmd->logger, symbol);
    if (!*destination)
        return -1;
    return 0;
}

static int init_logger_user_events(payload_cmd_t *payload_cmd,
    logger_functions_t *fns)
{
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_subscribed,
        "server_event_user_subscribed") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_unsubscribed,
        "server_event_user_unsubscribed") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_created,
        "server_event_user_created") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_loaded,
        "server_event_user_loaded") == -1)
        return -1;
    return 0;
}

static int init_logger_session_events(payload_cmd_t *payload_cmd,
    logger_functions_t *fns)
{
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_logged_in,
        "server_event_user_logged_in") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_user_logged_out,
        "server_event_user_logged_out") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_private_message_sended,
        "server_event_private_message_sended") == -1)
        return -1;
    return 0;
}

static int init_logger_structure_events(payload_cmd_t *payload_cmd,
    logger_functions_t *fns)
{
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_team_created,
        "server_event_team_created") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_channel_created,
        "server_event_channel_created") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_thread_created,
        "server_event_thread_created") == -1)
        return -1;
    if (load_logger_symbol(payload_cmd,
        (void **)&fns->server_event_reply_created,
        "server_event_reply_created") == -1)
        return -1;
    return 0;
}

int init_logger_functions(payload_cmd_t *payload_cmd)
{
    logger_functions_t *fns = NULL;

    if (!payload_cmd || !payload_cmd->logger)
        return 0;
    fns = calloc(1, sizeof(*fns));
    if (!fns)
        return -1;
    if (init_logger_structure_events(payload_cmd, fns) == -1 ||
        init_logger_user_events(payload_cmd, fns) == -1 ||
        init_logger_session_events(payload_cmd, fns) == -1) {
        free(fns);
        return -1;
    }
    payload_cmd->logger_functions = fns;
    return 0;
}
