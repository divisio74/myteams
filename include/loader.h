/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** loader
*/

#ifndef LOADER_H_
    #define LOADER_H_

    #include <dlfcn.h>

    #define FUNCTION_GET_LIB "get_lib"

void *load(char *path);
void *getfunction(void **handle);
void unload(void *handle);

#endif /* LOADER_H_ */
