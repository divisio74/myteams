/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_2.h
*/

#ifndef OPERATIONS_2_H
    #define OPERATIONS_2_H

    #include "struct.h"

int cmd_help(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_login(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_logout(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_users(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_user(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_send(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_messages(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_subscribe(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_subscribed(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_unsubscribe(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_use(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_create(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_list(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int cmd_info(servor_t *servor, client_state_t *client_state, char *args,
    int client_fd);
int send_simple_response(int client_fd, int code);

#endif /* OPERATIONS_2_H */
