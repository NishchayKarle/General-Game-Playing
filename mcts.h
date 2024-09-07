#ifndef _MCTS_H
#define _MCTS_H

#include "game.h"

#define MAX_ITERATIONS 10000000
#define UCB1_CONSTANT 1.414
#define REWARD_DRAW 0.5

Move* monte_carlo_tree_search(Game* g, Player p);
#endif