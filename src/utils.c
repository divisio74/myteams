/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** utils.c
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int parse_server_port(char *port, int *port_buffer)
{
    char *end = NULL;
    long parsed_port;

    errno = 0;
    parsed_port = strtol(port, &end, 10);
    if (errno != 0 || *end != '\0' || parsed_port < 0 || parsed_port > 65535) {
        fprintf(stderr, "Erreur: Port invalide '%s'.\n", port);
        return -1;
    }
    *port_buffer = (int)parsed_port;
    return 0;
}

void split_with_separator(char *input, char **operant,
    char **parameters, char separator)
{
    char *cpy = malloc((strlen(input) + 1) * sizeof(char));
    char *sep;

    strcpy(cpy, input);
    sep = strchr(cpy, separator);
    if (!sep) {
        *operant = cpy;
        return;
    }
    *sep = '\0';
    *operant = cpy;
    *parameters = sep + 1;
}

void free_str_to_word_array(char **tokens)
{
    if (!tokens)
        return;
    for (int i = 0; tokens[i]; i++)
        free(tokens[i]);
    free(tokens);
}

static int count_word(const char *input, char *separator)
{
    char *copy = NULL;
    char *token = NULL;
    int count = 0;

    copy = strdup(input);
    if (!copy)
        return -1;
    token = strtok(copy, separator);
    while (token) {
        count++;
        token = strtok(NULL, separator);
    }
    free(copy);
    return count;
}

static char **allocate_words(int count)
{
    char **tokens = malloc(sizeof(char *) * (count + 1));

    if (!tokens)
        return NULL;
    tokens[count] = NULL;
    return tokens;
}

static int fill_words(const char *input, char *separator, char **tokens)
{
    char *copy = NULL;
    char *token = NULL;

    copy = strdup(input);
    if (!copy)
        return -1;
    token = strtok(copy, separator);
    for (int i = 0; token; i++) {
        tokens[i] = strdup(token);
        if (!tokens[i]) {
            free(copy);
            tokens[i] = NULL;
            free_str_to_word_array(tokens);
            return -1;
        }
        token = strtok(NULL, separator);
    }
    free(copy);
    return 0;
}

int str_to_word_array(char *input, char *separator, char ***tokens)
{
    int count = 0;

    if (!input || input[0] == '\0' || !separator || separator[0] == '\0')
        return 0;
    count = count_word(input, separator);
    if (count == -1)
        return -1;
    *tokens = allocate_words(count);
    if (!*tokens)
        return -1;
    if (fill_words(input, separator, *tokens) == -1) {
        *tokens = NULL;
        return -1;
    }
    return count;
}
