/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** teams_function_2
*/

#include "mylist.h"
#include "mylistl.h"
#include "job_logic.h"
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

int teams_init(teams_t *teams)
{
    if (!teams)
        return -1;
    memset(teams, 0, sizeof(*teams));
    teams->users = instantiate_linked_list();
    teams->messages = instantiate_linked_list();
    teams->teams = instantiate_linked_list();
    if (!teams->users || !teams->messages || !teams->teams)
        return -1;
    return 0;
}

void teams_free(teams_t *teams)
{
    if (!teams)
        return;
    free_node_list(teams->users, free_user);
    teams->users = NULL;
    free_node_list(teams->messages, free_message);
    teams->messages = NULL;
    free_node_list(teams->teams, free_team);
    teams->teams = NULL;
}
