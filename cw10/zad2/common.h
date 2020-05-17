#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256

typedef enum { EMPTY, O, X } board_object;

typedef struct {
    int o_move;
    board_object objects[9];
} board_t;

board_t new_board() {
    board_t board = {1, {EMPTY}};
    return board;
}

int make_move(board_t* board, int position) {
    if (position < 0 || position > 9 || board->objects[position] != EMPTY)
        return 0;
    board->objects[position] = board->o_move ? O : X;
    board->o_move = !board->o_move;
    return 1;
}

board_object same_column(board_t* board) {
    for (int x = 0; x < 3; x++) {
        board_object first = board->objects[x];
        board_object second = board->objects[x + 3];
        board_object third = board->objects[x + 6];
        if (first == second && first == third && first != EMPTY) return first;
    }
    return EMPTY;
}

board_object same_row(board_t* board) {
    for (int y = 0; y < 3; y++) {
        board_object first = board->objects[3 * y];
        board_object second = board->objects[3 * y + 1];
        board_object third = board->objects[3 * y + 2];
        if (first == second && first == third && first != EMPTY) return first;
    }
    return EMPTY;
}

board_object same_diagonal(board_t* board) {
    board_object first = board->objects[3 * 0 + 0];
    board_object second = board->objects[3 * 1 + 1];
    board_object third = board->objects[3 * 2 + 2];
    if (first == second && first == third && first != EMPTY) return first;

    first = board->objects[3 * 0 + 2];
    second = board->objects[3 * 1 + 1];
    third = board->objects[3 * 2 + 1];
    if (first == second && first == third && first != EMPTY) return first;

    return EMPTY;
}

board_object get_winner(board_t* board) {
    board_object column = same_column(board);
    board_object row = same_row(board);
    board_object diagonal = same_diagonal(board);

    return column != EMPTY ? column : row != EMPTY ? row : diagonal;
}
