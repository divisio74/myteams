/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** infrastructure.h
*/

#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H

#include "job_logic.h"
#include <dlfcn.h>
#include <poll.h>
#include <stdbool.h>

#define MAX_SOCKET 1000
#define FUNCTION_GET_LIB "get_lib"

typedef struct client_s {
  char *output_buffer;
  char *user_uid;
  bool in_use;
  int client_fd;
  char *data_ip;
} client_t;

typedef struct logger_functions_s logger_functions_t;
typedef struct payload_cmd_s payload_cmd_t;

typedef int (*command_handler_t)(payload_cmd_t *);

typedef struct command_s {
  char *command_name;
  command_handler_t command;
  char *description;
} command_t;

typedef struct response_code_s {
  int code;
  char *message;
} response_code_t;

typedef struct logger_functions_s {
  int (*server_event_team_created)(
      char const *team_uuid,
      char const *team_name,
      char const *user_uuid);
  int (*server_event_channel_created)(
      char const *team_uuid,
      char const *channel_uuid,
      char const *channel_name);
  int (*server_event_thread_created)(
      char const *channel_uuid,
      char const *thread_uuid,
      char const *user_uuid,
      char const *thread_title,
      char const *thread_body);
  int (*server_event_reply_created)(
      char const *thread_uuid,
      char const *user_uuid,
      char const *reply_body);
  int (*server_event_user_subscribed)(
      char const *team_uuid,
      char const *user_uuid);
  int (*server_event_user_unsubscribed)(
      char const *team_uuid,
      char const *user_uuid);
  int (*server_event_user_created)(
      char const *user_uuid,
      char const *user_name);
  int (*server_event_user_loaded)(
      char const *user_uuid,
      char const *user_name);
  int (*server_event_user_logged_in)(char const *user_uuid);
  int (*server_event_user_logged_out)(char const *user_uuid);
  int (*server_event_private_message_sended)(
      char const *sender_uuid,
      char const *receiver_uuid,
      char const *message_body);
} logger_functions_t;

typedef struct payload_cmd_s {
  teams_t *teams;
  client_t *client;
  char *args;
  char *current_command;
  char *parameter;
  void *logger;
  logger_functions_t *logger_functions;
} payload_cmd_t;

typedef struct server_s {
  client_t *client_fds;
  struct pollfd *poll_fds;
  char buffer[1024];
  payload_cmd_t payload_cmd;
  void *logger;
  teams_t teams;
} server_t;

int cmd_help(payload_cmd_t *payload_cmd);
int cmd_login(payload_cmd_t *payload_cmd);
int cmd_logout(payload_cmd_t *payload_cmd);
int cmd_users(payload_cmd_t *payload_cmd);
int cmd_user(payload_cmd_t *payload_cmd);
int cmd_send(payload_cmd_t *payload_cmd);
int cmd_messages(payload_cmd_t *payload_cmd);
int cmd_subscribe(payload_cmd_t *payload_cmd);
int cmd_subscribed(payload_cmd_t *payload_cmd);
int cmd_unsubscribe(payload_cmd_t *payload_cmd);
int cmd_use(payload_cmd_t *payload_cmd);
int cmd_create(payload_cmd_t *payload_cmd);
int cmd_list(payload_cmd_t *payload_cmd);
int cmd_info(payload_cmd_t *payload_cmd);

extern const command_t command_functions[14];
extern const response_code_t response_dispatch_table[];

int call_command(server_t *server);
char *call_help_command(char *command);
char *call_server_response(int code);
int send_simple_response(int client_fd, int code);
int send_login_succes(payload_cmd_t *payload_cmd, user_t *user);
int send_lougout_sucess(payload_cmd_t *payload_cmd);

void split_with_space(char *input, char **operant, char **parameters);
void bufferise_client_commands(server_t *server);

int accept_new_client(int fd, client_t *clientFds, struct pollfd *poll_fds);
int handle_client_events(server_t *server);
int has_pending_connection(int fd, int timeout_ms);

void *load(char *path);
void *getfunction(void **handle);
void *getfunction_by_name(void **handle, const char *function_name);
void unload(void *handle);

int server_init(server_t *server, char *path);
void server_free(server_t *server);
int init_logger_functions(payload_cmd_t *payload_cmd);

int socket_bind(int fd, int port);
int socket_initiation(int port);

int parse_server_port(char *port, int *port_buffer);
void split_with_separator(char *input, char **operant,
    char **parameters, char separator);
int str_to_word_array(char *input, char *separator, char ***tokens);
void free_str_to_word_array(char **tokens);

int list_transfer(char *args, int data_client_fd);
int retr_transfer(client_t *client, char *args);

#endif /* INFRASTRUCTURE_H */
