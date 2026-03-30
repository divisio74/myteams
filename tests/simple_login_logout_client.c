/*
** EPITECH PROJECT, 2026
** G-NWP-400-LYN-4-1-myteams-3
** File description:
** simple_login_logout_client.c
*/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static int connect_to_server(const char *host, int port) {
  int sockfd = -1;
  struct sockaddr_in addr = {0};

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    return -1;
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons((unsigned short)port);
  if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
    fprintf(stderr, "invalid host: %s\n", host);
    close(sockfd);
    return -1;
  }
  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    close(sockfd);
    return -1;
  }
  return sockfd;
}

static int set_nonblocking(int sockfd) {
  int flags = fcntl(sockfd, F_GETFL, 0);

  if (flags == -1) {
    perror("fcntl(F_GETFL)");
    return -1;
  }
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl(F_SETFL)");
    return -1;
  }
  return 0;
}

static int wait_socket_readable(int sockfd, int timeout_ms) {
  struct pollfd pfd = {0};
  int rc = 0;

  pfd.fd = sockfd;
  pfd.events = POLLIN;
  rc = poll(&pfd, 1, timeout_ms);
  if (rc == -1) {
    perror("poll");
    return -1;
  }
  if (rc == 0) {
    fprintf(stderr, "timeout while waiting for server response\n");
    return -1;
  }
  return 0;
}

static int read_line(int sockfd, char *buffer, size_t size) {
  size_t i = 0;
  ssize_t rd = 0;

  if (!buffer || size == 0)
    return -1;
  while (i + 1 < size) {
    if (wait_socket_readable(sockfd, 2000) == -1)
      return -1;
    rd = recv(sockfd, &buffer[i], 1, 0);
    if (rd == 0)
      break;
    if (rd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        continue;
      perror("recv");
      return -1;
    }
    if (buffer[i] == '\n') {
      i++;
      break;
    }
    i++;
  }
  buffer[i] = '\0';
  if (i > 0 && buffer[i - 1] == '\n')
    buffer[i - 1] = '\0';
  i = strlen(buffer);
  if (i > 0 && buffer[i - 1] == '\r')
    buffer[i - 1] = '\0';
  return 0;
}

static int send_command(int sockfd, const char *cmd) {
  char out[512];
  size_t len = 0;

  if (!cmd)
    return -1;
  if (snprintf(out, sizeof(out), "%s\\r\\n", cmd) >= (int)sizeof(out)) {
    fprintf(stderr, "command too long\n");
    return -1;
  }
  len = strlen(out);
  if (send(sockfd, out, len, 0) != (ssize_t)len) {
    perror("send");
    return -1;
  }
  printf("C: %s\n", cmd);
  return 0;
}

static void print_usage(const char *bin) {
  printf("USAGE:\n");
  printf("  %s <host> <port> <username> [login_fmt] [logout_cmd]\n", bin);
  printf("\nDEFAULTS (RFC style):\n");
  printf("  login_fmt = \"LOGIN %%s\"\n");
  printf("  logout_cmd = \"LOGOUT\"\n");
  printf("\nEXAMPLES:\n");
  printf("  %s 127.0.0.1 4242 alice\n", bin);
  printf("  %s 127.0.0.1 4242 alice \"/login %%s\" \"/logout\"\n", bin);
}

static int wait_for_welcome(int sockfd, char *line, size_t size) {
  if (read_line(sockfd, line, size) == -1) {
    fprintf(stderr, "failed to read server welcome message\n");
    return -1;
  }
  printf("S: %s\n", line);
  if (strncmp(line, "220 ", 4) != 0) {
    fprintf(stderr, "unexpected welcome response, expected 220\n");
    return -1;
  }
  return 0;
}

int main(int argc, char **argv) {
  int sockfd = -1;
  int port = 0;
  char line[1024];
  char login_cmd[256];
  const char *host = NULL;
  const char *username = NULL;
  const char *login_fmt = "LOGIN %s";
  const char *logout_cmd = "LOGOUT";

  if (argc < 4 || argc > 6) {
    print_usage(argv[0]);
    return 1;
  }
  host = argv[1];
  port = atoi(argv[2]);
  username = argv[3];
  if (argc >= 5)
    login_fmt = argv[4];
  if (argc == 6)
    logout_cmd = argv[5];
  if (snprintf(login_cmd, sizeof(login_cmd), login_fmt, username) >=
      (int)sizeof(login_cmd)) {
    fprintf(stderr, "login command format generated a command too long\n");
    return 1;
  }
  sockfd = connect_to_server(host, port);
  if (sockfd == -1)
    return 1;
  if (set_nonblocking(sockfd) == -1) {
    close(sockfd);
    return 1;
  }

  if (wait_for_welcome(sockfd, line, sizeof(line)) == -1) {
    close(sockfd);
    return 1;
  }

  if (send_command(sockfd, login_cmd) == -1 ||
      read_line(sockfd, line, sizeof(line)) == -1) {
    close(sockfd);
    return 1;
  }
  printf("S: %s\n", line);

  if (send_command(sockfd, logout_cmd) == -1 ||
      read_line(sockfd, line, sizeof(line)) == -1) {
    close(sockfd);
    return 1;
  }
  printf("S: %s\n", line);

  close(sockfd);
  return 0;
}
