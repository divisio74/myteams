/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** struct.h
*/

#ifndef STRUCT_H
    #define STRUCT_H

    #include <poll.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include "mylist.h"
    #include "utils.h"

    #define MAX_SOCKET 1000

typedef enum data_mode_e {
    DATA_MODE_NONE,
    DATA_MODE_PASSIVE,
    DATA_MODE_ACTIVE
} data_mode_t;

typedef struct client_state_s {
    char *cwd;
    char *output_buffer;
    char *user_uid;
    bool user_logged_in;
    bool login_in;
    bool in_use;
    int data_fd;
    char *data_ip;
    int data_port;
    data_mode_t data_mode;
} client_state_t;

typedef struct servor_s servor_t;

typedef int (*operation_handler_t)(servor_t *, client_state_t *, char *, int);

typedef struct operation_s {
    char *command_name;
    operation_handler_t operation;
    char *description;
} operation_t;

typedef struct response_code_s {
    int code;
    char *message;
} response_code_t;

typedef struct user_s {
    char *name;
    char *uid;
} user_t;

typedef struct servor_s {
    linked_list_t *users;
} servor_t;

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

extern const operation_t operation_functions[14];
extern const response_code_t response_dispatch_table[];

int call_operation(servor_t *servor, client_state_t *client_state,
    char *operation, char *args, int client_fd);
char *call_help_function(char *operation);
char *call_server_response(int code);
int send_simple_response(int client_fd, int code);

#endif /* STRUCT_H */
