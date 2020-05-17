#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "common.h"

int server_socket;
int is_o;
char buffer[MAX_MESSAGE_LENGTH + 1];
char* nickname;

board_t board;

typedef enum {
    START,
    WAIT_FOR_ENEMY,
    WAIT_FOR_MOVE,
    PROCESS_ENEMY_MOVE,
    MOVE,
    QUIT
} state_t;

state_t state = START;

char *cmd, *arg;

pthread_mutex_t reply_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reply_cond = PTHREAD_COND_INITIALIZER;

void quit() {
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "quit: :%s", nickname);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

    exit(0);
}

void check_board_status() {
    // check for a win
    int is_won = 0;
    board_object winner = get_winner(&board);
    if (winner != EMPTY) {
        if ((is_o && winner == O) || (!is_o && winner == X)) {
            puts("You have won the game!");
        } else {
            puts("You have lost :(");
        }

        is_won = 1;
    }

    // check for a draw
    int is_drawn = 1;
    for (int i = 0; i < 9; i++) {
        if (board.objects[i] == EMPTY) {
            is_drawn = 0;
            break;
        }
    }

    if (is_drawn && !is_won) {
        puts("Game ended in a draw");
    }

    if (is_won || is_drawn) {
        state = QUIT;
    }
}

void split_reply(char* reply) {
    cmd = strtok(reply, ":");
    arg = strtok(NULL, ":");
}

void draw_board() {
    char symbols[3] = {' ', 'O', 'X'};
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            symbols[0] = y * 3 + x + 1 + '0';
            printf("|%c|", symbols[board.objects[y * 3 + x]]);
        }
        puts("\n---------");
    }
}

void game_loop() {
    if (state == START) {
        if (strcmp(arg, "name_taken") == 0) {
            puts("This nickname is already taken");
            exit(1);
        } else if (strcmp(arg, "no_enemy") == 0) {
            puts("Game will begin as soon as the other player joins the game");
            state = WAIT_FOR_ENEMY;
        } else {
            board = new_board();
            is_o = arg[0] == 'O';
            state = is_o ? MOVE : WAIT_FOR_MOVE;
        }
    } else if (state == WAIT_FOR_ENEMY) {
        pthread_mutex_lock(&reply_mutex);
        while (state != START && state != QUIT) {
            pthread_cond_wait(&reply_cond, &reply_mutex);
        }
        pthread_mutex_unlock(&reply_mutex);

        board = new_board();
        is_o = arg[0] == 'O';
        state = is_o ? MOVE : WAIT_FOR_MOVE;
    } else if (state == WAIT_FOR_MOVE) {
        puts("Waiting for enemy to make a move");

        pthread_mutex_lock(&reply_mutex);
        while (state != PROCESS_ENEMY_MOVE && state != QUIT) {
            pthread_cond_wait(&reply_cond, &reply_mutex);
        }
        pthread_mutex_unlock(&reply_mutex);

    } else if (state == PROCESS_ENEMY_MOVE) {
        int move = atoi(arg);
        make_move(&board, move);
        check_board_status();
        if (state != QUIT) {
            state = MOVE;
        }
    } else if (state == MOVE) {
        draw_board();

        int move;
        do {
            printf("Enter next move (%c): ", is_o ? 'O' : 'X');
            scanf("%d", &move);
            move--;
        } while (!make_move(&board, move));

        draw_board();

        char buffer[MAX_MESSAGE_LENGTH + 1];
        sprintf(buffer, "move:%d:%s", move, nickname);
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

        check_board_status();
        if (state != QUIT) {
            state = WAIT_FOR_MOVE;
        }
    } else if (state == QUIT) {
        quit();
    }
    game_loop();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./client nickname type destination");
        return 1;
    }

    nickname = argv[1];
    char* type = argv[2];
    char* destination = argv[3];

    signal(SIGINT, quit);

    if (strcmp(type, "local") == 0) {
        server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);

        struct sockaddr_un local_sockaddr;
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);

        connect(server_socket, (struct sockaddr*)&local_sockaddr,
                sizeof(struct sockaddr_un));
    } else {
        struct addrinfo* info;

        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        getaddrinfo("localhost", destination, &hints, &info);

        server_socket =
            socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "add: :%s", nickname);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

    int game_thread_running = 0;

    while (1) {
        recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        split_reply(buffer);

        pthread_mutex_lock(&reply_mutex);
        if (strcmp(cmd, "add") == 0) {
            state = START;
            if (!game_thread_running) {
                pthread_t t;
                pthread_create(&t, NULL, (void* (*)(void*))game_loop, NULL);
                game_thread_running = 1;
            }
        } else if (strcmp(cmd, "move") == 0) {
            state = PROCESS_ENEMY_MOVE;
        } else if (strcmp(cmd, "quit") == 0) {
            state = QUIT;
            exit(0);
        } else if (strcmp(cmd, "ping") == 0) {
            sprintf(buffer, "pong: :%s", nickname);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&reply_cond);
        pthread_mutex_unlock(&reply_mutex);
    }
}