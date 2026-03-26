/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** data_connection.c
*/

#include "data_connection.h"
#include "operations_2.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int open_data_connection(client_state_t *client_state, int client_fd)
{
    if (client_state->data_mode == DATA_MODE_NONE) {
        fprintf(stderr, "LIST error: no data mode set\n");
        send_simple_response(client_fd, 425);
        return -1;
    }
    if (client_state->data_mode == DATA_MODE_PASSIVE) {
        if (client_state->data_fd == 0) {
            fprintf(stderr, "LIST error: no data connection available\n");
            send_simple_response(client_fd, 425);
            return -1;
        }
    }
    if (client_state->data_mode == DATA_MODE_ACTIVE) {
        if (active_mode(client_state) == -1) {
            fprintf(stderr, "LIST error: active mode not implemented\n");
            send_simple_response(client_fd, 425);
            return -1;
        }
    }
    return send_simple_response(client_fd, 150);
}

int close_data_connection(client_state_t *client_state)
{
    if (close(client_state->data_fd) == -1) {
        fprintf(stderr, "LIST error: close data socket failed: %s\n",
            strerror(errno));
        return -1;
    }
    if (client_state->data_mode == DATA_MODE_ACTIVE) {
        free(client_state->data_ip);
        client_state->data_ip = NULL;
        client_state->data_port = 0;
    }
    client_state->data_fd = 0;
    client_state->data_mode = DATA_MODE_NONE;
    return 0;
}
