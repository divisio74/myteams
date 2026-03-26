/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** data_connection.h
*/

#ifndef DATA_CONNECTION_H
    #define DATA_CONNECTION_H

    #include "struct.h"

int open_data_connection(client_state_t *client_state, int client_fd);
int close_data_connection(client_state_t *client_state);
int active_mode(client_state_t *client_state);

#endif /* DATA_CONNECTION_H */
