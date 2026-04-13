/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** teams_function
*/

#include "mylist.h"
#include "mylistl.h"
#include "job_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_node_list(linked_list_t *list, void (*free_data)(void *))
{
    linked_list_t *current = list;
    linked_list_t *next = NULL;

    while (current) {
        next = current->next;
        if (free_data)
            free_data(current->data);
        free(current);
        current = next;
    }
}

int compare_team_uid(void *team_data, void *uid_data)
{
    team_t *team = team_data;
    char *uid = uid_data;

    if (!team || !uid || !team->uid)
        return 0;
    if (strcmp(team->uid, uid) == 0)
        return 1;
    return 0;
}

int compare_string(void *str_a, void *str_b)
{
    if (!str_a || !str_b)
        return 0;
    if (strcmp(str_a, str_b) == 0)
        return 1;
    return 0;
}

static char *generate_team_uid(void)
{
    static size_t uid = 0;
    char buffer[32];
    int written = 0;

    written = snprintf(buffer, sizeof(buffer), "t%zu", uid);
    uid++;
    if (written < 0 || (size_t)written >= sizeof(buffer))
        return NULL;
    return strdup(buffer);
}

static int init_team_strings(team_t *team, const char *name,
    const char *description, const char *creator_uid)
{
    team->name = NULL;
    team->description = NULL;
    team->creator_uid = NULL;
    if (name)
        team->name = strdup(name);
    if (description)
        team->description = strdup(description);
    if (creator_uid)
        team->creator_uid = strdup(creator_uid);
    if ((name && !team->name) || (description && !team->description)
        || (creator_uid && !team->creator_uid))
        return -1;
    return 0;
}

static int init_team_lists(team_t *team)
{
    team->subscribers = instantiate_linked_list();
    team->channels = instantiate_linked_list();
    if (!team->subscribers || !team->channels)
        return -1;
    return 0;
}

team_t *init_team(const char *name, const char *description,
    const char *creator_uid)
{
    team_t *team = malloc(sizeof(team_t));

    if (!team)
        return NULL;
    team->uid = generate_team_uid();
    if (!team->uid || init_team_strings(team, name, description,
        creator_uid) == -1 || init_team_lists(team) == -1) {
        free_team(team);
        return NULL;
    }
    return team;
}

void free_team(void *data)
{
    team_t *team = data;

    if (!team)
        return;
    free(team->uid);
    free(team->name);
    free(team->description);
    free(team->creator_uid);
    free_node_list(team->subscribers, free);
    free_node_list(team->channels, free_channel);
    free(team);
}
