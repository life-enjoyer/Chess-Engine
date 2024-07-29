#include "game.h"
#include "consts.h"

typedef struct Piece {
	char piece;
	Coordinates coordinates;
} Piece;

typedef struct BoardState {
	Piece* pieces;
	Move lastMove;
	char checked;
	unsigned int piecesSize;
	char gameState; // The state of the board, are the whites checkmate -> , the blacks -> , is it a stalemate -> , is it insufficient material -> , or nothing ->
} BoardState;

typedef struct NNInputs {
	double allyPawns[8 * 3];	// 4 inputs for each 8 ally pawns: the first 2 are the x and y coordinates, the third is 1 if the piece is alive and 0 otherwise and the last is for the promotion status
	unsigned char allyPawnsSize; // number of pawns already in the NN inputs
	double allyKnights[2 * 3]; 	// 3 inputs for each 2 ally bishops: x, y coordinates + alive
	unsigned char allyKnightSize; // number of knights already in the NN inputs
	double allyBishops[2 * 3]; 	// same but for bishops
	unsigned char allyBishopsSize; // number of bishops already in the NN inputs
	double allyRooks[2 * 4]; //x, y coordinates, alive bit, cam castle with this rock input
	unsigned char allyRookSize; // number of rooks already in the NN inputs
	double allyQueen[3 * 1]; //x,y coordinates, alive bit
	unsigned char allyQueensSize;
	double allyKing[2 * 1]; //x, y coordinates

	double opponentPawns[8 * 3];	// 4 inputs for each 8 opponent pawns: the first 2 are the x and y coordinates, the third is 1 if the piece is alive and 0 otherwise and the last is for the promotion status
	unsigned char opponentPawnsSize; // number of pawns already in the NN inputs
	double opponentKnights[(2+3) * 3]; 	// 3 inputs for each 2 opponent bishops + 3 for promotion: x, y coordinates + alive
	unsigned char opponentKnightSize; // number of knights already in the NN inputs
	double opponentBishops[(2+3) * 3]; 	// same but for bishops
	unsigned char opponentBishopsSize; // number of bishops already in the NN inputs
	double opponentRooks[(2+3) * 4]; //x, y coordinates, alive bit, cam castle with this rock input
	unsigned char opponentRookSize; // number of rooks already in the NN inputs
	double opponentQueen[(1 + 4) * 3]; //1 + 4 for promotions: x,y coordinates, alive bit
	unsigned char opponentQueensSize;
	double opponentKing[1 * 2]; //x, y coordinates
} NNInputs;

typedef struct StatesRequest {
	BoardState currentBoardState;
	unsigned int* boardStateSize;
	int turnOf;
} StatesRequest;

void importBoard(char new_board[BOARD_SIZE*BOARD_SIZE]);
void createBoardFromState(BoardState state, char flipColors);
BoardState convertBoardToState();
Move* getAllPossibleMoves(char turnOf, unsigned int* possibleMovesSize);
BoardState* get_all_possible_next_board_states(StatesRequest request);
void printAllPossibleBoards(char turnOf);
void printAllPossibleBoardStates(int turnOf);
void print_pieces_array(Piece* pieces, unsigned int size);
void print_board_state(BoardState state);
BoardState get_start_board();