/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** user_function.c
*/

#include "job_logic.h"
#include "mylistl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_user_uid(void *user_data, void *uid_data)
{
    user_t *user = user_data;
    char *uid = uid_data;

    if (!user || !uid || !user->uid)
        return 0;
    if (strcmp(user->uid, uid) == 0)
        return 1;
    return 0;
}

int compare_user_name(void *user_data, void *name_data)
{
    user_t *user = user_data;
    char *name = name_data;

    if (!user || !name || !user->name)
        return 0;
    if (strcmp(user->name, name) == 0)
      return 1;
    return 0;
}

static char *generate_uid(void)
{
    static size_t uid = 0;
    char buffer[32];
    int written = 0;

    written = snprintf(buffer, sizeof(buffer), "%zu", uid);
    uid++;
    if (written < 0 || (size_t)written >= sizeof(buffer)) {
        fprintf(stderr, "Error: Failed to generate UID\n");
        return NULL;
    }
    return strdup(buffer);
}

user_t *init_user(const char *name)
{
    user_t *user = malloc(sizeof(user_t));

    if (!user) {
      fprintf(stderr, "Error: Failed to allocate memory for user\n");
      return NULL;
    }
    user->name = NULL;
    user->uid = NULL;
    if (name)
        user->name = strdup(name);
    user->uid = generate_uid();
    if ((name && !user->name) || !user->uid) {
        return NULL;
    }
    return user;
}

user_t *find_or_create_user(teams_t *teams, const char *name)
{
    user_t *user = NULL;

    if (!teams || !teams->users || !name)
        return NULL;
    user = my_find_node(teams->users, (void *)name, compare_user_name);
    if (user)
        return user;
    user = init_user(name);
    if (!user)
        return NULL;
    add_node(teams->users, user);
    return user;
}

void free_user(void *data)
{
    user_t *user = data;

    if (!user)
        return;
    free(user->name);
    free(user->uid);
    free(user);
}
