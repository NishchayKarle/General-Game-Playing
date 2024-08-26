#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define BOARD_SIZE 15

typedef struct Move {
    int r, c;
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
    char *board = (char *)malloc(sizeof(char) * BOARD_SIZE * BOARD_SIZE);
    memset(board, '.', BOARD_SIZE * BOARD_SIZE);

    g->board = (void *)board;
}

bool is_valid_move(Game *g, Move *m) {
    if (m->r <= 0 || m->r > BOARD_SIZE || m->c <= 0 || m->c > BOARD_SIZE) {
        printf(
            "Invalid move! The move is out of bounds. Please enter row and "
            "column values between 1 and %d.\n",
            BOARD_SIZE);
        return false;
    }

    char *board = (char *)g->board;

    int index = (m->r - 1) * BOARD_SIZE + (m->c - 1);
    if (board[index] != '.') {
        printf(
            "Invalid move! The cell at row %d, column %d is already occupied. "
            "Please choose an empty cell.\n",
            m->r, m->c);
        return false;
    }

    return true;
}

void make_move(Game *g, Move *m) {
    char *board = (char *)g->board;
    int index = (m->r - 1) * BOARD_SIZE + (m->c - 1);

    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';
    board[index] = symbol;

    free(m);
}

Move *get_move(Game *g) {
    Move *m = (Move *)malloc(sizeof(Move));
    char input[10];
    printf("Please enter your move in the format 'row,column' (e.g., 1,2):\n");

    while (1) {
        printf("%s - '%c' - Enter your move: ",
               g->player_turn == PLAYER1 ? g->player1 : g->player2,
               g->player_turn == PLAYER1 ? 'X' : 'O');
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';

            if (sscanf(input, "%d,%d", &m->r, &m->c) == 2) {
                return m;
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

GameState evaluate_game_state(Game *g) {
    char *board = (char *)g->board;

    // Check horizontal, vertical, and both diagonal directions for a win
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char current = board[i * BOARD_SIZE + j];
            if (current == 'X' || current == 'O') {
                // Check horizontal
                if (j <= BOARD_SIZE - 5) {
                    if (current == board[i * BOARD_SIZE + (j + 1)] &&
                        current == board[i * BOARD_SIZE + (j + 2)] &&
                        current == board[i * BOARD_SIZE + (j + 3)] &&
                        current == board[i * BOARD_SIZE + (j + 4)]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check vertical
                if (i <= BOARD_SIZE - 5) {
                    if (current == board[(i + 1) * BOARD_SIZE + j] &&
                        current == board[(i + 2) * BOARD_SIZE + j] &&
                        current == board[(i + 3) * BOARD_SIZE + j] &&
                        current == board[(i + 4) * BOARD_SIZE + j]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check diagonal (top-left to bottom-right)
                if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5) {
                    if (current == board[(i + 1) * BOARD_SIZE + (j + 1)] &&
                        current == board[(i + 2) * BOARD_SIZE + (j + 2)] &&
                        current == board[(i + 3) * BOARD_SIZE + (j + 3)] &&
                        current == board[(i + 4) * BOARD_SIZE + (j + 4)]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check diagonal (bottom-left to top-right)
                if (i >= 4 && j <= BOARD_SIZE - 5) {
                    if (current == board[(i - 1) * BOARD_SIZE + (j + 1)] &&
                        current == board[(i - 2) * BOARD_SIZE + (j + 2)] &&
                        current == board[(i - 3) * BOARD_SIZE + (j + 3)] &&
                        current == board[(i - 4) * BOARD_SIZE + (j + 4)]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }
            }
        }
    }

    // Check for draw (if all cells are filled)
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        if (board[i] == '.') {
            return GAME_NOT_FINISHED;  // The game is still ongoing
        }
    }

    return GAME_DRAWN;
}

GameState is_game_over(Game *g) {
    GameState result = evaluate_game_state(g);
    g->result = result;

    return result;
}

void print_game_board(Game *g) {
    char *board = (char *)g->board;

    printf("   ");
    // Print column headers
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i + 1 < 10)
            printf(" %d  ", i + 1);
        else
            printf("%d  ", i + 1);
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        // Print row headers
        printf("%2d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c ", board[i * BOARD_SIZE + j] == '\0'
                               ? '.'
                               : board[i * BOARD_SIZE + j]);
            if (j < BOARD_SIZE - 1) {
                printf("|");
            }
        }

        printf("\n");

        if (i < BOARD_SIZE - 1) {
            printf("   ");
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (j < BOARD_SIZE - 1) {
                    printf("---+");
                } else {
                    printf("---");
                }
            }
            printf("\n");
        }
    }
}

void end_game(Game *g) {
    free((char *)g->board);
    g->board = NULL;
}