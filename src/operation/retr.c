/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** retr.c
*/

#include "retr.h"
#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

static int read_from_fd(int fd, char **buffer, ssize_t *bytes_read)
{
    *bytes_read = read(fd, *buffer, 999997);
    if (*bytes_read != -1)
        return 0;
    close(fd);
    free(*buffer);
    *buffer = NULL;
    return -1;
}

static int read_file(char **buffer, char *path)
{
    int fd = 0;
    ssize_t bytes_read = 0;

    if (!buffer || *buffer != NULL || !path)
        return -1;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -1;
    *buffer = malloc(sizeof(char) * 1000000);
    if (*buffer == NULL) {
        close(fd);
        return -1;
    }
    if (read_from_fd(fd, buffer, &bytes_read) == -1)
        return -1;
    (*buffer)[bytes_read] = '\r';
    (*buffer)[bytes_read + 1] = '\n';
    (*buffer)[bytes_read + 2] = '\0';
    close(fd);
    return 0;
}

static int write_file_to_data_fd(client_state_t *client_state,
    char *file_buffer)
{
    ssize_t written = 0;
    size_t total_len = strlen(file_buffer);

    written = write(client_state->data_fd, file_buffer, total_len);
    if (written == -1)
        return -1;
    return 0;
}

static int transfert_file(client_state_t *client_state, char *path)
{
    char *file_buffer = NULL;
    int status = 0;

    status = read_file(&file_buffer, path);
    if (status == -1)
        return -1;
    status = write_file_to_data_fd(client_state, file_buffer);
    free(file_buffer);
    return status;
}

int retr_transfer(client_state_t *client_state, char *args)
{
    char **tokens = NULL;
    int token_count = 0;

    token_count = str_to_word_array(args, " ", &tokens);
    if (token_count <= 0)
        return -1;
    for (int i = 0; i < token_count; i++) {
        if (transfert_file(client_state, tokens[i]) == -1) {
            free_str_to_word_array(tokens);
            return -1;
        }
    }
    free_str_to_word_array(tokens);
    return 0;
}
