/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** handle_response.c
*/

#include "client.h"
#include "logging_client.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int parse_fields(char *s, char **out, int max)
{
    int n = 0;
    char *p = s;

    while (*p == ' ')
        p++;
    while (n < max && *p) {
        out[n++] = p;
        if (n == max)
            break;
        while (*p && *p != ' ')
            p++;
        if (*p) {
            *p++ = '\0';
            while (*p == ' ')
                p++;
        }
    }
    return n;
}

static void handle_use_ok(client_t *client)
{
    pending_t *p = &client->pending;

    free(client->ctx_team);
    free(client->ctx_channel);
    free(client->ctx_thread);
    client->ctx_team = p->pending_team ? strdup(p->pending_team) : NULL;
    client->ctx_channel = p->pending_channel
        ? strdup(p->pending_channel) : NULL;
    client->ctx_thread = p->pending_thread
        ? strdup(p->pending_thread) : NULL;
    client->ctx_level = (client->ctx_team ? 1 : 0)
        + (client->ctx_channel ? 1 : 0)
        + (client->ctx_thread ? 1 : 0);
}

static void handle_login_ok(client_t *client, char *data)
{
    char *f[1] = {NULL};

    if (parse_fields(data, f, 1) < 1)
        return;
    free(client->user_uid);
    free(client->user_name);
    client->user_uid = strdup(f[0]);
    client->user_name = client->pending.arg1
        ? strdup(client->pending.arg1) : NULL;
    client_event_logged_in(f[0], client->user_name ? client->user_name : "");
}

static void handle_logout_ok(client_t *client, char *data)
{
    char *f[1] = {NULL};
    const char *uid = NULL;
    const char *name = NULL;

    parse_fields(data, f, 1);
    uid = f[0] ? f[0] : (client->user_uid ? client->user_uid : "");
    name = client->user_name ? client->user_name : "";
    client_event_logged_out(uid, name);
    free(client->user_uid);
    free(client->user_name);
    client->user_uid = NULL;
    client->user_name = NULL;
}

static void handle_user_ok(char *data)
{
    char *f[2] = {NULL, NULL};

    if (parse_fields(data, f, 2) < 2)
        return;
    client_print_user(f[1], f[0], 0);
}

static void handle_create_ok(client_t *client, char *data)
{
    char *f[5] = {NULL, NULL, NULL, NULL, NULL};
    int n = parse_fields(data, f, 5);
    time_t ts = 0;

    if (n < 1)
        return;
    if (client->pending.ctx_level == 3) {
        if (n >= 3)
            ts = (time_t)atol(f[2]);
        client_print_reply_created(f[0], f[1] ? f[1] : "", ts,
            f[3] ? f[3] : "");
    } else if (client->pending.ctx_level == 2) {
        if (n >= 3)
            ts = (time_t)atol(f[2]);
        client_print_thread_created(f[0], f[1] ? f[1] : "", ts,
            f[3] ? f[3] : "", f[4] ? f[4] : "");
    } else if (client->pending.ctx_level == 1) {
        client_print_channel_created(f[0], f[1] ? f[1] : "",
            f[2] ? f[2] : "");
    } else {
        client_print_team_created(f[0], f[1] ? f[1] : "",
            f[2] ? f[2] : "");
    }
}

static void handle_info_ok(client_t *client, char *data)
{
    char *f[5] = {NULL, NULL, NULL, NULL, NULL};
    int n = parse_fields(data, f, 5);
    time_t ts = 0;

    if (n < 1)
        return;
    if (client->pending.ctx_level == 3) {
        if (n >= 3)
            ts = (time_t)atol(f[2]);
        client_print_thread(f[0], f[1] ? f[1] : "", ts,
            f[3] ? f[3] : "", f[4] ? f[4] : "");
    } else if (client->pending.ctx_level == 2) {
        client_print_channel(f[0], f[1] ? f[1] : "", f[2] ? f[2] : "");
    } else if (client->pending.ctx_level == 1) {
        client_print_team(f[0], f[1] ? f[1] : "", f[2] ? f[2] : "");
    }
}

static void handle_200(client_t *client, char *data)
{
    pending_t *p = &client->pending;

    switch (p->kind) {
    case LOGIN:
        handle_login_ok(client, data);
        break;
    case LOGOUT:
        handle_logout_ok(client, data);
        break;
    case USER:
        handle_user_ok(data);
        break;
    case CREATE:
        handle_create_ok(client, data);
        break;
    case INFO:
        handle_info_ok(client, data);
        break;
    case SUBSCRIBE:
        client_print_subscribed(client->user_uid ? client->user_uid : "",
            p->arg1 ? p->arg1 : "");
        break;
    case UNSUBSCRIBE:
        client_print_unsubscribed(client->user_uid ? client->user_uid : "",
            p->arg1 ? p->arg1 : "");
        break;
    case USE:
        handle_use_ok(client);
        break;
    default:
        break;
    }
    pending_reset(p);
}

static void handle_list_item(client_t *client, char *trimmed)
{
    pending_t *p = &client->pending;
    char *f[5] = {NULL, NULL, NULL, NULL, NULL};
    int n = parse_fields(trimmed, f, 5);
    time_t ts = 0;

    if (n < 1)
        return;
    if (p->kind == USERS) {
        client_print_users(f[1] ? f[1] : f[0], f[0], 0);
        return;
    }
    if (p->kind == MESSAGES) {
        client_private_message_print_messages(f[0], 0, f[1] ? f[1] : "");
        return;
    }
    if (p->kind == SUBSCRIBED_ALL) {
        client_print_teams(f[0], f[1] ? f[1] : "", "");
        return;
    }
    if (p->kind == SUBSCRIBED_TEAM) {
        client_print_users(f[0], "", 0);
        return;
    }
    if (p->kind != LIST)
        return;
    if (p->ctx_level == 0) {
        client_print_teams(f[0], f[1] ? f[1] : "", f[2] ? f[2] : "");
    } else if (p->ctx_level == 1) {
        client_team_print_channels(f[0], f[1] ? f[1] : "",
            f[2] ? f[2] : "");
    } else if (p->ctx_level == 2) {
        if (n >= 3)
            ts = (time_t)atol(f[2]);
        client_channel_print_threads(f[0], f[1] ? f[1] : "", ts,
            f[3] ? f[3] : "", f[4] ? f[4] : "");
    } else if (p->ctx_level == 3) {
        if (n >= 3)
            ts = (time_t)atol(f[2]);
        client_thread_print_replies(f[0], f[1] ? f[1] : "", ts,
            f[3] ? f[3] : "");
    }
}

static void handle_error(client_t *client, char *line)
{
    pending_t *p = &client->pending;
    int code = atoi(line);

    if (code == 300) {
        client_error_unauthorized();
    } else if (code == 400) {
        if (p->kind == USER || p->kind == SEND
                || p->kind == MESSAGES)
            client_error_unknown_user(p->arg1 ? p->arg1 : "");
        else if (p->kind == SUBSCRIBE || p->kind == SUBSCRIBED_TEAM
                || p->kind == UNSUBSCRIBE)
            client_error_unknown_team(p->arg1 ? p->arg1 : "");
        else if (p->kind == USE)
            client_error_unknown_team(
                p->pending_team ? p->pending_team : "");
        else if (p->kind == CREATE || p->kind == LIST
                || p->kind == INFO) {
            if (p->ctx_level == 1)
                client_error_unknown_team(
                    client->ctx_team ? client->ctx_team : "");
            else if (p->ctx_level == 2)
                client_error_unknown_channel(
                    client->ctx_channel ? client->ctx_channel : "");
            else if (p->ctx_level == 3)
                client_error_unknown_thread(
                    client->ctx_thread ? client->ctx_thread : "");
        }
    } else if (code == 409) {
        client_error_already_exist();
    }
    pending_reset(p);
}

void handle_line(client_t *client, char *line)
{
    char *data = NULL;

    if (strncmp(line, "220", 3) == 0)
        return;
    if (client->in_list) {
        if (line[0] == ']') {
            client->in_list = false;
            pending_reset(&client->pending);
            return;
        }
        handle_list_item(client,
            strncmp(line, "  ", 2) == 0 ? line + 2 : line);
        return;
    }
    if (strncmp(line, "200 [", 5) == 0) {
        client->in_list = true;
        return;
    }
    if (strncmp(line, "200", 3) == 0) {
        data = line[3] == ' ' ? line + 4 : line + 3;
        handle_200(client, data);
        return;
    }
    handle_error(client, line);
}
