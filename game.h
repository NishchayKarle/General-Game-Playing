#include <stdbool.h>

#ifndef _GAME_H
#define _GAME_H

typedef enum {
    PLAYER1, /** Represents Player 1. */
    PLAYER2, /** Represents Player 2. */
} Player;

typedef enum {
    GAME_NOT_FINISHED,   /** The game is still ongoing. */
    GAME_DRAWN,          /** The game ended in a draw. */
    GAME_WON_BY_PLAYER1, /** Player 1 has won the game. */
    GAME_WON_BY_PLAYER2  /** Player 2 has won the game. */
} GameState;

/**
 * Structure representing a move in the game.
 * This structure is intended to be defined by each specific game.
 * The fields of the Move structure will vary depending on the game's
 * requirements.
 */
typedef struct Move Move;

/**
 * Structure representing the state of a game.
 */
typedef struct Game {
    char player1[11];   /** The name of Player 1. */
    char player2[11];   /** The name of Player 2. */
    Player player_turn; /** Enum indicating the current player's turn. */
    void *board; /** Pointer to the game board (type depends on the game). */
    GameState result; /** The current result/state of the game. */
    void *extra1;     /** Extra field for any additional data (implementation
                         specific). */
    void *extra2;     /** Another extra field for any additional data
                         (implementation specific). */
} Game;

/**
 * Initializes the players in the game by setting their names.
 * This function prompts the users to enter the names of Player 1 and Player 2.
 *
 * @param g Pointer to the game structure to be initialized.
 * @param single_player AI mode if true, two-player mode if false.
 */
void setup_players(Game *g, bool single_player);

/**
 * Allocates and initializes the game state.
 * This function sets up the game board and any other necessary state for the
 * game.
 *
 * @param g Pointer to the game structure.
 */
void create_and_set_game_state(Game *g);

/**
 * Makes a deep copy of the game state.
 *
 * @param g Pointer to the game structure.
 * @return Game* Copy of the game state.
 */
Game *copy_game_state(Game *g);

/**
 * Makes a deep copy of the move.
 *
 * @param m Pointer to the move structure.
 * @return Move* Copy of the move.
 */
Move *copy_move(Move *m);

/**
 * Validates whether a given move is valid.
 * This function checks the validity of the move according to the game rules.
 * If the move is not valid, an appropriate error message is printed.
 *
 * @param g Pointer to the game structure.
 * @param m Pointer to the move structure to be validated.
 * @return true if the move is valid, false otherwise.
 */
bool is_valid_move(Game *g, Move *m);

/**
 * Executes a move in the game.
 * This function updates the game state based on the provided move.
 *
 * @param g Pointer to the game structure.
 * @param m Pointer to the move structure containing the move to be made.
 */
void make_move(Game *g, Move *m);

/**
 * Retrieves a move from the current player.
 * This function prompts the current player to enter their move.
 *
 * @param g Pointer to the game structure.
 * @return Pointer to the move structure containing the player's move.
 */
Move *get_move(Game *g);

/**
 * Frees the memory allocated for a move.
 *
 * @param m Pointer to the move structure to be freed.
 */
void free_move(Move *m);

/**
 * Generates a list of possible moves for the current game state.
 *
 * @param g Pointer to the game structure.
 * @param num_moves Pointer to an integer to store the number of possible moves.
 * @return Move** Array of possible moves.
 */
Move **get_possible_moves(Game *g, int *num_moves);

/**
 * Evaluates the current state of the game to determine if it is over.
 * This function checks if the game has been won, drawn, or is still ongoing.
 * Update the result in the game structure to indicate current game state.
 *
 * @param g Pointer to the game structure.
 * @return The current state of the game.
 */
GameState is_game_over(Game *g);

/**
 * Prints the current state of the game.
 * This function outputs the game board and any relevant information to the
 * console.
 *
 * @param g Pointer to the game structure.
 */
void print_game_board(Game *g);

/**
 * Prints the details of a move.
 *
 * @param m Pointer to the move structure to be printed.
 */
void print_move(Move *m);

/**
 * Cleans up and frees any resources allocated for the game.
 * This function should be called at the end of the game to release memory and
 * other resources.
 *
 * @param g Pointer to the game structure.
 */
void end_game(Game *g);

#endif