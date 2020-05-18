#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <poll.h>
#include <pthread.h>
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
    struct sockaddr addr;
    int is_alive;
} client_t;

client_t* clients[MAX_PLAYERS] = {NULL};
int clients_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int poll_sockets(int local_socket, int network_socket) {
    struct pollfd pfds[2];
    pfds[0].fd = local_socket;
    pfds[0].events = POLLIN;
    pfds[1].fd = network_socket;
    pfds[1].events = POLLIN;

    poll(pfds, 2, -1);

    for (int i = 0; i < 2; i++) {
        if (pfds[i].revents & POLLIN) {
            return pfds[i].fd;
        }
    }

    return -1;
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

int add_client(char* nickname, struct sockaddr addr, int fd) {
    if (get_by_nickname(nickname) != -1) return -1;

    int index = -1;
    // check if there is another player waiting for an opponent
    for (int i = 0; i < MAX_PLAYERS; i += 2) {
        if (clients[i] != NULL && clients[i + 1] == NULL) {
            index = i + 1;
            break;
        }
    }

    // if no opponent avaible, get first free place
    for (int i = 0; i < MAX_PLAYERS && index == -1; i++) {
        if (clients[i] == NULL) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        client_t* new_client = calloc(1, sizeof(client_t));
        new_client->nickname = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(new_client->nickname, nickname);
        new_client->addr = addr;
        new_client->fd = fd;
        new_client->is_alive = 1;

        clients[index] = new_client;
        clients_count++;
    }

    return index;
}

void remove_client(char* nickname) {
    printf("removing client: %s\n", nickname);
    int client_index = get_by_nickname(nickname);
    if (client_index == -1) return;

    free(clients[client_index]->nickname);
    free(clients[client_index]);
    clients[client_index] = NULL;
    clients_count--;

    int opponent_index = get_opponent(client_index);

    if (clients[opponent_index] != NULL) {
        puts("removing opponent");
        sendto(clients[opponent_index]->fd, "quit: ", MAX_MESSAGE_LENGTH, 0,
               &clients[opponent_index]->addr, sizeof(struct addrinfo));
        free(clients[opponent_index]->nickname);
        free(clients[opponent_index]);
        clients[opponent_index] = NULL;
        clients_count--;
    }
}

void pinging_loop() {
    puts("pinging");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL && !clients[i]->is_alive) {
            printf("removing ping: %s\n", clients[i]->nickname);
            remove_client(clients[i]->nickname);
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL) {
            sendto(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0,
                   &clients[i]->addr, sizeof(struct addrinfo));
            clients[i]->is_alive = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    sleep(2);
    pinging_loop();
}

int setup_local_socket(char* path) {
    int local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);

    unlink(path);
    bind(local_socket, (struct sockaddr*)&local_sockaddr,
         sizeof(struct sockaddr_un));

    return local_socket;
}

int setup_network_socket(char* port) {
    struct addrinfo* info;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &info);

    int network_socket =
        socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(network_socket, info->ai_addr, info->ai_addrlen);

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

    pthread_t t;
    pthread_create(&t, NULL, (void* (*)(void*))pinging_loop, NULL);

    while (1) {
        int socket_fd = poll_sockets(local_socket, network_socket);

        char buffer[MAX_MESSAGE_LENGTH + 1];
        struct sockaddr from_addr;
        socklen_t from_length = sizeof(struct sockaddr);
        recvfrom(socket_fd, buffer, MAX_MESSAGE_LENGTH, 0, &from_addr,
                 &from_length);
        puts(buffer);

        char* cmd = strtok(buffer, ":");
        char* arg = strtok(NULL, ":");
        char* nickname = strtok(NULL, ":");

        pthread_mutex_lock(&clients_mutex);
        if (strcmp(cmd, "add") == 0) {
            int index = add_client(nickname, from_addr, socket_fd);

            if (index == -1) {
                sendto(socket_fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0,
                       (struct sockaddr*)&from_addr, sizeof(struct addrinfo));
            } else if (index % 2 == 0) {
                sendto(socket_fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0,
                       (struct sockaddr*)&from_addr, sizeof(struct addrinfo));
            } else {
                int waiting_client_goes_first = rand() % 2;
                int first_player_index = index - waiting_client_goes_first;
                int second_player_index = get_opponent(first_player_index);

                sendto(clients[first_player_index]->fd, "add:O",
                       MAX_MESSAGE_LENGTH, 0,
                       &clients[first_player_index]->addr,
                       sizeof(struct addrinfo));
                sendto(clients[second_player_index]->fd, "add:X",
                       MAX_MESSAGE_LENGTH, 0,
                       &clients[second_player_index]->addr,
                       sizeof(struct addrinfo));
            }
        }
        if (strcmp(cmd, "move") == 0) {
            int move = atoi(arg);
            int player = get_by_nickname(nickname);

            sprintf(buffer, "move:%d", move);
            sendto(clients[get_opponent(player)]->fd, buffer,
                   MAX_MESSAGE_LENGTH, 0, &clients[get_opponent(player)]->addr,
                   sizeof(struct addrinfo));
        }
        if (strcmp(cmd, "quit") == 0) {
            remove_client(nickname);
        }
        if (strcmp(cmd, "pong") == 0) {
            int player = get_by_nickname(nickname);
            if (player != -1) {
                clients[player]->is_alive = 1;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}