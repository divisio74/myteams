/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** response.c
*/

#include "client.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_line(client_t *client, char *line)
{
    (void)client;
    fprintf(stdout, "%s\n", line);
}

static void process_buffer(client_t *client)
{
    char *newline = NULL;
    char *start = client->sock_buffer;

    while ((newline = strstr(start, "\r\n"))) {
        *newline = '\0';
        handle_line(client, start);
        start = newline + 2;
    }
    if (start != client->sock_buffer) {
        client->sock_buffer_len = strlen(start);
        memmove(client->sock_buffer, start, client->sock_buffer_len + 1);
    }
}

int handle_socket_read(client_t *client)
{
    ssize_t n = 0;
    size_t dispo = CLIENT_BUFFER_SIZE - client->sock_buffer_len - 1;

    if (dispo == 0) {
        fprintf(stdout, "Error: socket buffer overflow\n");
        return -1;
    }
    n = read(client->socket_fd,
        client->sock_buffer + client->sock_buffer_len, dispo);
    if (n == -1) {
        fprintf(stdout, "Error: read failed: %s\n", strerror(errno));
        return -1;
    }
    if (n == 0) {
        client->running = false;
        return 0;
    }
    client->sock_buffer_len += (size_t)n;
    client->sock_buffer[client->sock_buffer_len] = '\0';
    process_buffer(client);
    return 0;
}

static void process_stdin_line(client_t *client, const char *raw)
{
    char *line = strdup(raw);
    char *trimmed = NULL;

    if (!line)
        return;
    trimmed = clean_spaces(line);
    if (*trimmed == '\0') {
        free(line);
        return;
    }
    if (strcmp(trimmed, "/help") == 0)
        print_help();
    else
        client_send_line(client->socket_fd, trimmed);
    free(line);
}

static void process_stdin_buffer(client_t *client)
{
    char *newline = NULL;
    char *start = client->stdin_buffer;

    while ((newline = strchr(start, '\n'))) {
        *newline = '\0';
        process_stdin_line(client, start);
        start = newline + 1;
    }
    if (start != client->stdin_buffer) {
        client->stdin_buffer_len = strlen(start);
        memmove(client->stdin_buffer, start,
            client->stdin_buffer_len + 1);
    }
}

int handle_stdin_read(client_t *client)
{
    ssize_t n = 0;
    size_t dispo = CLIENT_BUFFER_SIZE - client->stdin_buffer_len - 1;

    if (dispo == 0) {
        fprintf(stdout, "Error: stdin buffer overflow\n");
        return -1;
    }
    n = read(STDIN_FILENO,
        client->stdin_buffer + client->stdin_buffer_len, dispo);
    if (n == -1) {
        fprintf(stdout, "Error: stdin read failed: %s\n", strerror(errno));
        return -1;
    }
    if (n == 0) {
        client->running = false;
        return 0;
    }
    client->stdin_buffer_len += (size_t)n;
    client->stdin_buffer[client->stdin_buffer_len] = '\0';
    process_stdin_buffer(client);
    return 0;
}
