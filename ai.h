#include "game.h"

#define MOVE_TIME_LIMIT 5

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
 * Generates a list of possible moves for the current game state.
 *
 * @param g Pointer to the game structure.
 * @param num_moves Pointer to an integer to store the number of possible moves.
 * @return Move** Array of possible moves.
 */
Move **get_possible_moves(Game *g, int *num_moves);

/**
 * This function is called to free the memory(if) allocated for an array of
 * moves.
 *
 * @param moves Array of moves to be freed.
 * @param num_moves Number of moves in the array.
 */
void destroy_list_of_moves(Move **moves, int num_moves);

/**
 * Function for AI to make a move in the game.
 *
 * @param g Pointer to the game structure.
 * @return Move* Pointer to the best move determined by the AI.
 */
Move *ai_make_move(Game *g);

/**
 * Function to get the board score for the given player based on strength of
 * winning from current position.
 *
 * @param g Pointer to the game structure.
 * @return double
 */
double board_score(Game *g, double min_score, double max_score);