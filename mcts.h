#ifndef _MCTS_H
#define _MCTS_H

#include "game.h"

/**
 * Function to run the Monte Carlo Tree Search (MCTS) algorithm.
 *
 * @param game Pointer to the current game state.
 * @return Move* Pointer to the best move determined by MCTS.
 */
Move *monte_carlo_tree_search(Game *game);

#endif