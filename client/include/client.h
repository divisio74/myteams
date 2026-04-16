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

typedef struct client_s {
    int socket_fd;
    bool running;
    char sock_buffer[CLIENT_BUFFER_SIZE];
    size_t sock_buffer_len;
    char stdin_buffer[CLIENT_BUFFER_SIZE];
    size_t stdin_buffer_len;
} client_t;

int run_client(const char *ip, int port);

int connect_to_server(const char *ip, int port);
int client_send_line(int fd, const char *line);

int handle_socket_read(client_t *client);
int handle_stdin_read(client_t *client);
void handle_line(client_t *client, char *line);

void print_help(void);

char *clean_spaces(char *s);

#endif
