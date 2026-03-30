/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** bufferiser.h
*/

#ifndef BUFFERISER_H
#define BUFFERISER_H

#include "struct.h"

void split_with_space(char *input, char **operant, char **parameters);
void bufferise_client_operations(teams_t *server, client_t *client,
                                 char *args);

#endif /* BUFFERISER_H */
