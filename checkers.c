#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define PLAYER1_NAME_LINE 0
#define PLAYER2_NAME_LINE 1
#define BOARD_SIZE 8
#define BOX_WIDTH 7
#define BOX_HEIGHT 3
#define BOARD_COL_OFFSET 0
#define MOVE_PROMPT_LINE (BOARD_SIZE * BOX_HEIGHT + 4)
#define INPUT_ERROR_LINE (BOARD_SIZE * BOX_HEIGHT + 2)
#define PRINT_MOVE_LINE (BOARD_SIZE * BOX_HEIGHT + 2)
#define WHITE_COLOR 1
#define BLACK_COLOR 2
#define RED_PIECE_COLOR 3
#define WHITE_PIECE_COLOR 4
#define PAWN "●"
#define KING_SYMBOL "K"
#define PAWN_COUNT 12

typedef struct Pawn {
    int row;
    int col;
    Player player;
    bool is_king;
    bool is_captured;
} Pawn;

typedef struct Move {
    int from_row;
    int from_col;
    int to_row;
    int to_col;
} Move;

void setup_players(Game *g, bool single_player) {
    char input[11];

    clear();
    mvprintw(PLAYER1_NAME_LINE, BOARD_COL_OFFSET,
             "Player1 - 'Red': Enter your name (up to 10 characters): ");
    refresh();
    getstr(input);
    strncpy(g->player1, input, 10);
    g->player1[10] = '\0';

    if (single_player) {
        strcpy(g->player2, "AI");
    } else {
        mvprintw(PLAYER2_NAME_LINE, BOARD_COL_OFFSET,
                 "Player2 - 'White': Enter your name (up to 10 characters): ");
        refresh();
        getstr(input);
        strncpy(g->player2, input, 10);
        g->player2[10] = '\0';
    }

    clear();
    refresh();
}

void pick_starting_player(Game *g) {
    g->player_turn = PLAYER1;
}

static void init_pawns(Pawn *p1, Pawn *p2) {
    // Initialize PLAYER1 pawns
    p1[0] = (Pawn){5, 0, PLAYER1, false, false};
    p1[1] = (Pawn){5, 2, PLAYER1, false, false};
    p1[2] = (Pawn){5, 4, PLAYER1, false, false};
    p1[3] = (Pawn){5, 6, PLAYER1, false, false};
    p1[4] = (Pawn){6, 1, PLAYER1, false, false};
    p1[5] = (Pawn){6, 3, PLAYER1, false, false};
    p1[6] = (Pawn){6, 5, PLAYER1, false, false};
    p1[7] = (Pawn){6, 7, PLAYER1, false, false};
    p1[8] = (Pawn){7, 0, PLAYER1, false, false};
    p1[9] = (Pawn){7, 2, PLAYER1, false, false};
    p1[10] = (Pawn){7, 4, PLAYER1, false, false};
    p1[11] = (Pawn){7, 6, PLAYER1, false, false};

    // Initialize PLAYER2 pawns
    p2[0] = (Pawn){0, 1, PLAYER2, false, false};
    p2[1] = (Pawn){0, 3, PLAYER2, false, false};
    p2[2] = (Pawn){0, 5, PLAYER2, false, false};
    p2[3] = (Pawn){0, 7, PLAYER2, false, false};
    p2[4] = (Pawn){1, 0, PLAYER2, false, false};
    p2[5] = (Pawn){1, 2, PLAYER2, false, false};
    p2[6] = (Pawn){1, 4, PLAYER2, false, false};
    p2[7] = (Pawn){1, 6, PLAYER2, false, false};
    p2[8] = (Pawn){2, 1, PLAYER2, false, false};
    p2[9] = (Pawn){2, 3, PLAYER2, false, false};
    p2[10] = (Pawn){2, 5, PLAYER2, false, false};
    p2[11] = (Pawn){2, 7, PLAYER2, false, false};
}

static void draw_board() {
    int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            int x = col * BOX_WIDTH + 1;
            int y = row * BOX_HEIGHT + 1;

            if ((row + col) % 2 == 0) {
                attron(COLOR_PAIR(WHITE_COLOR));
            } else {
                attron(COLOR_PAIR(BLACK_COLOR));
            }

            for (int i = 0; i < BOX_HEIGHT; i++) {
                // 7 characters wide
                mvprintw(y + i, x, "       ");
            }

            attroff(COLOR_PAIR(WHITE_COLOR));
            attroff(COLOR_PAIR(BLACK_COLOR));
        }
    }
}

static void draw_pieces(Pawn *pawns) {
    for (int i = 0; i < PAWN_COUNT; i++) {
        Pawn *pawn = &pawns[i];

        if (pawn->is_captured) {
            continue;
        }

        int screen_row = pawn->row * BOX_HEIGHT + 1 + 1;
        int screen_col = pawn->col * BOX_WIDTH + 1 + 3;

        if (pawn->player == PLAYER1) {
            attron(COLOR_PAIR(RED_PIECE_COLOR));
        } else if (pawn->player == PLAYER2) {
            attron(COLOR_PAIR(WHITE_PIECE_COLOR));
        }

        if (pawn->is_king) {
            attron(A_BOLD);
            mvprintw(screen_row, screen_col, KING_SYMBOL);
            attroff(A_BOLD);
        } else {
            mvprintw(screen_row, screen_col, PAWN);
        }

        attroff(COLOR_PAIR(RED_PIECE_COLOR));
        attroff(COLOR_PAIR(WHITE_PIECE_COLOR));
    }
}

static void draw_border() {
    int board_width = BOARD_SIZE * BOX_WIDTH;
    int board_height = BOARD_SIZE * BOX_HEIGHT;

    // Draw a border around the checkers board
    mvhline(0, 1, '-', board_width + 1);                 // Top border
    mvhline(board_height + 1, 1, '-', board_width + 1);  // Bottom border
    mvvline(1, 0, '|', board_height);                    // Left border
    mvvline(1, board_width + 1, '|', board_height);      // Right border

    mvaddch(0, 0, '+');
    mvaddch(0, board_width + 1, '+');
    mvaddch(board_height + 1, 0, '+');
    mvaddch(board_height + 1, board_width + 1, '+');

    // Draw column numbers (top)
    for (int col = 0; col < BOARD_SIZE; col++) {
        mvprintw(0, col * BOX_WIDTH + 1 + 3, "%d", col);
    }

    // Draw row numbers (left)
    for (int row = 0; row < BOARD_SIZE; row++) {
        mvprintw(row * BOX_HEIGHT + 1 + 1, 0, "%d", row);
    }
}

void init() {
    reset_shell_mode();

    setlocale(LC_ALL, "");

    // Initialize ncurses
    initscr();
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Error: Terminal does not support colors.\n");
        exit(1);
    }

    start_color();
    use_default_colors();

    // Set cursor to be invisible
    curs_set(0);

    // Initialize color pairs for the board and pieces
    init_pair(WHITE_COLOR, COLOR_WHITE, COLOR_WHITE);
    init_pair(BLACK_COLOR, COLOR_BLACK, COLOR_BLACK);
    init_pair(RED_PIECE_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(WHITE_PIECE_COLOR, COLOR_WHITE, COLOR_BLACK);
}

void end() {
    endwin();
}

void init_game_state(Game *g) {
    Pawn *pawns = (Pawn *)malloc(sizeof(Pawn) * PAWN_COUNT * 2);
    init_pawns(pawns, pawns + PAWN_COUNT);

    g->board = (void *)pawns;
    g->player_turn = PLAYER1;
    g->result = GAME_NOT_FINISHED;
    g->extra1 = NULL;
    g->extra2 = NULL;
}

void display_result(Game *g) {
    clear();
    print_game_board(g);

    switch (g->result) {
        case GAME_DRAWN:
            mvprintw(BOARD_SIZE * BOX_HEIGHT + 3, 0, "GAME DRAWN");
            break;
        case GAME_WON_BY_PLAYER1:
            mvprintw(BOARD_SIZE * BOX_HEIGHT + 3, 0, "%s - PLAYER 1 WON",
                     g->player1);
            break;
        case GAME_WON_BY_PLAYER2:
            mvprintw(BOARD_SIZE * BOX_HEIGHT + 3, 0, "%s - PLAYER 2 WON",
                     g->player2);
            break;
        default:
            mvprintw(BOARD_SIZE * BOX_HEIGHT + 3, 0, "Error in game result");
            break;
    }

    mvprintw(BOARD_SIZE * BOX_HEIGHT + 5, 0, "Press any key to exit...");
    refresh();
    getch();
}

static bool is_valid_move_private(Game *g, Move *m, char *error_message) {
    if (m->from_row < 0 || m->from_row >= BOARD_SIZE || m->from_col < 0 ||
        m->from_col >= BOARD_SIZE || m->to_row < 0 || m->to_row >= BOARD_SIZE ||
        m->to_col < 0 || m->to_col >= BOARD_SIZE) {
        if (error_message != NULL)
            strcpy(error_message, "Invalid move coordinates!");
        return false;
    }

    Pawn *pawns = (Pawn *)g->board;
    Pawn *player_pawns = pawns + (g->player_turn == PLAYER1 ? 0 : PAWN_COUNT);
    Pawn *opponent_pawns = pawns + (g->player_turn == PLAYER1 ? PAWN_COUNT : 0);

    Pawn *selected_pawn = NULL;

    // Check if the starting position has a valid pawn
    for (int i = 0; i < PAWN_COUNT; i++)
        if (player_pawns[i].row == m->from_row &&
            player_pawns[i].col == m->from_col &&
            !player_pawns[i].is_captured) {
            selected_pawn = &player_pawns[i];
            break;
        }

    if (!selected_pawn) {
        if (error_message != NULL)
            strcpy(error_message, "Invalid start position!");
        return false;
    }

    // Check if the destination square is occupied by any player or is invalid
    // (white square)
    for (int i = 0; i < PAWN_COUNT; i++)
        if ((player_pawns[i].row == m->to_row &&
             player_pawns[i].col == m->to_col &&
             !player_pawns[i].is_captured) ||
            (!opponent_pawns[i].is_captured &&
             opponent_pawns[i].row == m->to_row &&
             opponent_pawns[i].col == m->to_col) ||
            (m->to_row + m->to_col) % 2 == 0) {
            if (error_message != NULL)
                strcpy(error_message, "Invalid destination!");
            return false;
        }

    // King Move Logic
    int row_diff = abs(m->to_row - m->from_row);
    int col_diff = abs(m->to_col - m->from_col);

    if (selected_pawn->is_king) {
        if (row_diff == 1 && col_diff == 1)
            // Valid move for king
            return true;

        if (row_diff == 2 && col_diff == 2) {
            // Check for an opponent pawn to capture
            int mid_row = (m->to_row + m->from_row) / 2;
            int mid_col = (m->to_col + m->from_col) / 2;

            for (int i = 0; i < PAWN_COUNT; i++)
                if (!opponent_pawns[i].is_captured &&
                    opponent_pawns[i].row == mid_row &&
                    opponent_pawns[i].col == mid_col)
                    return true;
        }
    } else {
        // Regular pawn movement
        bool is_forward = (g->player_turn == PLAYER1)
                              ? (m->to_row < m->from_row)
                              : (m->to_row > m->from_row);

        // Check if the move is a valid diagonal and forward move
        if (is_forward && row_diff == 1 && col_diff == 1)
            // Valid move for regular pawn
            return true;

        // Check if it's a capture move (2 squares diagonally)
        if (is_forward && row_diff == 2 && col_diff == 2) {
            int mid_row = (m->to_row + m->from_row) / 2;
            int mid_col = (m->to_col + m->from_col) / 2;

            // Check if there's an opponent pawn to capture
            for (int i = 0; i < PAWN_COUNT; i++)
                if (!opponent_pawns[i].is_captured &&
                    opponent_pawns[i].row == mid_row &&
                    opponent_pawns[i].col == mid_col)
                    // Valid capture move
                    return true;
        }
    }

    if (error_message != NULL) strcpy(error_message, "Invalid move!");
    return false;  // Invalid move case
}

bool is_valid_move(Game *g, Move *m) {
    char error_message[50];
    memset(error_message, 0, sizeof(error_message));

    bool is_valid = is_valid_move_private(g, m, error_message);

    if (!is_valid) {
        clear();
        print_game_board(g);
        mvprintw(INPUT_ERROR_LINE, BOARD_COL_OFFSET, "%s", error_message);
        mvprintw(INPUT_ERROR_LINE + 1, BOARD_COL_OFFSET,
                 "Press any key to re-enter...");
        refresh();
        getch();
    }

    return is_valid;
}

bool make_move(Game *g, Move *m) {
    Pawn *pawns = (Pawn *)g->board;
    Pawn *player_pawns = pawns + (g->player_turn == PLAYER1 ? 0 : PAWN_COUNT);
    Pawn *opponent_pawns = pawns + (g->player_turn == PLAYER1 ? PAWN_COUNT : 0);

    Pawn *selected_pawn = NULL;

    // Find the selected pawn
    for (int i = 0; i < PAWN_COUNT; i++) {
        if (player_pawns[i].is_captured) continue;

        if (player_pawns[i].row == m->from_row &&
            player_pawns[i].col == m->from_col) {
            selected_pawn = &player_pawns[i];
            break;
        }
    }

    // Move the pawn to the destination
    selected_pawn->row = m->to_row;
    selected_pawn->col = m->to_col;

    // Check if the pawn should be promoted to a king
    if ((selected_pawn->player == PLAYER1 && selected_pawn->row == 0) ||
        (selected_pawn->player == PLAYER2 &&
         selected_pawn->row == BOARD_SIZE - 1)) {
        selected_pawn->is_king = true;
    }

    // Check if the move was a capture
    if (abs(m->to_row - m->from_row) == 2) {
        // Capture the opponent's pawn
        int opponent_row = (m->to_row + m->from_row) / 2;
        int opponent_col = (m->to_col + m->from_col) / 2;
        for (int i = 0; i < PAWN_COUNT; i++) {
            if (!opponent_pawns[i].is_captured &&
                opponent_pawns[i].row == opponent_row &&
                opponent_pawns[i].col == opponent_col) {
                opponent_pawns[i].is_captured = true;
                opponent_pawns[i].row = -1;
                opponent_pawns[i].col = -1;
                break;
            }
        }

        // Check if there is another valid capture move for the current player
        // with the same pawn
        // Check adjacent squares for opponent pawns and the final destination
        // to be empty and within bounds
        for (int row = -2; row <= 2; row += 4) {
            for (int col = -2; col <= 2; col += 4) {
                if (row == 0 || col == 0) continue;
                // if the pawn is king, check all 4 directions
                // else check only the forward direction
                if (!selected_pawn->is_king &&
                    ((selected_pawn->player == PLAYER1 && row > 0) ||
                     (selected_pawn->player == PLAYER2 && row < 0)))
                    continue;

                if (m->to_row + row < 0 || m->to_row + row >= BOARD_SIZE ||
                    m->to_col + col < 0 || m->to_col + col >= BOARD_SIZE)
                    continue;

                Move next_move = {
                    .from_row = m->to_row,
                    .from_col = m->to_col,
                    .to_row = m->to_row + row,
                    .to_col = m->to_col + col,
                };

                if (is_valid_move_private(g, &next_move, NULL)) {
                    return false;
                }
            }
        }
    }

    return true;
}

Move *get_move(Game *g) {
    Move *m = (Move *)malloc(sizeof(Move));
    char input[10];

    while (1) {
        clear();
        print_game_board(g);

        // Display prompt for player move
        mvprintw(MOVE_PROMPT_LINE, BOARD_COL_OFFSET, "%s - '%s' - Move:",
                 g->player_turn == PLAYER1 ? g->player1 : g->player2,
                 g->player_turn == PLAYER1 ? "Red" : "White");

        // Ask for starting location
        mvprintw(MOVE_PROMPT_LINE + 1, BOARD_COL_OFFSET + 4,
                 "Enter starting location (row,column): ");
        refresh();

        // Capture input for starting location
        wgetnstr(stdscr, input, sizeof(input));
        input[strcspn(input, "\n")] = '\0';  // Remove any trailing newline

        // Parse and validate the starting location
        if (sscanf(input, "%d,%d", &m->from_row, &m->from_col) == 2) {
            if (m->from_row >= 0 && m->from_row < BOARD_SIZE &&
                m->from_col >= 0 && m->from_col < BOARD_SIZE) {
                // Ask for destination location
                mvprintw(MOVE_PROMPT_LINE + 2, BOARD_COL_OFFSET + 4,
                         "Enter destination location (row,column): ");
                refresh();

                // Capture input for destination location
                wgetnstr(stdscr, input, sizeof(input));
                input[strcspn(input, "\n")] =
                    '\0';  // Remove any trailing newline

                // Parse and validate the destination location
                if (sscanf(input, "%d,%d", &m->to_row, &m->to_col) == 2 &&
                    m->to_row >= 0 && m->to_row < BOARD_SIZE &&
                    m->to_col >= 0 && m->to_col < BOARD_SIZE) {
                    return m;  // Valid move, return it
                } else {
                    mvprintw(INPUT_ERROR_LINE, BOARD_COL_OFFSET,
                             "Invalid destination! Try again.");
                }
            } else {
                mvprintw(INPUT_ERROR_LINE, BOARD_COL_OFFSET,
                         "Invalid start location! Try again.");
            }
        } else {
            mvprintw(INPUT_ERROR_LINE, BOARD_COL_OFFSET,
                     "Invalid format! Use 'row,column'.");
        }

        // Wait for user to acknowledge the error
        mvprintw(INPUT_ERROR_LINE + 1, BOARD_COL_OFFSET,
                 "Press any key to re-enter...");
        refresh();
        clrtoeol();
        getch();  // Wait for key press
    }
}

void print_game_board(Game *g) {
    clear();
    draw_border();
    draw_board();
    draw_pieces((Pawn *)g->board);
    draw_pieces((Pawn *)g->board + PAWN_COUNT);
    refresh();
}

void destroy_move(Move *m) {
    free(m);
}

void destroy_game(Game *g) {
    free(g->board);
}

void print_move(Game *g, Move *m) {
    mvprintw(PRINT_MOVE_LINE, BOARD_COL_OFFSET,
             "Player %d - %s made the move: (%d, %d) to (%d, %d)\n",
             g->player_turn == PLAYER1 ? 1 : 2,
             g->player_turn == PLAYER1 ? g->player1 : g->player2, m->from_row,
             m->from_col, m->to_row, m->to_col);
    refresh();
}

void print(char *msg) {
    mvprintw(INPUT_ERROR_LINE, BOARD_COL_OFFSET, "%s", msg);
    refresh();
}

GameState is_game_over(Game *g) {
    Player turn = g->player_turn;

    Pawn *pawns = (Pawn *)g->board;
    Pawn *player_pawns = pawns + (turn == PLAYER1 ? 0 : PAWN_COUNT);

    // Check if the current player has any valid moves
    for (int i = 0; i < PAWN_COUNT; i++) {
        if (player_pawns[i].is_captured) continue;

        // check if the pawn can move diagonally forward
        Move m = {
            .from_row = player_pawns[i].row,
            .from_col = player_pawns[i].col,
            .to_row = turn == PLAYER1 ? player_pawns[i].row - 1
                                      : player_pawns[i].row + 1,
            .to_col = player_pawns[i].col - 1,
        };

        if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
            m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
            return GAME_NOT_FINISHED;
        }

        m.to_col = player_pawns[i].col + 1;

        if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
            m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
            return GAME_NOT_FINISHED;
        }

        // check if the pawn can move diagonally forward with a capture
        m.to_row =
            turn == PLAYER1 ? player_pawns[i].row - 2 : player_pawns[i].row + 2;
        m.to_col = player_pawns[i].col - 2;
        if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
            m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
            return GAME_NOT_FINISHED;
        }

        m.to_col = player_pawns[i].col + 2;
        if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
            m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
            return GAME_NOT_FINISHED;
        }

        // If pawn is a king, check if it can move diagonally backward
        if (player_pawns[i].is_king) {
            m.to_row = turn == PLAYER1 ? player_pawns[i].row + 1
                                       : player_pawns[i].row - 1;
            m.to_col = player_pawns[i].col - 1;
            if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
                m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
                return GAME_NOT_FINISHED;
            }

            m.to_col = player_pawns[i].col + 1;
            if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
                m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
                return GAME_NOT_FINISHED;
            }

            // check if the pawn can move diagonally backward with a capture
            m.to_row = turn == PLAYER1 ? player_pawns[i].row + 2
                                       : player_pawns[i].row - 2;
            m.to_col = player_pawns[i].col - 2;
            if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
                m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
                return GAME_NOT_FINISHED;
            }

            m.to_col = player_pawns[i].col + 2;
            if (m.to_row >= 0 && m.to_row < BOARD_SIZE && m.to_col >= 0 &&
                m.to_col < BOARD_SIZE && is_valid_move_private(g, &m, NULL)) {
                return GAME_NOT_FINISHED;
            }
        }
    }

    // Current player has no valid moves or captures
    // Game is won by the other player
    return turn == PLAYER1 ? GAME_WON_BY_PLAYER2 : GAME_WON_BY_PLAYER1;
}
