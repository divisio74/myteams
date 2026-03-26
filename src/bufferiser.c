/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** bufferiser.c
*/

#include "bufferiser.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

// operant and parameters should be equal to NULL
void split_with_space(char *input, char **operant, char **parameters)
{
    split_with_separator(input, operant, parameters, ' ');
}

static void operation_execution(servor_t *servor, client_state_t *client,
    char *current_operation, int client_fd)
{
    char *operant = NULL;
    char *parameters = NULL;

    split_with_space(current_operation, &operant, &parameters);
    call_operation(servor, client, operant, parameters, client_fd);
}

static void algo_loop(servor_t *servor, client_state_t *client, char *args,
    char *current_operation, int client_fd)
{
    char current_char[2];

    for (size_t i = 0; args[i] != '\0'; i++) {
        if (args[i] == '\r' && args[i + 1] != '\0' && args[i + 1] == '\n') {
            operation_execution(servor, client, current_operation, client_fd);
            current_operation[0] = '\0';
            i += 1;
            continue;
        }
        current_char[0] = args[i];
        current_char[1] = '\0';
        if (current_operation && strlen(current_operation) + 1 < 1024)
            strcat(current_operation, current_char);
    }
}

void bufferise_client_operations(servor_t *servor, client_state_t *client,
    char *args, int client_fd)
{
    char *current_operation = NULL;

    if (client == NULL || args == NULL)
        return;
    current_operation = malloc(sizeof(char) * 1024);
    if (current_operation == NULL)
        return;
    current_operation[0] = '\0';
    if (client->output_buffer != NULL && client->output_buffer[0] != '\0')
        strcat(current_operation, client->output_buffer);
    algo_loop(servor, client, args, current_operation, client_fd);
    if (current_operation[0] != '\0') {
        free(client->output_buffer);
        client->output_buffer = strdup(current_operation);
    }
    free(current_operation);
}
