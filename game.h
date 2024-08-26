#include <stdbool.h>

#ifndef _GAME_
#define _GAME_

#define PLAYER1 0
#define PLAYER2 1

typedef enum {
    GAME_NOT_FINISHED,
    GAME_DRAWN,
    GAME_WON_BY_PLAYER1,
    GAME_WON_BY_PLAYER2
} GameState;

typedef struct Move Move;

typedef struct Game {
    char player1[11];
    char player2[11];
    bool player_turn;
    void *board;
    GameState result;
    void *extra1;
    void *extra2;
} Game;

void setup_players(Game *);

void create_and_set_game_state(Game *);

bool is_valid_move(Game *, Move *);

void make_move(Game *, Move *);

Move *get_move(Game *);

GameState is_game_over(Game *);

void print_game_state(Game *);

void end_game(Game *);

#endif