#include "mcts.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ITERATIONS 100000
#define NUM_SECONDS 5
#define UCB1_CONSTANT 1.414
#define REWARD_DRAW 0.0

typedef struct Node {
    Game *game_state;
    Move *move;
    struct Node *parent;
    struct Node **children;
    int num_children;
    int visit_count;
    double win_count;
} Node;

Node *create_node(Game *g, Move *m, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));

    node->game_state = copy_game_state(g);
    node->move = copy_move(m);
    node->parent = parent;
    node->children = NULL;
    node->num_children = 0;
    node->visit_count = 0;
    node->win_count = 0.0;

    return node;
}

void free_node(Node *n) {
    if (n == NULL) return;

    destroy_game(n->game_state);
    free_move(n->move);

    if (n->children != NULL) {
        for (int i = 0; i < n->num_children; i++) {
            free_node(n->children[i]);
        }
        free(n->children);
    }

    free(n);
}

double UCB1(double W, int N, int Nj, double C) {
    return (W / N) + C * sqrt(log(N) / Nj);
}

Node *select_best_child(Node *n) {
    Node *best_child = NULL;
    double best_score = -1.0;

    for (int i = 0; i < n->num_children; i++) {
        Node *child = n->children[i];
        if (child->visit_count == 0) {
            return child;
        }

        double score = UCB1(child->win_count, n->visit_count,
                            child->visit_count, UCB1_CONSTANT);

        if (score > best_score) {
            best_score = score;
            best_child = child;
        }
    }

    return best_child;
}

Node *expand(Node *n) {
    if (n->children == NULL) {
        int num_moves = 0;
        Move **moves = get_possible_moves(n->game_state, &num_moves);

        n->children = (Node **)malloc(sizeof(Node *) * num_moves);
        n->num_children = num_moves;

        for (int i = 0; i < num_moves; i++) {
            Game *game = copy_game_state(n->game_state);
            make_move(game, moves[i]);
            game->player_turn =
                (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;
            n->children[i] = create_node(game, moves[i], n);
            destroy_game(game);
        }

        destroy_list_of_moves(moves, num_moves);
    }

    return select_best_child(n);
}

double simulate(Node *n) {
    Game *game = copy_game_state(n->game_state);

    while (is_game_over(game) == GAME_NOT_FINISHED) {
        int num_moves = 0;
        Move **moves = get_possible_moves(game, &num_moves);
        make_move(game, moves[rand() % num_moves]);
        game->player_turn = (game->player_turn == PLAYER1) ? PLAYER2 : PLAYER1;
        destroy_list_of_moves(moves, num_moves);
    }

    GameState result = is_game_over(game);

    destroy_game(game);

    switch (result) {
        case GAME_DRAWN:
            return REWARD_DRAW;
        case GAME_WON_BY_PLAYER1:
            return 0.0;
        case GAME_WON_BY_PLAYER2:
            return 1.0;
    }
}

void backpropagate(Node *n, double reward) {
    Node *current = n;

    while (current != NULL) {
        current->visit_count++;
        current->win_count += reward;
        current = current->parent;
    }
}

Move *monte_carlo_tree_search(Game *g) {
    Node *root = create_node(g, NULL, NULL);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        Node *selected_child = expand(root);
        double reward = simulate(selected_child);
        backpropagate(selected_child, reward);
    }

    Node *best_child = select_best_child(root);
    Move *best_move = copy_move(best_child->move);

    free_node(root);

    return best_move;
}
