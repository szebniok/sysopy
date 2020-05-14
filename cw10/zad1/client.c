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

void check_win_condition() {
    board_object winner = get_winner(&board);
    if (winner != EMPTY) {
        if ((is_o && winner == O) || (!is_o && winner == X)) {
            puts("You have won the game!");
        } else {
            puts("You have lost :(");
        }

        sprintf(buffer, "quit: :%s", nickname);
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

        exit(0);
    }
}

void get_move() {
    char symbols[3] = {' ', 'O', 'X'};
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            symbols[0] = y * 3 + x + '0';
            printf("|%c|", symbols[board.objects[y * 3 + x]]);
        }
        puts("\n---------");
    }

    check_win_condition();

    if (is_o == board.o_move) {
        int move;
        do {
            printf("Enter next move: ");
            scanf("%d", &move);
        } while (!make_move(&board, move));

        sprintf(buffer, "move:%d:%s", move, nickname);
        puts(buffer);
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        check_win_condition();
        get_move();
    } else {
        puts("Waiting for enemy to make a move...");
        recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        perror("recv");
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
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./client nickname type address");
        return 1;
    }

    nickname = argv[1];
    // char *type = argv[2];
    char* address = argv[3];

    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    server_socket = local_socket;

    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, address);

    connect(local_socket, (struct sockaddr*)&local_sockaddr,
            sizeof(struct sockaddr_un));

    sprintf(buffer, "add: :%s", nickname);
    send(local_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    recv(local_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    handle_reply(buffer);
}