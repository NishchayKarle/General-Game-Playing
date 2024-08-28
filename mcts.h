#ifndef _MCTS_H
#define _MCTS_H

#include "game.h"

#define MAX_ITERATIONS 1000000
#define MOVE_TIME_LIMIT 5
#define UCB1_CONSTANT 1.414
#define REWARD_DRAW 0.5

/**
 * Function to run the Monte Carlo Tree Search (MCTS) algorithm.
 *
 * @param game Pointer to the current game state.
 * @return Move* Pointer to the best move determined by MCTS.
 */
Move *monte_carlo_tree_search(Game *game);

#endif