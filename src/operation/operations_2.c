/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** operations_2.c
*/

#include "operations_2.h"
#include "mylistl.h"
#include "user_function.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int cmd_help(teams_t *servor, client_state_t *client_state, char *args,
             int client_fd) {
  (void)servor;
  (void)client_state;
  (void)args;
  (void)client_fd;
  return 0;
}

int cmd_login(teams_t *servor, client_state_t *client_state, char *args,
              int client_fd) {
  user_t *user = NULL;

  if (!args || !*args)
    return send_simple_response(client_fd, 501);
  user = my_find_node(servor->users, args, compare_user_name);
  if (!user) {
    user = init_user(args);
    if (!user) {
      fprintf(stderr, "Error: Failed to initialize user\n");
      return -1;
    }
    add_node(servor->users, user);
  }
  free(client_state->user_uid);
  client_state->user_uid = NULL;
  client_state->user_uid = strdup(user->uid);
  if (!client_state->user_uid)
    return -1;
  return 0;
}

int cmd_logout(teams_t *servor, client_state_t *client_state, char *args,
               int client_fd) {
  (void)servor;
  (void)args;
  (void)client_fd;
  free(client_state->user_uid);
  client_state->user_uid = NULL;
  return 0;
}

int cmd_users(teams_t *servor, client_state_t *client_state, char *args,
              int client_fd) {
  user_t *user = NULL;

  (void)client_state;
  (void)args;
  (void)client_fd;
  for (linked_list_t *node = servor->users; node; node = node->next) {
    user = node->data;
    if (!user || !user->uid || !user->name)
      continue;
    printf("%s %s\n", user->uid, user->name);
  }
  return 0;
}

int cmd_user(teams_t *servor, client_state_t *client_state, char *args,
             int client_fd) {
  user_t *user = NULL;

  (void)client_state;
  (void)client_fd;
  if (!args || !*args)
    return 0;
  user = my_find_node(servor->users, args, compare_user_uid);
  if (!user)
    return 0;
  printf("%s %s\n", user->uid, user->name);
  return 0;
}
