/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** handling_connection.c
*/

#include "infrastructure.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void cleanup_client(client_t *client) {
  free(client->output_buffer);
  free(client->user_uid);
  memset(client, 0, sizeof(client_t));
}

// return 1 if there is a pending connection, 0 if not, and -1 on error
int has_pending_connection(int fd, int timeout_ms) {
  struct pollfd pfd;

  pfd.fd = fd;
  pfd.events = POLLIN;
  if (poll(&pfd, 1, timeout_ms) == -1) {
    fprintf(stderr, "Erreur: Impossible d'effectuer le poll: %s\n",
            strerror(errno));
    return -1;
  }
  if (pfd.revents & POLLIN)
    return 1;
  return 0;
}

static int locate_available_socket(int fd, client_t *clientFds,
                                   int *client_fd) {
  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  int idx = 0;

  *client_fd = accept(fd, (struct sockaddr *)&client, &len);
  if (*client_fd == -1) {
    fprintf(stderr, "Erreur: Impossible d'accepter la connexion client: %s\n",
            strerror(errno));
    return -1;
  }
  while (idx < MAX_SOCKET && clientFds[idx].in_use)
    idx++;
  if (idx == MAX_SOCKET) {
    fprintf(stderr, "Erreur: Nombre maximum de sockets atteint.\n");
    close(*client_fd);
    return -1;
  }
  return idx;
}

static int handle_pending_client_connection(int fd, client_t *clientFds,
                                            struct pollfd *poll_fds,
                                            int has_client) {
  int idx;
  int client_fd;

  if (has_client == -1)
    return -1;
  if (has_client == 1) {
    idx = locate_available_socket(fd, clientFds, &client_fd);
    if (idx == -1)
      return -1;
    send_simple_response(client_fd, 220);
    clientFds[idx].in_use = true;
    clientFds[idx].client_fd = client_fd;
    poll_fds[idx].fd = client_fd;
    poll_fds[idx].events = POLLIN;
  }
  return has_pending_connection(fd, 0);
}

int accept_new_client(int fd, client_t *clientFds,
                      struct pollfd *poll_fds) {
  int has_client;

  has_client = has_pending_connection(fd, 0);
  while (has_client == 1) {
    has_client =
        handle_pending_client_connection(fd, clientFds, poll_fds, has_client);
    if (has_client == -1)
      return -1;
  }
  return 0;
}

// buffer et bytes_read sont temporaires
static void process_received_data(client_t *client, ssize_t bytes_read,
                                  server_t *server) {
  char *instruction = NULL;

  instruction = malloc((bytes_read + 1) * sizeof(char));
  if (instruction == NULL) {
    fprintf(stderr, "Erreur: Impossible d'allouer la mémoire.\n");
    return;
  }
  strncpy(instruction, server->buffer, bytes_read);
  instruction[bytes_read] = '\0';
  server->payload_cmd.client = client;
  server->payload_cmd.args = instruction;
  bufferise_client_commands(server);
  server->payload_cmd.args = NULL;
  free(instruction);
}

static int process_event(server_t *server, size_t index) {
  client_t *clientFds = server->client_fds;
  struct pollfd *poll_fds = server->poll_fds;
  ssize_t bytes_read;

  bytes_read =
      read(clientFds[index].client_fd, server->buffer, sizeof(server->buffer));
  if (bytes_read > 0) {
    process_received_data(&clientFds[index], bytes_read, server);
    return 0;
  }
  if (bytes_read == 0) {
    clientFds[index].in_use = false;
    return 0;
  }
  fprintf(stderr, "Erreur: Impossible de lire les données du client: %s\n",
          strerror(errno));
  close(poll_fds[index].fd);
  return -1;
}

int handle_client_events(server_t *server) {
  client_t *clientFds = server->client_fds;
  struct pollfd *poll_fds = server->poll_fds;
  int process_status;

  for (size_t i = 0; i < MAX_SOCKET; i++) {
    if (!clientFds[i].in_use || !(poll_fds[i].revents & POLLIN))
      continue;
    process_status = process_event(server, i);
    if (clientFds[i].in_use == false || process_status == -1) {
      close(poll_fds[i].fd);
      memset(&poll_fds[i], 0, sizeof(struct pollfd));
      cleanup_client(&clientFds[i]);
    }
  }
  return 0;
}
