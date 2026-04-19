/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** client.h
*/

#ifndef MYTEAMS_CLIENT_H
    #define MYTEAMS_CLIENT_H

    #include <stdbool.h>
    #include <stddef.h>

    #define CLIENT_BUFFER_SIZE 4096

typedef enum {
    NONE = 0,
    LOGIN,
    LOGOUT,
    USERS,
    USER,
    SEND,
    MESSAGES,
    SUBSCRIBE,
    SUBSCRIBED_ALL,
    SUBSCRIBED_TEAM,
    UNSUBSCRIBE,
    USE,
    CREATE,
    LIST,
    INFO,
} client_cmd_t;

typedef struct pending_s {
    client_cmd_t kind;
    int ctx_level;
    char *arg1;
    char *arg2;
    char *pending_team;
    char *pending_channel;
    char *pending_thread;
} pending_t;

typedef struct client_s {
    int socket_fd;
    bool running;
    char sock_buffer[CLIENT_BUFFER_SIZE];
    size_t sock_buffer_len;
    char stdin_buffer[CLIENT_BUFFER_SIZE];
    size_t stdin_buffer_len;
    pending_t pending;
    int ctx_level;
    char *ctx_team;
    char *ctx_channel;
    char *ctx_thread;
    char *user_uid;
    char *user_name;
    bool in_list;
} client_t;

int run_client(const char *ip, int port);

int connect_to_server(const char *ip, int port);
int client_send_line(int fd, const char *line);

int handle_socket_read(client_t *client);
int handle_stdin_read(client_t *client);
void handle_line(client_t *client, char *line);

void print_help(void);
char *clean_spaces(char *s);

int parse_arg(const char **cursor, char **out);
int split_response(char *line, char **tokens, int max);
void pending_reset(pending_t *p);
void pending_update_from_input(client_t *c, const char *line);

#endif
