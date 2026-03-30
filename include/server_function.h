/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** server_function
*/

#ifndef SERVER_FUNCTION_H_
    #define SERVER_FUNCTION_H_

    #include "struct.h"

int server_init(server_t *server, char *path);
void server_free(server_t *server);

#endif /* SERVER_FUNCTION_H_ */
