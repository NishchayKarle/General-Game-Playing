# Default target when no specific target is specified
default:
	@echo "Please specify a target to build. Available targets: tictactoe, tictactoe_ai, connect4, connect4_ai, gomoku, gomoku_ai"

# Targets for Tic-Tac-Toe
tictactoe: game.c game.h tictactoe.c 
	gcc -o tictactoe -Ofast game.c tictactoe.c -lm -DP_VS_P

tictactoe_ai: game.c game.h tictactoe.c ai.h mcts.c
	gcc -o tictactoe_ai -Ofast game.c tictactoe.c mcts.c -lm -DAI_VS_P

# Targets for Connect 4
connect4: game.c game.h connect4.c
	gcc -o connect4 -Ofast game.c connect4.c -lm -DP_VS_P

connect4_ai: game.c game.h connect4.c ai.h mcts.c
	gcc -o connect4_ai -Ofast game.c connect4.c mcts.c -lm -DAI_VS_P

# Targets for Gomoku
gomoku: game.c game.h gomoku.c
	gcc -o gomoku -Ofast game.c gomoku.c -lm -DP_VS_P

gomoku_ai: game.c game.h gomoku.c mcts.c ai.h
	gcc -o gomoku_ai -Ofast game.c gomoku.c mcts.c -lm -DAI_VS_P

# Clean target to remove all compiled files
clean:
	rm -f tictactoe connect4 gomoku tictactoe_ai connect4_ai gomoku_ai