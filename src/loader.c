/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** loader
*/

#include <stdio.h>
#include "loader.h"

void *load(char *path)
{
    void *handle = NULL;

    if (!path || path[0] == '\0') {
        fprintf(stderr, "Error loading library: empty path\n");
        return NULL;
    }
    handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return NULL;
    }
    return handle;
}

void *getfunction(void **handle)
{
    void *my_entry_point = NULL;
    const char *error = NULL;

    if (!handle || !*handle) {
        fprintf(stderr, "Error finding symbol: invalid handle\n");
        return NULL;
    }
    dlerror();
    my_entry_point = dlsym(*handle, FUNCTION_GET_LIB);
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error finding symbol: %s\n", error);
        dlclose(*handle);
        *handle = NULL;
        return NULL;
    }
    return my_entry_point;
}

void unload(void *handle)
{
    if (!handle)
        return;
    dlclose(handle);
}
