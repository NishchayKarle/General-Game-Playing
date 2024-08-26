#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    Game *game = (Game *)malloc(sizeof(Game));
    if (game == NULL) {
        perror("Failed to allocate memory for the game");
        return EXIT_FAILURE;
    }

    setup_players(game);

    create_and_set_game_state(game);

    // Randomly select the first player
    game->player_turn = (rand() % 2 == 0) ? PLAYER1 : PLAYER2;

    printf("INITIAL GAME STATE: \n");
    print_game_board(game);

    // Main game loop
    while (is_game_over(game) == GAME_NOT_FINISHED) {
        Move *move = get_move(game);

        if (is_valid_move(game, move)) {
            printf("\n");
            make_move(game, move);
            print_game_board(game);
            game->player_turn =
                (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;
        }
    }

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
            printf("ERROR: Game result not set after the game is finished\n");
            break;
    }

    end_game(game);
    free(game);

    return 0;
}