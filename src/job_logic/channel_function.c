/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** channel_function
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

static char *generate_channel_uid(void)
{
    static size_t uid = 0;
    char buffer[32];
    int written = 0;

    written = snprintf(buffer, sizeof(buffer), "ch%zu", uid);
    uid++;
    if (written < 0 || (size_t)written >= sizeof(buffer))
        return NULL;
    return strdup(buffer);
}

void free_channel(void *data)
{
    channel_t *channel = data;

    if (!channel)
        return;
    free(channel->uid);
    free_node_list(channel->users, free);
    free_node_list(channel->threads, free_thread);
    free(channel);
}

channel_t *init_channel(void)
{
    channel_t *channel = malloc(sizeof(channel_t));

    if (!channel)
        return NULL;
    channel->uid = generate_channel_uid();
    channel->threads = instantiate_linked_list();
    channel->users = instantiate_linked_list();
    if (!channel->uid || !channel->threads || ! channel->users) {
        free_channel(channel);
        return NULL;
    }
    return channel;
}

int compare_channel_uid(void *channel_data, void *uid_data)
{
    channel_t *channel = channel_data;
    char *uid = uid_data;

    if (!channel || !uid || !channel->uid)
        return 0;
    if (strcmp(channel->uid, uid) == 0)
        return 1;
    return 0;
}