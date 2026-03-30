/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** operations.c
*/

#include "struct.h"
#include "operations_2.h"
#include <string.h>
#include <unistd.h>

const operation_t operation_functions[14] = {
    {"/help", cmd_help, "/help: show help.\n"},
    {"/login", cmd_login, "/login [\"user_name\"]: set the user_name.\n"},
    {"/logout", cmd_logout, "/logout: disconnect client from server.\n"},
    {"/users", cmd_users, "/users: list all users in the domain.\n"},
    {"/user", cmd_user, "/user [\"user_uuid\"]: get user details.\n"},
    {"/send", cmd_send,
        "/send [\"user_uuid\"] [\"message_body\"]: send a message.\n"},
    {"/messages", cmd_messages,
        "/messages [\"user_uuid\"]: list exchanged messages.\n"},
    {"/subscribe", cmd_subscribe,
        "/subscribe [\"team_uuid\"]: subscribe to team events.\n"},
    {"/subscribed", cmd_subscribed,
        "/subscribed ?[\"team_uuid\"]: list subscriptions.\n"},
    {"/unsubscribe", cmd_unsubscribe,
        "/unsubscribe [\"team_uuid\"]: unsubscribe from a team.\n"},
    {"/use", cmd_use,
        "/use ?[\"team_uuid\"] ?[\"channel_uuid\"] ?[\"thread_uuid\"]: "
        "set context.\n"},
    {"/create", cmd_create, "/create: create a sub resource by context.\n"},
    {"/list", cmd_list, "/list: list sub resources by context.\n"},
    {"/info", cmd_info, "/info: display current resource details.\n"}
};

int send_simple_response(int client_fd, int code)
{
    char *response = call_server_response(code);

    if (write(client_fd, response, strlen(response)) == -1)
        return -1;
    return 0;
}

int call_operation(teams_t *teams, client_t *client,
    char *operation, char *args)
{
    if (!operation)
        return send_simple_response(client->client_fd, 500);
    for (int i = 0; i < 14; i++) {
        if (strcmp(operation_functions[i].command_name, operation) == 0)
            return operation_functions[i].operation(teams, client,
                args);
    }
    return send_simple_response(client->client_fd, 500);
}

char *call_help_function(char *operation)
{
    for (int i = 0; i < 14; i++) {
        if (!operation)
            continue;
        if (strcmp(operation_functions[i].command_name, operation) == 0)
            return operation_functions[i].description;
    }
    return NULL;
}
