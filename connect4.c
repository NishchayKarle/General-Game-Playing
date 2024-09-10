#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"

#define ROWS 5
#define COLUMNS 8

typedef struct Move {
    int c;
} Move;

void init() {
    srand(time(NULL));
}

void end() {
}

void setup_players(Game *g, bool single_player) {
    printf("Player1 - 'X': Enter your name (up to 10 characters): ");
    if (!scanf("%10s", g->player1)) {
        perror("Failed to read player1 name");
        exit(EXIT_FAILURE);
    }
    getchar();

    if (single_player) {
        strcpy(g->player2, "AI");
    } else {
        printf("Player2 - 'O': Enter your name (up to 10 characters): ");
        if (!scanf("%10s", g->player2)) {
            perror("Failed to read player2 name");
            exit(EXIT_FAILURE);
        }
        getchar();
    }
}

void pick_starting_player(Game *g) {
    g->player_turn = rand() % 2 == 0 ? PLAYER1 : PLAYER2;
}

void init_game_state(Game *g) {
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

bool make_move(Game *g, Move *m) {
    char *board = (char *)g->board;
    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';

    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i * COLUMNS + m->c - 1] == '.') {
            board[i * COLUMNS + m->c - 1] = symbol;
            break;
        }
    }

    return true;
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

void destroy_move(Move *m) {
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
        destroy_move(moves[i]);
    }
    free(moves);
}

static GameState evaluate_game_state(Game *g) {
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

static double score_sequence(char *sequence, int length, char player,
                             double min_score, double max_score) {
    double score_range = max_score - min_score;

    // Winning opportunity is very important
    double THREE_IN_A_ROW = score_range * 0.5;
    // Two-in-a-row is important, but less so
    double TWO_IN_A_ROW = score_range * 0.1;
    // Single pieces should have minimal influence
    double ONE_IN_A_ROW = score_range * 0.01;

    int count_player = 0;
    int count_empty = 0;

    for (int i = 0; i < length; i++) {
        if (sequence[i] == player) count_player++;
        if (sequence[i] == '.') count_empty++;
    }

    if (count_player == 3 && count_empty == 1) return THREE_IN_A_ROW;
    if (count_player == 2 && count_empty == 2) return TWO_IN_A_ROW;
    if (count_player == 1 && count_empty == 3) return ONE_IN_A_ROW;

    return 0;
};

// TODO: change to handle score based on current player (rn, it's always for 'O')
double board_score(Game *g, double min_score, double max_score) {
    GameState result = is_game_over(g);

    // If the game is over, return the maximum or minimum score accordingly
    if (result == GAME_WON_BY_PLAYER1) {
        return min_score;  // Player1 ('X') won, which is bad for Player2 ('O')
    } else if (result == GAME_WON_BY_PLAYER2) {
        return max_score;  // Player2 ('O') won, which is good for Player2
    } else if (result == GAME_DRAWN) {
        return 0.0;  // A draw results in a neutral score
    }

    char *board = (char *)g->board;
    double score = 0.0;

    // Horizontal scoring
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            char sequence[4] = {
                board[i * COLUMNS + j], board[i * COLUMNS + j + 1],
                board[i * COLUMNS + j + 2], board[i * COLUMNS + j + 3]};
            score += score_sequence(sequence, 4, 'O', min_score, max_score);
            score -= score_sequence(sequence, 4, 'X', min_score, max_score);
        }
    }

    // Vertical scoring
    for (int j = 0; j < COLUMNS; j++) {
        for (int i = 0; i <= ROWS - 4; i++) {
            char sequence[4] = {
                board[i * COLUMNS + j], board[(i + 1) * COLUMNS + j],
                board[(i + 2) * COLUMNS + j], board[(i + 3) * COLUMNS + j]};
            score += score_sequence(sequence, 4, 'O', min_score, max_score);
            score -= score_sequence(sequence, 4, 'X', min_score, max_score);
        }
    }

    // Diagonal scoring (bottom-left to top-right)
    for (int i = 0; i <= ROWS - 4; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            char sequence[4] = {board[i * COLUMNS + j],
                                board[(i + 1) * COLUMNS + j + 1],
                                board[(i + 2) * COLUMNS + j + 2],
                                board[(i + 3) * COLUMNS + j + 3]};
            score += score_sequence(sequence, 4, 'O', min_score, max_score);
            score -= score_sequence(sequence, 4, 'X', min_score, max_score);
        }
    }

    // Diagonal scoring (top-left to bottom-right)
    for (int i = 3; i < ROWS; i++) {
        for (int j = 0; j <= COLUMNS - 4; j++) {
            char sequence[4] = {board[i * COLUMNS + j],
                                board[(i - 1) * COLUMNS + j + 1],
                                board[(i - 2) * COLUMNS + j + 2],
                                board[(i - 3) * COLUMNS + j + 3]};
            score += score_sequence(sequence, 4, 'O', min_score, max_score);
            score -= score_sequence(sequence, 4, 'X', min_score, max_score);
        }
    }

    // Return the computed score for the board
    return score;
}

void display_result(Game *g) {
    switch (g->result) {
        case GAME_DRAWN:
            printf("GAME DRAWN\n");
            break;

        case GAME_WON_BY_PLAYER1:
            printf("PLAYER 1 - %s WON\n", g->player1);
            break;

        case GAME_WON_BY_PLAYER2:
            printf("PLAYER 2 - %s WON\n", g->player2);
            break;

        default:
            printf("Error in game result\n");
            break;
    }
}

void print_game_board(Game *g) {
    char *board = (char *)g->board;

    printf("\n");

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

void print_move(Game *g, Move *m) {
    printf("%s - '%c' made the move: ",
           g->player_turn == PLAYER1 ? g->player1 : g->player2,
           g->player_turn == PLAYER1 ? 'X' : 'O');
    printf("Col: %d\n\n", m->c);
}

void print(char *str) {
    printf("%s\n", str);
}

void destroy_game(Game *g) {
    free((char *)g->board);
    g->board = NULL;
    free(g);
}
