/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** struct.h
*/

#ifndef STRUCT_H
#define STRUCT_H

#include "mylist.h"
#include "utils.h"
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_SOCKET 1000

typedef struct client_s {
  char *output_buffer;
  char *user_uid;
  bool in_use;
  int client_fd;
  char *data_ip;
} client_t;

typedef struct teams_s teams_t;

typedef int (*operation_handler_t)(teams_t *, client_t *, char *);

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

typedef struct teams_s {
  linked_list_t *users;
} teams_t;

typedef struct server_s {
  client_t *client_fds;
  struct pollfd *poll_fds;
  void *handle;
  teams_t teams;
} server_t;

int cmd_help(teams_t *teams, client_t *client, char *args);
int cmd_login(teams_t *teams, client_t *client, char *args);
int cmd_logout(teams_t *teams, client_t *client, char *args);
int cmd_users(teams_t *teams, client_t *client, char *args);
int cmd_user(teams_t *teams, client_t *client, char *args);
int cmd_send(teams_t *teams, client_t *client, char *args);
int cmd_messages(teams_t *teams, client_t *client, char *args);
int cmd_subscribe(teams_t *teams, client_t *client, char *args);
int cmd_subscribed(teams_t *teams, client_t *client, char *args);
int cmd_unsubscribe(teams_t *teams, client_t *client,
                    char *args);
int cmd_use(teams_t *teams, client_t *client, char *args);
int cmd_create(teams_t *teams, client_t *client, char *args);
int cmd_list(teams_t *teams, client_t *client, char *args);
int cmd_info(teams_t *teams, client_t *client, char *args);

extern const operation_t operation_functions[14];
extern const response_code_t response_dispatch_table[];

int call_operation(teams_t *teams, client_t *client,
                   char *operation, char *args);
char *call_help_function(char *operation);
char *call_server_response(int code);
int send_simple_response(int client_fd, int code);

#endif /* STRUCT_H */
