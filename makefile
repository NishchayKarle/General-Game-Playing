# Default target when no specific target is specified
default:
	@echo "Please specify a target to build. Available targets: tictactoe, connect4, gomoku, checkers, tictactoe_ai, connect4_ai, gomoku_ai, tictactoe_mcts, connect4_mcts, gomoku_mcts, tictactoe_minimax, connect4_minimax, gomoku_minimax"

# Targets for Tic-Tac-Toe
tictactoe: game.c game.h tictactoe.c 
	gcc -o tictactoe -Ofast game.c tictactoe.c -lm -DP_VS_P

tictactoe_ai: game.c game.h tictactoe.c ai.h mcts.c mcts.h
	gcc -o tictactoe_ai -Ofast game.c tictactoe.c mcts.c -lm -DAI_VS_P

tictactoe_mcts: game.c game.h tictactoe.c mcts.c mcts.h ai.h
	gcc -o tictactoe_mcts -Ofast game.c tictactoe.c mcts.c -lm -DAI_VS_P

tictactoe_minimax: game.c game.h tictactoe.c minimax.c minimax.h ai.h
	gcc -o tictactoe_minimax -Ofast game.c tictactoe.c minimax.c -lm -DAI_VS_P

# Targets for Connect 4
connect4: game.c game.h connect4.c
	gcc -o connect4 -Ofast game.c connect4.c -lm -DP_VS_P

connect4_ai: game.c game.h connect4.c ai.h mcts.c mcts.h
	gcc -o connect4_ai -Ofast game.c connect4.c mcts.c -lm -DAI_VS_P

connect4_mcts: game.c game.h connect4.c mcts.c mcts.h ai.h
	gcc -o connect4_mcts -Ofast game.c connect4.c mcts.c -lm -DAI_VS_P

connect4_minimax: game.c game.h connect4.c minimax.c minimax.h ai.h
	gcc -o connect4_minimax -Ofast game.c connect4.c minimax.c -lm -DAI_VS_P

# Targets for Gomoku
gomoku: game.c game.h gomoku.c
	gcc -o gomoku -Ofast game.c gomoku.c -lm -DP_VS_P

gomoku_ai: game.c game.h gomoku.c mcts.c ai.h mcts.h
	gcc -o gomoku_ai -Ofast game.c gomoku.c mcts.c -lm -DAI_VS_P

gomoku_mcts: game.c game.h gomoku.c mcts.c mcts.h ai.h
	gcc -o gomoku_mcts -Ofast game.c gomoku.c mcts.c -lm -DAI_VS_P

gomoku_minimax: game.c game.h gomoku.c minimax.c minimax.h ai.h
	gcc -o gomoku_minimax -Ofast game.c gomoku.c minimax.c -lm -DAI_VS_P

checkers: game.c game.h checkers.c
	gcc -o checkers -Ofast game.c checkers.c -lm -lcurses -DP_VS_P

# Clean target to remove all compiled files
clean:
	rm -f tictactoe connect4 gomoku tictactoe_ai connect4_ai gomoku_ai tictactoe_mcts connect4_mcts gomoku_mcts tictactoe_minimax connect4_minimax gomoku_minimax