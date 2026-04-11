/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** thread_function
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

static char *generate_thread_uid(void)
{
    static size_t uid = 0;
    char buffer[32];
    int written = 0;

    written = snprintf(buffer, sizeof(buffer), "th%zu", uid);
    uid++;
    if (written < 0 || (size_t)written >= sizeof(buffer))
        return NULL;
    return strdup(buffer);
}

void free_thread(void *data)
{
    thread_t *thread = data;

    if (!thread)
        return;
    free(thread->uid);
    free(thread->name);
    free(thread->body);
    free(thread->creator_uid);
    free_node_list(thread->messages, free_message);
    free(thread);
}

thread_t *init_thread(const char *name, const char *body,
    const char *creator_uid)
{
    thread_t *thread = malloc(sizeof(thread_t));

    if (!thread)
        return NULL;
    thread->uid = generate_thread_uid();
    thread->name = NULL;
    thread->body = NULL;
    thread->creator_uid = NULL;
    if (name)
        thread->name = strdup(name);
    if (body)
        thread->body = strdup(body);
    if (creator_uid)
        thread->creator_uid = strdup(creator_uid);
    thread->messages = instantiate_linked_list();
    if (!thread->uid || (name && !thread->name) || (body && !thread->body)
        || (creator_uid && !thread->creator_uid) || !thread->messages) {
        free_thread(thread);
        return NULL;
    }
    return thread;
}

int compare_thread_uid(void *thread_data, void *uid_data)
{
    thread_t *thread = thread_data;
    char *uid = uid_data;

    if (!thread || !uid || !thread->uid)
        return 0;
    if (strcmp(thread->uid, uid) == 0)
        return 1;
    return 0;
}