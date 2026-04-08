/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** bufferiser.c
*/

#include "infrastructure.h"
#include <stdlib.h>
#include <string.h>

// operant and parameters should be equal to NULL
void split_with_space(char *input, char **operant, char **parameters)
{
    split_with_separator(input, operant, parameters, ' ');
}

static void command_execution(server_t *server, char *current_command)
{
    payload_cmd_t *payload_cmd = &server->payload_cmd;
    char *operant = NULL;
    char *parameters = NULL;

    split_with_space(current_command, &operant, &parameters);
    payload_cmd->current_command = operant;
    payload_cmd->parameter = parameters;
    call_command(server);
    payload_cmd->current_command = NULL;
    payload_cmd->parameter = NULL;
}

static void algo_loop(server_t *server, char *current_command)
{
    payload_cmd_t *payload_cmd = &server->payload_cmd;
    char *args = payload_cmd->args;
    char current_char[2];

    for (size_t i = 0; args[i] != '\0'; i++) {
        if (args[i] == '\r' && args[i + 1] != '\0' && args[i + 1] == '\n') {
            command_execution(server, current_command);
            current_command[0] = '\0';
            i += 1;
            continue;
        }
        current_char[0] = args[i];
        current_char[1] = '\0';
        if (current_command && strlen(current_command) + 1 < 1024)
            strcat(current_command, current_char);
    }
}

void bufferise_client_commands(server_t *server)
{
    char *current_command = NULL;
    payload_cmd_t *payload_cmd = &server->payload_cmd;
    client_t *client = payload_cmd->client;
    char *args = payload_cmd->args;

    if (!server || !client || !args)
        return;
    current_command = malloc(sizeof(char) * 1024);
    if (!current_command)
        return;
    current_command[0] = '\0';
    if (client->output_buffer && client->output_buffer[0] != '\0')
        strcat(current_command, client->output_buffer);
    algo_loop(server, current_command);
    free(client->output_buffer);
    client->output_buffer = NULL;
    if (current_command[0] != '\0')
        client->output_buffer = strdup(current_command);
    free(current_command);
}
