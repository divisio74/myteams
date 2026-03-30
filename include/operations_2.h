/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_2.h
*/

#ifndef OPERATIONS_2_H
#define OPERATIONS_2_H

#include "struct.h"

int cmd_help(teams_t *servor, client_t *client, char *args);
int cmd_login(teams_t *servor, client_t *client, char *args);
int cmd_logout(teams_t *servor, client_t *client, char *args);
int cmd_users(teams_t *servor, client_t *client, char *args);
int cmd_user(teams_t *servor, client_t *client, char *args);
int cmd_send(teams_t *servor, client_t *client, char *args);
int cmd_messages(teams_t *servor, client_t *client, char *args);
int cmd_subscribe(teams_t *servor, client_t *client, char *args);
int cmd_subscribed(teams_t *servor, client_t *client, char *args);
int cmd_unsubscribe(teams_t *servor, client_t *client,
                    char *args);
int cmd_use(teams_t *servor, client_t *client, char *args);
int cmd_create(teams_t *servor, client_t *client, char *args);
int cmd_list(teams_t *servor, client_t *client, char *args);
int cmd_info(teams_t *servor, client_t *client, char *args);
int send_simple_response(int client_fd, int code);

#endif /* OPERATIONS_2_H */
