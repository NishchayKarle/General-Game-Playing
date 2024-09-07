#include "minimax.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "ai.h"
#include "game.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Node {
    Game *game_state;
    Move *move;
    struct Node *parent;
    struct Node **children;
    int num_children;
    double score;
} Node;

static Node *create_node(Game *g, Move *m, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));

    node->game_state = copy_game_state(g);
    node->move = copy_move(m);
    node->parent = parent;
    node->children = NULL;
    node->num_children = 0;
    node->score = 0.0;

    return node;
}

static void free_node(Node *n) {
    if (n == NULL) return;

    destroy_game(n->game_state);
    destroy_move(n->move);

    if (n->children != NULL) {
        for (int i = 0; i < n->num_children; i++) {
            free_node(n->children[i]);
        }
        free(n->children);
    }

    free(n);
}

static Node *select_best_child(Node *n, bool maximizing_player) {
    Node *best_child = NULL;
    double best_score = (maximizing_player) ? INT_MIN : INT_MAX;

    for (int i = 0; i < n->num_children; i++) {
        Node *child = n->children[i];
        if (maximizing_player) {
            if (child->score > best_score) {
                best_score = child->score;
                best_child = child;
            }
        } else {
            if (child->score < best_score) {
                best_score = child->score;
                best_child = child;
            }
        }
    }

    return best_child;
}

static void simulate(Node *n, bool maximizing_player) {
    GameState result = is_game_over(n->game_state);

    switch (result) {
        case GAME_WON_BY_PLAYER1:
            n->score = -1.0;
            return;

        case GAME_WON_BY_PLAYER2:
            n->score = 1.0;
            return;

        case GAME_DRAWN:
            n->score = MINIMAX_REWARD_DRAW;
            return;

        default:
            break;
    }

    int num_moves = 0;
    Move **moves = get_possible_moves(n->game_state, &num_moves);

    n->num_children = num_moves;
    n->children = (Node **)malloc(sizeof(Node *) * num_moves);

    for (int i = 0; i < num_moves; i++) {
        Game *game = copy_game_state(n->game_state);
        make_move(game, moves[i]);
        game->player_turn = (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;

        Node *child = create_node(game, moves[i], n);
        n->children[i] = child;
        simulate(child, !maximizing_player);

        if (maximizing_player) {
            n->score = MAX(n->score, child->score);
            if (n->score == 1.0) break;
        } else {
            n->score = MIN(n->score, child->score);
            if (n->score == -1.0) break;
        }
    }

    destroy_list_of_moves(moves, num_moves);
}

Move *minimax(Game *g) {
    Node *root = create_node(g, NULL, NULL);

    simulate(root, true);

    Node *best_child = select_best_child(root, true);

    Move *best_move = copy_move(best_child->move);

    free_node(root);

    return best_move;
}

Move *ai_make_move(Game *g) {
    return minimax(g);
}
