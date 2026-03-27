/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** main.c
*/

#include "handling_connection.h"
#include "mylistl.h"
#include "operations_2.h"
#include "socket.h"
#include "struct.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char HELP_MSG[] = "USAGE: ./myteams_server port\n";

static void free_space(int fd, client_state_t *clientFds,
                       struct pollfd *poll_fds) {
  if (fd >= 0)
    close(fd);
  free(clientFds);
  free(poll_fds);
}

static int parse_command(char *port, int *port_buffer) {
  if (parse_server_port(port, port_buffer) == -1)
    return -1;
  return 0;
}

static int main_boucle(int fd, client_state_t *clientFds,
                       struct pollfd *poll_fds, teams_t *servor) {
  if (accept_new_client(fd, clientFds, poll_fds) == -1)
    return -1;
  if (poll(poll_fds, MAX_SOCKET, 0) == -1) {
    fprintf(stderr, "Erreur: Impossible d'effectuer le poll: %s\n",
            strerror(errno));
    return -1;
  }
  if (handle_client_events(servor, clientFds, poll_fds) == -1)
    return -1;
  return 0;
}

static int run_server(int port) {
  int fd;
  teams_t servor = {0};
  client_state_t *clientFds = calloc(MAX_SOCKET, sizeof(client_state_t));
  struct pollfd *poll_fds = calloc(MAX_SOCKET, sizeof(struct pollfd));

  servor.users = instantiate_linked_list();
  if (servor.users == NULL) {
    free(clientFds);
    free(poll_fds);
    return 84;
  }
  fd = socket_initiation(port);
  if (fd == -1) {
    free(servor.users);
    free(clientFds);
    free(poll_fds);
    return 84;
  }
  while (1) {
    if (main_boucle(fd, clientFds, poll_fds, &servor) == -1) {
      free(servor.users);
      free_space(fd, clientFds, poll_fds);
      return -1;
    }
  }
  free(servor.users);
  free_space(fd, clientFds, poll_fds);
  return 0;
}

static int validate_arg_count(int argc) {
  if (argc != 2) {
    fprintf(stderr, "Erreur: Nombre d'arguments invalide.\n");
    return -1;
  }
  return 0;
}

int main(int argc, char **argv) {
  int port_buffer;

  if (validate_arg_count(argc) == -1)
    return 84;
  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    printf("%s", HELP_MSG);
    return 0;
  }
  if (parse_command(argv[1], &port_buffer) == -1) {
    fprintf(stderr, "Erreur: Arguments invalides.\n");
    return 84;
  }
  return run_server(port_buffer);
}
