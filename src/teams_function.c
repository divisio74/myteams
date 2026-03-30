/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** teams_function
*/

#include "mylist.h"
#include "mylistl.h"
#include "teams_function.h"
#include "user_function.h"
#include <stdlib.h>
#include <string.h>

int teams_init(teams_t *teams)
{
    if (!teams)
        return -1;
    memset(teams, 0, sizeof(*teams));
    teams->users = instantiate_linked_list();
    if (!teams->users) {
        teams->users = NULL;
        return -1;
    }
    return 0;
}

void teams_free(teams_t *teams)
{
    linked_list_t *current = NULL;
    linked_list_t *next = NULL;

    if (!teams || !teams->users)
        return;
    current = teams->users;
    while (current) {
        next = current->next;
        free_user(current->data);
        free(current);
        current = next;
    }
    teams->users = NULL;
}
