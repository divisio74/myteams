/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** args.c
*/

#include "client.h"
#include <stdlib.h>
#include <string.h>

static int read_quoted(const char **cursor, char **out)
{
    const char *p = *cursor + 1;
    const char *start = p;
    size_t len = 0;
    char *buf = NULL;

    while (*p && *p != '"')
        p++;
    len = (size_t)(p - start);
    buf = malloc(len + 1);
    if (!buf)
        return -1;
    memcpy(buf, start, len);
    buf[len] = '\0';
    *out = buf;
    *cursor = (*p == '"') ? p + 1 : p;
    return 0;
}

static int read_bare(const char **cursor, char **out)
{
    const char *p = *cursor;
    const char *start = p;
    size_t len = 0;
    char *buf = NULL;

    while (*p && *p != ' ' && *p != '\t')
        p++;
    len = (size_t)(p - start);
    buf = malloc(len + 1);
    if (!buf)
        return -1;
    memcpy(buf, start, len);
    buf[len] = '\0';
    *out = buf;
    *cursor = p;
    return 0;
}

int parse_arg(const char **cursor, char **out)
{
    const char *p = *cursor;

    *out = NULL;
    while (*p == ' ' || *p == '\t')
        p++;
    *cursor = p;
    if (!*p)
        return -1;
    if (*p == '"')
        return read_quoted(cursor, out);
    return read_bare(cursor, out);
}

int split_response(char *line, char **tokens, int max)
{
    int count = 0;
    char *p = line;

    while (count < max) {
        while (*p == ' ')
            p++;
        if (!*p)
            break;
        tokens[count++] = p;
        if (count == max)
            break;
        while (*p && *p != ' ')
            p++;
        if (*p) {
            *p = '\0';
            p++;
        }
    }
    return count;
}
