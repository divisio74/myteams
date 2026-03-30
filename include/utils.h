/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** utils.h
*/

#ifndef UTILS_H
    #define UTILS_H


int parse_server_port(char *port, int *port_buffer);
void split_with_separator(char *input, char **operant,
    char **parameters, char separator);

int str_to_word_array(char *input, char *separator, char ***tokens);

void free_str_to_word_array(char **tokens);

#endif /* UTILS_H */
