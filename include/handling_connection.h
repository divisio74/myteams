/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** handling_connection.h
*/

#ifndef HANDLING_CONNECTION_H
    #define HANDLING_CONNECTION_H

    #include <poll.h>
    #include "struct.h"

int accept_new_client(int fd, client_state_t *clientFds,
    struct pollfd *poll_fds);
int handle_client_events(servor_t *servor, client_state_t *clientFds,
    struct pollfd *poll_fds);
int has_pending_connection(int fd, int timeout_ms);

#endif /* HANDLING_CONNECTION_H */
