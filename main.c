#include <stddef.h>
#include "API.h"
#include "consts.h"

extern char board[BOARD_MEMORY_SIZE];

int main() {
	BoardState currentBoardState = get_start_board();
	unsigned int boardStatesSize = 0;

	StatesRequest request = {currentBoardState, &boardStatesSize, 0};

	BoardState* all_boards = get_all_possible_next_board_states(request);

	for (int i = 0; i < boardStatesSize; i++) {
		print_board_state(all_boards[i]);
	}
}