#include <stdbool.h>

#ifndef _GAME_
#define _GAME_

#define PLAYER1 0
#define PLAYER2 1

#define GAME_DRAWN 0
#define GAME_WON_BY_PLAYER1 -1
#define GAME_WON_BY_PLAYER2 1

typedef struct Move Move;

typedef struct Game {
    char player1[11];
    char player2[11];
    bool player_turn;
    void *game_state;
    void *extra;
} Game;

void setup_players(Game *);

void create_and_set_game_state(Game *);

bool is_valid_move(Game *, Move *);

void make_move(Game *, Move *);

Move *get_move(Game *);

bool is_game_over(Game *);

int get_game_result(Game *);

void print_game_state(Game *);

void end_game(Game *);

#endif