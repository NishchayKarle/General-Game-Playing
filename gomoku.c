#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"

#define BOARD_SIZE 5

typedef struct Move {
    int r, c;
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
    char *board = (char *)malloc(sizeof(char) * BOARD_SIZE * BOARD_SIZE);
    memset(board, '.', BOARD_SIZE * BOARD_SIZE);

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

    char *board = (char *)malloc(sizeof(char) * BOARD_SIZE * BOARD_SIZE);
    memcpy(board, g->board, BOARD_SIZE * BOARD_SIZE);
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

bool make_move(Game *g, Move *m) {
    char *board = (char *)g->board;
    int index = (m->r - 1) * BOARD_SIZE + (m->c - 1);

    char symbol = g->player_turn == PLAYER1 ? 'X' : 'O';
    board[index] = symbol;

    return true;
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

void destroy_move(Move *m) {
    free(m);
}

Move **get_possible_moves(Game *g, int *num_moves) {
    char *board = (char *)g->board;
    Move **moves = (Move **)malloc(sizeof(Move *) * BOARD_SIZE * BOARD_SIZE);
    *num_moves = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i * BOARD_SIZE + j] == '.') {
                Move *m = (Move *)malloc(sizeof(Move));
                m->r = i + 1;
                m->c = j + 1;
                moves[(*num_moves)++] = m;
            }
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

GameState evaluate_game_state(Game *g) {
    char *board = (char *)g->board;

    // Check horizontal, vertical, and both diagonal directions for a win
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char current = board[i * BOARD_SIZE + j];
            if (current == 'X' || current == 'O') {
                // Check horizontal (ensure all are in the same row)
                if (j <= BOARD_SIZE -
                             5) {  // Ensure sequence doesn't go out of bounds
                    if (current == board[i * BOARD_SIZE + (j + 1)] &&
                        current == board[i * BOARD_SIZE + (j + 2)] &&
                        current == board[i * BOARD_SIZE + (j + 3)] &&
                        current == board[i * BOARD_SIZE + (j + 4)]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check vertical (ensure all are in the same column)
                if (i <= BOARD_SIZE -
                             5) {  // Ensure sequence doesn't go out of bounds
                    if (current == board[(i + 1) * BOARD_SIZE + j] &&
                        current == board[(i + 2) * BOARD_SIZE + j] &&
                        current == board[(i + 3) * BOARD_SIZE + j] &&
                        current == board[(i + 4) * BOARD_SIZE + j]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check diagonal (top-left to bottom-right)
                if (i <= BOARD_SIZE - 5 &&
                    j <= BOARD_SIZE - 5) {  // Ensure within bounds
                    if (current == board[(i + 1) * BOARD_SIZE + (j + 1)] &&
                        current == board[(i + 2) * BOARD_SIZE + (j + 2)] &&
                        current == board[(i + 3) * BOARD_SIZE + (j + 3)] &&
                        current == board[(i + 4) * BOARD_SIZE + (j + 4)]) {
                        return current == 'X' ? GAME_WON_BY_PLAYER1
                                              : GAME_WON_BY_PLAYER2;
                    }
                }

                // Check diagonal (bottom-left to top-right)
                if (i >= 4 && j <= BOARD_SIZE - 5) {  // Ensure within bounds
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

static double score_sequence(char *sequence, int length, char player,
                             double min_score, double max_score) {
    double score_range = max_score - min_score;

    // Adjust the weights according to how critical the sequence is
    double FOUR_IN_A_ROW = score_range * 0.8;   // Strong position
    double THREE_IN_A_ROW = score_range * 0.4;  // Moderate influence
    double TWO_IN_A_ROW = score_range * 0.1;    // Minimal influence
    double ONE_IN_A_ROW = score_range * 0.01;   // Very small influence

    int count_player = 0;
    int count_empty = 0;

    for (int i = 0; i < length; i++) {
        if (sequence[i] == player) count_player++;
        if (sequence[i] == '.') count_empty++;
    }

    if (count_player == 4 && count_empty == 1) return FOUR_IN_A_ROW;
    if (count_player == 3 && count_empty == 2) return THREE_IN_A_ROW;
    if (count_player == 2 && count_empty == 3) return TWO_IN_A_ROW;
    if (count_player == 1 && count_empty == 4) return ONE_IN_A_ROW;

    return 0;
};

// TODO: change to handle score based on current player (rn, it's always for
// 'O')
double board_score(Game *g, double min_score, double max_score) {
    GameState result = is_game_over(g);

    // If the game is over, return the appropriate score
    if (result == GAME_WON_BY_PLAYER1) {
        return min_score;  // Player1 ('X') won
    } else if (result == GAME_WON_BY_PLAYER2) {
        return max_score;  // Player2 ('O') won
    } else if (result == GAME_DRAWN) {
        return 0.0;  // A draw results in a neutral score
    }

    char *board = (char *)g->board;
    double score = 0.0;

    // Check all possible sequences of 5 on the board (horizontal, vertical,
    // diagonal)

    // Horizontal scoring
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j <= BOARD_SIZE - 5; j++) {
            char sequence[5] = {
                board[i * BOARD_SIZE + j], board[i * BOARD_SIZE + j + 1],
                board[i * BOARD_SIZE + j + 2], board[i * BOARD_SIZE + j + 3],
                board[i * BOARD_SIZE + j + 4]};
            score += score_sequence(sequence, 5, 'O', min_score, max_score);
            score -= score_sequence(sequence, 5, 'X', min_score, max_score);
        }
    }

    // Vertical scoring
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i <= BOARD_SIZE - 5; i++) {
            char sequence[5] = {board[i * BOARD_SIZE + j],
                                board[(i + 1) * BOARD_SIZE + j],
                                board[(i + 2) * BOARD_SIZE + j],
                                board[(i + 3) * BOARD_SIZE + j],
                                board[(i + 4) * BOARD_SIZE + j]};
            score += score_sequence(sequence, 5, 'O', min_score, max_score);
            score -= score_sequence(sequence, 5, 'X', min_score, max_score);
        }
    }

    // Diagonal scoring (bottom-left to top-right)
    for (int i = 0; i <= BOARD_SIZE - 5; i++) {
        for (int j = 0; j <= BOARD_SIZE - 5; j++) {
            char sequence[5] = {board[i * BOARD_SIZE + j],
                                board[(i + 1) * BOARD_SIZE + j + 1],
                                board[(i + 2) * BOARD_SIZE + j + 2],
                                board[(i + 3) * BOARD_SIZE + j + 3],
                                board[(i + 4) * BOARD_SIZE + j + 4]};
            score += score_sequence(sequence, 5, 'O', min_score, max_score);
            score -= score_sequence(sequence, 5, 'X', min_score, max_score);
        }
    }

    // Diagonal scoring (top-left to bottom-right)
    for (int i = 4; i < BOARD_SIZE; i++) {
        for (int j = 0; j <= BOARD_SIZE - 5; j++) {
            char sequence[5] = {board[i * BOARD_SIZE + j],
                                board[(i - 1) * BOARD_SIZE + j + 1],
                                board[(i - 2) * BOARD_SIZE + j + 2],
                                board[(i - 3) * BOARD_SIZE + j + 3],
                                board[(i - 4) * BOARD_SIZE + j + 4]};
            score += score_sequence(sequence, 5, 'O', min_score, max_score);
            score -= score_sequence(sequence, 5, 'X', min_score, max_score);
        }
    }

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

    printf("\n");
}

void print_move(Game *g, Move *m) {
    printf("%s - '%c' made the move: ",
           g->player_turn == PLAYER1 ? g->player1 : g->player2,
           g->player_turn == PLAYER1 ? 'X' : 'O');
    printf("(%d, %d)\n\n", m->r, m->c);
}

void print(char *str) {
    printf("%s\n", str);
}

void destroy_game(Game *g) {
    free((char *)g->board);
    g->board = NULL;
    free(g);
}
