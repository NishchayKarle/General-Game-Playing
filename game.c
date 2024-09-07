#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef AI_VS_P
#include "ai.h"
#define SINGLE_PLAYER true

void begin_game(Game *game) {
    char ai_move_str[100];
    snprintf(ai_move_str, sizeof(ai_move_str),
             "AI is thinking...(Move time limit: %ds)\n", MOVE_TIME_LIMIT);

    while (is_game_over(game) == GAME_NOT_FINISHED) {
        Move *move = NULL;

        if (game->player_turn == PLAYER1) {
            move = get_move(game);
            if (!is_valid_move(game, move)) continue;
        } else {
            print(ai_move_str);
            move = ai_make_move(game);
            print_move(game, move);
        }

        bool done = make_move(game, move);
        print_game_board(game);

        if (done)
            game->player_turn =
                (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;

        destroy_move(move);
    }
}
#endif

#ifdef P_VS_P
#define SINGLE_PLAYER false

void begin_game(Game *game) {
    while ((game->result = is_game_over(game)) == GAME_NOT_FINISHED) {
        Move *move = get_move(game);

        if (is_valid_move(game, move)) {
            bool done = make_move(game, move);
            print_game_board(game);
            print_move(game, move);
            if (done)
                game->player_turn =
                    (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;
        }

        destroy_move(move);
    }
}
#endif

int main() {
    srand(time(NULL));

    init();

    Game *game = (Game *)malloc(sizeof(Game));
    if (game == NULL) {
        print("Failed to allocate memory for the game");
        exit(EXIT_FAILURE);
    }

    init_game_state(game);

    setup_players(game, SINGLE_PLAYER);

    // Randomly select the first player
    game->player_turn = (rand() % 2 == 0) ? PLAYER1 : PLAYER2;

    print_game_board(game);

    // Start the game
    begin_game(game);

    // Display the result of the game
    display_result(game);

    destroy_game(game);
    end();

    return EXIT_SUCCESS;
}
