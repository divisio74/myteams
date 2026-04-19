/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** pending.c
*/

#include "client.h"
#include <stdlib.h>
#include <string.h>

static const struct {
    const char *name;
    client_cmd_t kind;
} CMD_TABLE[] = {
    {"/login", LOGIN},
    {"/logout", LOGOUT},
    {"/users", USERS},
    {"/user", USER},
    {"/send", SEND},
    {"/messages", MESSAGES},
    {"/subscribe", SUBSCRIBE},
    {"/unsubscribe", UNSUBSCRIBE},
    {"/use", USE},
    {"/create", CREATE},
    {"/list", LIST},
    {"/info", INFO},
    {NULL, NONE},
};

void pending_reset(pending_t *p)
{
    if (!p)
        return;
    free(p->arg1);
    free(p->arg2);
    free(p->pending_team);
    free(p->pending_channel);
    free(p->pending_thread);
    memset(p, 0, sizeof(*p));
}

static client_cmd_t lookup_cmd(const char *cmd, int has_arg)
{
    if (!cmd)
        return NONE;
    if (strcmp(cmd, "/subscribed") == 0)
        return has_arg ? SUBSCRIBED_TEAM : SUBSCRIBED_ALL;
    for (int i = 0; CMD_TABLE[i].name; i++) {
        if (strcmp(CMD_TABLE[i].name, cmd) == 0)
            return CMD_TABLE[i].kind;
    }
    return NONE;
}

static void assign_use(pending_t *p, char *args[3])
{
    p->pending_team = args[0];
    p->pending_channel = args[1];
    p->pending_thread = args[2];
    args[0] = NULL;
    args[1] = NULL;
    args[2] = NULL;
}

static void assign_pair(pending_t *p, char *args[3])
{
    p->arg1 = args[0];
    p->arg2 = args[1];
    args[0] = NULL;
    args[1] = NULL;
}

static void fill_pending_args(pending_t *p, char *args[3])
{
    switch (p->kind) {
    case USE:
        assign_use(p, args);
        break;
    case SEND:
    case CREATE:
        assign_pair(p, args);
        break;
    case LOGIN:
    case USER:
    case MESSAGES:
    case SUBSCRIBE:
    case SUBSCRIBED_TEAM:
    case UNSUBSCRIBE:
        p->arg1 = args[0];
        args[0] = NULL;
        break;
    default:
        break;
    }
}

void pending_update_from_input(client_t *c, const char *line)
{
    const char *cursor = line;
    char *cmd = NULL;
    char *args[3] = {NULL, NULL, NULL};
    int argc = 0;

    pending_reset(&c->pending);
    if (parse_arg(&cursor, &cmd) == -1)
        return;
    while (argc < 3 && parse_arg(&cursor, &args[argc]) == 0)
        argc++;
    c->pending.kind = lookup_cmd(cmd, argc > 0);
    c->pending.ctx_level = c->ctx_level;
    fill_pending_args(&c->pending, args);
    free(cmd);
    for (int i = 0; i < 3; i++)
        free(args[i]);
}
