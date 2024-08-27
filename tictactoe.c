#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

typedef struct Move {
    int r, c;
} Move;

void setup_players(Game *g, bool single_player) {
    printf("Player1 - 'X': Enter your name (up to 10 characters): ");
    scanf("%10s", g->player1);
    getchar();

    if (single_player) {
        strcpy(g->player2, "AI");
    } else {
        printf("Player2 - 'O': Enter your name (up to 10 characters): ");
        scanf("%10s", g->player2);
        getchar();
    }
}

void create_and_set_game_state(Game *g) {
    char *board = (char *)malloc(sizeof(char) * 10);
    memset(board, '\0', 10);

    g->board = (void *)board;
    g->result = GAME_NOT_FINISHED;
}

Game *copy_game_state(Game *g) {
    Game *copy = (Game *)malloc(sizeof(Game));
    if (copy == NULL) {
        perror("Failed to allocate memory for the game copy");
        exit(EXIT_FAILURE);
    }

    strcpy(copy->player1, g->player1);
    strcpy(copy->player2, g->player2);
    copy->player_turn = g->player_turn;
    copy->result = g->result;

    char *board = (char *)malloc(sizeof(char) * 10);
    memcpy(board, g->board, 10);
    copy->board = (void *)board;

    return copy;
}

Move *copy_move(Move *m) {
    if (!m) return NULL;

    Move *copy = (Move *)malloc(sizeof(Move));
    if (copy == NULL) {
        perror("Failed to allocate memory for the move copy");
        exit(EXIT_FAILURE);
    }

    copy->r = m->r;
    copy->c = m->c;

    return copy;
}

bool is_valid_move(Game *g, Move *m) {
    if (m->r <= 0 || m->r > 3 || m->c <= 0 || m->c > 3) {
        printf(
            "Invalid move! The move is out of bounds. Please enter row and "
            "column values between 1 and 3.\n");
        return false;
    }

    char *board = (char *)g->board;

    int index = (m->r - 1) * 3 + (m->c - 1);
    if (board[index] != '\0') {
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
    int index = (m->r - 1) * 3 + (m->c - 1);

    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';
    board[index] = symbol;
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

void free_move(Move *m) {
    free(m);
}

Move **get_possible_moves(Game *g, int *num_moves) {
    char *board = (char *)g->board;
    *num_moves = 0;

    // First pass: count the number of empty cells
    for (int i = 0; i < 9; i++) {
        if (board[i] == '\0') {
            (*num_moves)++;
        }
    }

    // If no moves are possible, return NULL
    if (*num_moves == 0) {
        return NULL;
    }

    // Allocate memory for the array of moves
    Move **moves = (Move **)malloc(sizeof(Move *) * (*num_moves));
    if (moves == NULL) {
        perror("Failed to allocate memory for possible moves");
        exit(EXIT_FAILURE);
    }

    // Second pass: populate the array with possible moves
    int index = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] == '\0') {
            Move *m = (Move *)malloc(sizeof(Move));
            if (m == NULL) {
                perror("Failed to allocate memory for a move");
                // Free any previously allocated moves before exiting
                for (int j = 0; j < index; j++) {
                    free(moves[j]);
                }
                free(moves);
                exit(EXIT_FAILURE);
            }
            m->r = i / 3 + 1;  // Convert index to row
            m->c = i % 3 + 1;  // Convert index to column
            moves[index++] = m;
        }
    }

    return moves;
}

void destroy_list_of_moves(Move **moves, int num_moves) {
    for (int i = 0; i < num_moves; i++) {
        free_move(moves[i]);
    }
    free(moves);
}

GameState evaluate_game_state(Game *g) {
    char *board = g->board;

    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i * 3] != '\0' && board[i * 3] == board[i * 3 + 1] &&
            board[i * 3 + 1] == board[i * 3 + 2]) {
            return board[i * 3] == 'X' ? GAME_WON_BY_PLAYER1
                                       : GAME_WON_BY_PLAYER2;
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] != '\0' && board[i] == board[i + 3] &&
            board[i + 3] == board[i + 6]) {
            return board[i] == 'X' ? GAME_WON_BY_PLAYER1 : GAME_WON_BY_PLAYER2;
        }
    }

    // Check diagonals
    if (board[0] != '\0' && board[0] == board[4] && board[4] == board[8]) {
        return board[0] == 'X' ? GAME_WON_BY_PLAYER1 : GAME_WON_BY_PLAYER2;
    }

    if (board[2] != '\0' && board[2] == board[4] && board[4] == board[6]) {
        return board[2] == 'X' ? GAME_WON_BY_PLAYER1 : GAME_WON_BY_PLAYER2;
    }

    // Check for Draw
    for (int i = 0; i < 9; i++) {
        if (board[i] == '\0') {
            return GAME_NOT_FINISHED;
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

void print_move(Move *m) {
    printf("(%d, %d)\n", m->r, m->c);
}

void destroy_game(Game *g) {
    free((char *)g->board);
}