#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    Game *game = (Game *)malloc(sizeof(Game));

    setup_players(game);

    create_and_set_game_state(game);

    bool player_turn = rand() % 2;
    printf("INITIAL GAME STATE: \n");
    print_game_state(game);

    while (!is_game_over(game)) {
        game->player_turn = player_turn;
        Move *move = get_move(game);

        if (is_valid_move(game, move)) {
            make_move(game, move);
            print_game_state(game);
            player_turn = !player_turn;
        }
    }

    int game_result = get_game_result(game);

    switch (game_result) {
        case GAME_DRAWN:
            printf("GAME DRAWN\n");
            break;

        case GAME_WON_BY_PLAYER1:
            printf("PLAYER 1 - %s WON\n", game->player1);
            break;

        case GAME_WON_BY_PLAYER2:
            printf("PLAYER 2 - %s WON\n", game->player2);
            break;
    }

    end_game(game);
    free(game);
}