#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define ROWS 5
#define COLUMNS 8

typedef struct Move {
    int c;
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
    char *board = (char *)malloc(sizeof(char) * ROWS * COLUMNS);
    memset(board, '.', ROWS * COLUMNS);

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

    char *board = (char *)malloc(sizeof(char) * ROWS * COLUMNS);
    memcpy(board, g->board, ROWS * COLUMNS);
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

    copy->c = m->c;

    return copy;
}

bool is_valid_move(Game *g, Move *m) {
    char *board = (char *)g->board;

    if (board[m->c - 1] == '.') return true;

    return false;
}

void make_move(Game *g, Move *m) {
    char *board = (char *)g->board;
    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';

    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i * COLUMNS + m->c - 1] == '.') {
            board[i * COLUMNS + m->c - 1] = symbol;
            break;
        }
    }
}

Move *get_move(Game *g) {
    Move *m = (Move *)malloc(sizeof(Move));
    int column;

    while (1) {
        printf("%s - '%c' - Enter your move: ",
               g->player_turn == PLAYER1 ? g->player1 : g->player2,
               g->player_turn == PLAYER1 ? 'X' : 'O');

        printf(
            "%s, enter the column (1-%d) where you want to drop your "
            "disc: ",
            g->player_turn == PLAYER1 ? g->player1 : g->player2, COLUMNS);
        scanf("%d", &column);

        // Check if the input column is within the valid range
        if (column < 1 || column > COLUMNS) {
            printf("Invalid column. Please enter a number between 1 and %d.\n",
                   COLUMNS);
        } else {
            // Set the move column and break the loop if the input is valid
            m->c = column;
            break;
        }
    }

    return m;
}

void free_move(Move *m) {
    free(m);
}

Move **get_possible_moves(Game *g, int *num_moves) {
    char *board = (char *)g->board;
    Move **moves = (Move **)malloc(sizeof(Move *) * COLUMNS);
    *num_moves = 0;

    for (int i = 0; i < COLUMNS; i++) {
        if (board[i] == '.') {
            Move *m = (Move *)malloc(sizeof(Move));
            m->c = i + 1;
            moves[(*num_moves)++] = m;
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
    char *board = (char *)g->board;

    // Check horizontal (rows)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            if (board[i * COLUMNS + j] != '.' &&
                board[i * COLUMNS + j] == board[i * COLUMNS + j + 1] &&
                board[i * COLUMNS + j] == board[i * COLUMNS + j + 2] &&
                board[i * COLUMNS + j] == board[i * COLUMNS + j + 3]) {
                return board[i * COLUMNS + j] == 'X' ? GAME_WON_BY_PLAYER1
                                                     : GAME_WON_BY_PLAYER2;
            }
        }
    }

    // Check vertical (columns)
    for (int j = 0; j < COLUMNS; j++) {
        for (int i = 0; i <= ROWS - 4; i++) {
            if (board[i * COLUMNS + j] != '.' &&
                board[i * COLUMNS + j] == board[(i + 1) * COLUMNS + j] &&
                board[i * COLUMNS + j] == board[(i + 2) * COLUMNS + j] &&
                board[i * COLUMNS + j] == board[(i + 3) * COLUMNS + j]) {
                return board[i * COLUMNS + j] == 'X' ? GAME_WON_BY_PLAYER1
                                                     : GAME_WON_BY_PLAYER2;
            }
        }
    }

    // Check diagonal (bottom-left to top-right)
    for (int i = 0; i <= ROWS - 4; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            if (board[i * COLUMNS + j] != '.' &&
                board[i * COLUMNS + j] == board[(i + 1) * COLUMNS + j + 1] &&
                board[i * COLUMNS + j] == board[(i + 2) * COLUMNS + j + 2] &&
                board[i * COLUMNS + j] == board[(i + 3) * COLUMNS + j + 3]) {
                return board[i * COLUMNS + j] == 'X' ? GAME_WON_BY_PLAYER1
                                                     : GAME_WON_BY_PLAYER2;
            }
        }
    }

    // Check diagonal (top-left to bottom-right)
    for (int i = 3; i < ROWS; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            if (board[i * COLUMNS + j] != '.' &&
                board[i * COLUMNS + j] == board[(i - 1) * COLUMNS + j + 1] &&
                board[i * COLUMNS + j] == board[(i - 2) * COLUMNS + j + 2] &&
                board[i * COLUMNS + j] == board[(i - 3) * COLUMNS + j + 3]) {
                return board[i * COLUMNS + j] == 'X' ? GAME_WON_BY_PLAYER1
                                                     : GAME_WON_BY_PLAYER2;
            }
        }
    }

    // Check for Draw
    for (int j = 0; j < COLUMNS; j++) {
        if (board[j] == '.') {
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

    // Print column headers
    printf(" ");
    for (int i = 0; i < COLUMNS; i++) {
        if (i + 1 < 10)
            printf(" %d  ", i + 1);
        else
            printf(" %d ", i + 1);
    }
    printf("\n");

    // Print the board rows
    for (int i = 0; i < ROWS; i++) {
        printf("|");  // Start each row with a vertical bar
        for (int j = 0; j < COLUMNS; j++) {
            printf(" %c |", board[i * COLUMNS + j] == '\0'
                                ? ' '
                                : board[i * COLUMNS + j]);
        }
        printf("\n");
    }

    for (int j = 0; j < COLUMNS; j++) {
        printf("----");
    }
    printf("\n");
}

void print_move(Move *m) {
    printf("Col: %d\n", m->c);
}

void destroy_game(Game *g) {
    free((char *)g->board);
    g->board = NULL;
    free(g);
}
