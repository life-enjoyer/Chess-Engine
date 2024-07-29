#include <stddef.h>
#include "API.h"
#include "consts.h"

extern char board[BOARD_MEMORY_SIZE];

int main() {
	//printAllPossibleBoardStates(0);
	setupBaseValues();
	setupBasePosition();
	printBoard(NULL, NULL);
	printAllPossibleBoards(0);
	printBoard(NULL, NULL);
	//printBinary(board[65]);
}