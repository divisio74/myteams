/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** utils.c
*/

#include "client.h"
#include <ctype.h>
#include <string.h>

char *clean_spaces(char *s)
{
    size_t len = 0;
    char *end = NULL;

    if (!s)
        return NULL;
    while (*s && isspace((unsigned char)*s))
        s++;
    len = strlen(s);
    if (len == 0)
        return s;
    end = s + len - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return s;
}
