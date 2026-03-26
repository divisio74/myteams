/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** list.c
*/

#include "list.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static int loop_on_files(DIR *dir, int client_fd)
{
    struct dirent *entry = readdir(dir);

    while (entry) {
        if (write(client_fd, entry->d_name, strlen(entry->d_name)) == -1
            || write(client_fd, "\n", 1) == -1) {
            fprintf(stderr, "LIST error: write on data socket failed: %s\n",
                strerror(errno));
            return -1;
        }
        entry = readdir(dir);
    }
    return 0;
}

static int list_file(const char *path, int client_fd)
{
    DIR *dir = opendir(path);

    if (!dir) {
        fprintf(stderr, "LIST error: cannot open '%s': %s\n", path,
            strerror(errno));
        return -1;
    }
    if (loop_on_files(dir, client_fd) == -1) {
        closedir(dir);
        return -1;
    }
    closedir(dir);
    return 0;
}

static int list_on_folders(char *args, int data_client_fd)
{
    char *folder = strtok(args, " ");

    while (folder) {
        if (list_file(folder, data_client_fd) == -1)
            return -1;
        folder = strtok(NULL, " ");
    }
    return 0;
}

static int list_on_cwd(int data_client_fd)
{
    char *cwd = getcwd(NULL, 0);

    if (!cwd) {
        fprintf(stderr, "LIST error: getcwd failed: %s\n", strerror(errno));
        return -1;
    }
    if (list_file(cwd, data_client_fd) == -1) {
        free(cwd);
        return -1;
    }
    free(cwd);
    return 0;
}

int list_transfer(char *args, int data_client_fd)
{
    if (!args || args[0] == '\0')
        return list_on_cwd(data_client_fd);
    return list_on_folders(args, data_client_fd);
}
