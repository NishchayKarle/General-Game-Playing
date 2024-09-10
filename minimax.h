#ifndef _MINIMAX_H
#define _MINIMAX_H

#include "game.h"

#define MINIMAX_REWARD_WIN 10000
#define MINIMAX_REWARD_DRAW 1000
#define MINIMAX_REWARD_LOSE -10000

#define MAX_DEPTH 4

Move *minimax(Game *g);

#endif