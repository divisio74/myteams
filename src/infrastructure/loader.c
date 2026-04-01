/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** loader
*/

#include <stdio.h>
#include "infrastructure.h"

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

void *getfunction_by_name(void **handle, const char *function_name)
{
    void *my_entry_point = NULL;
    const char *error = NULL;

    if (!handle || !*handle) {
        fprintf(stderr, "Error finding symbol: invalid handle\n");
        return NULL;
    }
    if (!function_name || function_name[0] == '\0') {
        fprintf(stderr, "Error finding symbol: empty symbol name\n");
        return NULL;
    }
    dlerror();
    my_entry_point = dlsym(*handle, function_name);
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error finding symbol: %s\n", error);
        dlclose(*handle);
        return NULL;
    }
    return my_entry_point;
}

void *getfunction(void **handle)
{
    return getfunction_by_name(handle, FUNCTION_GET_LIB);
}

void unload(void *handle)
{
    if (!handle)
        return;
    dlclose(handle);
}
