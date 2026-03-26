/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** socket.c
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int socket_bind(int fd, int port)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        fprintf(stderr,
            "Erreur: Impossible de bind le socket sur le port %d: %s\n", port,
            strerror(errno));
        return -1;
    }
    return 0;
}

// return fd of the listening socket, or -1 on error
int socket_initiation(int port)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "Erreur: Impossible de créer le socket: %s\n",
            strerror(errno));
        return -1;
    }
    if (socket_bind(fd, port) == -1) {
        close(fd);
        return -1;
    }
    if (listen(fd, 5) == -1) {
        fprintf(stderr,
            "Erreur: Impossible de mettre le socket en écoute: %s\n",
            strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}
