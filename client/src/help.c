/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** help.c
*/

#include "client.h"
#include <stdio.h>

static const char HELP_TEXT[] =
    "Available commands:\n"
    "  /help                                     show this help\n"
    "  /login \"user_name\"                        login with a user name\n"
    "  /logout                                   logout from the server\n"
    "  /users                                    list all users\n"
    "  /user \"user_uuid\"                         get user details\n"
    "  /send \"user_uuid\" \"message_body\"          send a private message\n"
    "  /messages \"user_uuid\"                     list private messages\n"
    "  /subscribe \"team_uuid\"                    subscribe to a team\n"
    "  /subscribed [\"team_uuid\"]                 list subs or team members\n"
    "  /unsubscribe \"team_uuid\"                  unsubscribe from a team\n"
    "  /use [\"team\"] [\"channel\"] [\"thread\"]      set context\n"
    "  /create ...                               create sub-resource\n"
    "  /list                                     list sub-resources\n"
    "  /info                                     show current resource\n";

void print_help(void)
{
    fprintf(stdout, "%s", HELP_TEXT);
}
