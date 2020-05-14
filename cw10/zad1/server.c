#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

typedef struct {
    char* nickname;
    int fd;
} client_t;

client_t* clients[MAX_PLAYERS] = {NULL};
int clients_count = 0;

int poll_sockets(int local_socket, int network_socket) {
    struct pollfd* pfds = calloc(2 + clients_count, sizeof(struct pollfd));
    pfds[0].fd = local_socket;
    pfds[0].events = POLLIN;
    pfds[1].fd = network_socket;
    pfds[1].events = POLLIN;

    for (int i = 0; i < clients_count; i++) {
        pfds[i + 2].fd = clients[i]->fd;
        pfds[i + 2].events = POLLIN;
    }

    poll(pfds, clients_count + 2, -1);

    int retval;
    for (int i = 0; i < clients_count + 2; i++) {
        if (pfds[i].revents & POLLIN) {
            retval = pfds[i].fd;
            break;
        }
    }

    if (retval == local_socket || retval == network_socket) {
        retval = accept(retval, NULL, NULL);
    }

    free(pfds);

    return retval;
}

int get_by_nickname(char* nickname) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->nickname, nickname) == 0) {
            return i;
        }
    }
    return -1;
}

int get_opponent(int index) { return index % 2 == 0 ? index + 1 : index - 1; }

int add_client(char* nickname, int fd) {
    if (get_by_nickname(nickname) != -1) return -1;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] == NULL) {
            client_t* new_client = calloc(1, sizeof(client_t));
            new_client->nickname = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
            strcpy(new_client->nickname, nickname);
            new_client->fd = fd;

            clients[i] = new_client;
            clients_count++;

            return i;
        }
    }

    return -1;
}

void remove_client(char* nickname) {
    int client_index = get_by_nickname(nickname);
    if (client_index == -1) return;

    int opponent_index = get_opponent(client_index);
    int first_index =
        client_index < opponent_index ? client_index : opponent_index;

    free(clients[first_index]->nickname);
    free(clients[first_index]);
    free(clients[first_index + 1]->nickname);
    free(clients[first_index + 1]);

    if (clients_count == MAX_PLAYERS) {
        clients[first_index] = NULL;
        clients[first_index + 1] = NULL;
    } else {
        clients[first_index] = clients[clients_count - 2];
        clients[first_index + 1] = clients[clients_count - 1];
    }
    clients_count -= 2;
}

int setup_local_socket(char* path) {
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);

    unlink(path);
    bind(local_socket, (struct sockaddr*)&local_sockaddr,
         sizeof(struct sockaddr_un));

    listen(local_socket, MAX_BACKLOG);

    return local_socket;
}

int setup_network_socket(char* port) {
    struct addrinfo* info;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &info);

    int network_socket =
        socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(network_socket, info->ai_addr, info->ai_addrlen);
    perror("bind");

    listen(network_socket, MAX_BACKLOG);
    perror("listen");

    freeaddrinfo(info);

    return network_socket;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./server port path");
        return 1;
    }

    char* port = argv[1];
    char* socket_path = argv[2];

    srand(time(NULL));

    int local_socket = setup_local_socket(socket_path);
    int network_socket = setup_network_socket(port);

    while (1) {
        int client_fd = poll_sockets(local_socket, network_socket);

        char buffer[MAX_MESSAGE_LENGTH + 1];
        recv(client_fd, buffer, MAX_MESSAGE_LENGTH, 0);
        puts(buffer);

        char* cmd = strtok(buffer, ":");
        char* arg = strtok(NULL, ":");
        char* nickname = strtok(NULL, ":");

        if (strcmp(cmd, "add") == 0) {
            int index = add_client(nickname, client_fd);

            if (index == -1) {
                strcpy(buffer, "add:name_taken");
                send(client_fd, buffer, MAX_MESSAGE_LENGTH, 0);
                close(client_fd);
            } else if (index % 2 == 0) {
                strcpy(buffer, "add:no_enemy");
                send(client_fd, buffer, MAX_MESSAGE_LENGTH, 0);
            } else {
                int waiting_client_goes_first = rand() % 2;
                int first_player_index = index - waiting_client_goes_first;
                int second_player_index = get_opponent(first_player_index);

                strcpy(buffer, "add:O");
                send(clients[first_player_index]->fd, buffer,
                     MAX_MESSAGE_LENGTH, 0);
                strcpy(buffer, "add:X");
                send(clients[second_player_index]->fd, buffer,
                     MAX_MESSAGE_LENGTH, 0);
            }
        }
        if (strcmp(cmd, "move") == 0) {
            int move = atoi(arg);
            int player = get_by_nickname(nickname);

            sprintf(buffer, "move:%d", move);
            send(clients[get_opponent(player)]->fd, buffer, MAX_MESSAGE_LENGTH,
                 0);
        }
        if (strcmp(cmd, "quit") == 0) {
            remove_client(nickname);
        }
    }
}