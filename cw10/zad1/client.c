#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
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

void handle_reply(char* reply);

void quit() {
    sprintf(buffer, "quit: :%s", nickname);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

    exit(0);
}

void check_win_condition() {
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
        quit();
    }
}

void get_move() {
    char symbols[3] = {' ', 'O', 'X'};
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            symbols[0] = y * 3 + x + 1 + '0';
            printf("|%c|", symbols[board.objects[y * 3 + x]]);
        }
        puts("\n---------");
    }

    check_win_condition();

    if (is_o == board.o_move) {
        int move;
        do {
            printf("Enter next move (%c): ", is_o ? 'O' : 'X');
            scanf("%d", &move);
            move--;
        } while (!make_move(&board, move));

        sprintf(buffer, "move:%d:%s", move, nickname);
        puts(buffer);
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        check_win_condition();
        get_move();
    } else {
        puts("Waiting for enemy to make a move...");
        recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        handle_reply(buffer);
    }
}

void handle_reply(char* reply) {
    char* cmd = strtok(reply, ":");
    char* arg = strtok(NULL, ":");

    if (strcmp(cmd, "add") == 0) {
        if (strcmp(arg, "name_taken") == 0) {
            puts("This nickname is already taken");
            exit(1);
        } else if (strcmp(arg, "no_enemy") == 0) {
            puts("Game will begin as soon as the other player joins the game");
            recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
            handle_reply(buffer);
        } else if (arg[0] == 'X' || arg[0] == 'O') {
            board = new_board();
            is_o = arg[0] == 'O';
            get_move();
        }
    }
    if (strcmp(cmd, "move") == 0) {
        int move = atoi(arg);
        make_move(&board, move);
        get_move();
    }
    if (strcmp(cmd, "quit") == 0) {
        puts("Other player left the game :(");
        quit();
    }
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
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

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
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("localhost", destination, &hints, &info);

        server_socket =
            socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }

    sprintf(buffer, "add: :%s", nickname);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    handle_reply(buffer);
}