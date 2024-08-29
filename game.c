#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// define _AI_PLAYER_H to play against the AI
// Ex: gcc -D_AI_PLAYER_H <your_game.c> <your_ai.c> game.c -o game
#ifdef _AI_PLAYER_H
#include "ai.h"
#define SINGLE_PLAYER true

void begin_game(Game *game) {
    while (is_game_over(game) == GAME_NOT_FINISHED) {
        Move *move = NULL;

        if (game->player_turn == PLAYER1) {
            // Player 1 is a human player
            move = get_move(game);

            if (!is_valid_move(game, move)) continue;
        } else {
            // Player 2 is the AI using MCTS
            printf("AI is thinking...(Move time limit: %ds)\n",
                   MOVE_TIME_LIMIT);
            move = ai_make_move(game);
            printf("AI made the move: ");
            print_move(move);
        }

        make_move(game, move);
        print_game_board(game);
        game->player_turn = (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;

        destroy_move(move);
    }
}
#endif

#ifndef _AI_PLAYER_H
#define SINGLE_PLAYER false

void begin_game(Game *game) {
    while (is_game_over(game) == GAME_NOT_FINISHED) {
        Move *move = get_move(game);

        if (is_valid_move(game, move)) {
            make_move(game, move);
            print_game_board(game);
            game->player_turn =
                (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;
        }

        destroy_move(move);
    }
}
#endif

int main() {
    srand(time(NULL));

    Game *game = (Game *)malloc(sizeof(Game));
    if (game == NULL) {
        perror("Failed to allocate memory for the game");
        return EXIT_FAILURE;
    }

    setup_players(game, SINGLE_PLAYER);

    create_and_set_game_state(game);

    // Randomly select the first player
    game->player_turn = (rand() % 2 == 0) ? PLAYER1 : PLAYER2;

    printf("INITIAL GAME STATE: \n");
    print_game_board(game);

    // Start the game
    begin_game(game);

    // Display the result of the game
    switch (game->result) {
        case GAME_DRAWN:
            printf("GAME DRAWN\n");
            break;

        case GAME_WON_BY_PLAYER1:
            printf("PLAYER 1 - %s WON\n", game->player1);
            break;

        case GAME_WON_BY_PLAYER2:
            printf("PLAYER 2 - %s WON\n", game->player2);
            break;

        default:
            printf("Error in game result\n");
            break;
    }

    destroy_game(game);

    return 0;
}
