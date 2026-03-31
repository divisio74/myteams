/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** main.c
*/

#include "infrastructure.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char HELP_MSG[] = "USAGE: ./myteams_server port\n";
static const char LIB_PATH[] = "lib/libmyteams.so";

static int parse_command(char *port, int *port_buffer) {
  if (parse_server_port(port, port_buffer) == -1)
    return -1;
  return 0;
}

static int main_boucle(int fd, server_t *server) {
  if (accept_new_client(fd, server->client_fds, server->poll_fds) == -1)
    return -1;
  if (poll(server->poll_fds, MAX_SOCKET, 0) == -1) {
    fprintf(stderr, "Erreur: Impossible d'effectuer le poll: %s\n",
            strerror(errno));
    return -1;
  }
  if (handle_client_events(server) == -1)
    return -1;
  return 0;
}

static int run_server(int port) {
  int fd = -1;
  server_t server;

  if (server_init(&server, (char *)LIB_PATH) == -1) {
    server_free(&server);
    return 84;
  }
  fd = socket_initiation(port);
  if (fd == -1) {
    server_free(&server);
    return 84;
  }
  while (true) {
    if (main_boucle(fd, &server) == -1) {
      break;
    }
  }
  close(fd);
  server_free(&server);
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
