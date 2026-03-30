/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** user_function.h
*/

#ifndef USER_FUNCTION_H
    #define USER_FUNCTION_H

    #include "struct.h"

int compare_user_uid(void *user_data, void *uid_data);
int compare_user_name(void *user_data, void *name_data);
user_t *init_user(const char *name);
void free_user(void *data);

#endif /* USER_FUNCTION_H */
