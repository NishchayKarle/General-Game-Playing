#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

typedef struct Move {
    int i, j;
} Move;

void setup_players(Game *g) {
    printf("Player1 - 'X': Enter your name (up to 10 characters): ");
    scanf("%10s", g->player1);
    getchar();

    printf("Player2 - 'O': Enter your name (up to 10 characters): ");
    scanf("%10s", g->player2);
    getchar();
}

void create_and_set_game_state(Game *g) {
    char *board = (char *)malloc(sizeof(char) * 10);
    memset(board, '\0', 10);

    g->game_state = (void *)board;
}

bool is_valid_move(Game *g, Move *m) {
    if (m->i < 0 || m->i >= 3 || m->j < 0 || m->j >= 3) {
        printf(
            "Invalid move! The move is out of bounds. Please enter row and "
            "column values between 1 and 3.\n");
        return false;
    }

    char *board = (char *)g->game_state;

    if (board[m->i * 3 + m->j] != '\0') {
        printf(
            "Invalid move! The cell at row %d, column %d is already occupied. "
            "Please choose an empty cell.\n",
            m->i + 1, m->j + 1);
        return false;
    }

    return true;
}

void make_move(Game *g, Move *m) {
    char *board = (char *)g->game_state;
    int index = m->i * 3 + m->j;

    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';
    board[index] = symbol;

    free(m);
}

Move *get_move(Game *g) {
    Move *move = (Move *)malloc(sizeof(Move));
    char input[10];
    printf("Please enter your move in the format 'row,column' (e.g., 1,2):\n");

    while (1) {
        printf("%s - '%c' - Enter your move: ",
               g->player_turn == PLAYER1 ? g->player1 : g->player2,
               g->player_turn == PLAYER1 ? 'X' : 'O');
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';

            if (sscanf(input, "%d,%d", &move->i, &move->j) == 2) {
                move->i -= 1;
                move->j -= 1;
                return move;
            } else {
                printf(
                    "Invalid input format! Please enter your move as "
                    "'row,column' (e.g., 2,3).\n");
            }
        } else {
            printf("Error reading input. Please try again.\n");
        }
    }
}

bool is_game_won(char *board) {
    for (int i = 0; i < 3; i++) {
        if (board[i * 3] != '\0' && board[i * 3] == board[i * 3 + 1] &&
            board[i * 3 + 1] == board[i * 3 + 2]) {
            return true;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (board[i] != '\0' && board[i] == board[i + 3] &&
            board[i + 3] == board[i + 6]) {
            return true;
        }
    }

    if (board[0] != '\0' && board[0] == board[4] && board[4] == board[8]) {
        return true;
    }

    if (board[2] != '\0' && board[2] == board[4] && board[4] == board[6]) {
        return true;
    }

    return false;
}

bool is_game_drawn(char *board) {
    if (is_game_won(board)) {
        return false;
    }

    for (int i = 0; i < 9; i++) {
        if (board[i] == '\0') {
            return false;
        }
    }

    return true;
}

bool is_game_over(Game *g) {
    return is_game_drawn(g->game_state) || is_game_won(g->game_state);
}

int get_game_result(Game *g) {
    if (is_game_drawn(g->game_state)) return GAME_DRAWN;

    if (g->player_turn == PLAYER1) return GAME_WON_BY_PLAYER1;

    return GAME_WON_BY_PLAYER2;
}

void print_game_state(Game *g) {
    char *board = (char *)g->game_state;

    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i * 3 + j] == '\0' ? ' ' : board[i * 3 + j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

void end_game(Game *g) {
    free((char *)g->game_state);
}