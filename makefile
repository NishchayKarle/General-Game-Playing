default:
	@echo "Please specify a target to build. Available targets: tictactoe, connect4, gomoku"

tictactoe: game.c game.h tictactoe.c mcts.c mcts.h
	gcc -o tictactoe -Ofast game.c tictactoe.c mcts.c -lm

connect4: game.c game.h connect4.c mcts.c mcts.h
	gcc -o connect4 -Ofast game.c connect4.c mcts.c -lm

gomoku: game.c game.h gomoku.c mcts.c mcts.h
	gcc -o gomoku -Ofast game.c gomoku.c mcts.c -lm

clean:
	rm -f tictactoe connect4 gomoku