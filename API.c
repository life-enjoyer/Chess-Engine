#include "API.h"

#include "consts.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern char board[BOARD_MEMORY_SIZE];

void createBoardFromState(BoardState state, char flipColors)  {
	for (unsigned int i = 0; i < (unsigned int)((BOARD_SIZE*BOARD_SIZE)/2); i++) {
		board[i] = ((EMPTY << 4) & 0xF0) | (EMPTY & 0x0F);
	}
	for (int i = 0; i < state.piecesSize; i++) {
		if (flipColors) {
			setPiece((char)((state.pieces[i].piece & 0b11111110) | ((state.pieces[i].piece ^ 0x01) & 0x01)), state.pieces[i].coordinates.y * BOARD_SIZE + state.pieces[i].coordinates.x);
		} else {
			setPiece(state.pieces[i].piece, state.pieces[i].coordinates.y * BOARD_SIZE + state.pieces[i].coordinates.x);
		}
	}
}

BoardState convertBoardToState() {
	KingRelatedSquares* whitesKingRelatedSquares = getKingRelatedSquares(0);
	KingRelatedSquares* blacksKingRelatedSquares = getKingRelatedSquares(1);

	BoardState state = {NULL, 0, 0, 0, whitesKingRelatedSquares, blacksKingRelatedSquares};
	state.pieces = malloc(sizeof(Piece)*2);
	if (state.pieces == NULL) {
		printf("Memory Allocation failed");
		exit(0);
	}
	state.piecesSize = 0;

	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		char pieceCode = getPieceAt(i);
		if (pieceCode == EMPTY)
			continue;

		state.piecesSize++;
		if (state.piecesSize > 2) {
			state.pieces = realloc(state.pieces, sizeof(Piece)*(state.piecesSize));
			if (state.pieces == NULL) {
				printf("Memory Allocation failed");
				exit(0);
			}
		}

		Coordinates pieceCoordinates = {i%BOARD_SIZE, (unsigned int)(floor((double)i/BOARD_SIZE))};
		Piece piece = {pieceCode, pieceCoordinates};
		state.pieces[state.piecesSize-1] = piece;
	}

	state.checked = 0;
	state.gameState = 0;

	return state;
}

void importBoard(char new_board[BOARD_SIZE*BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		setPiece(getPieceAt(i), i);
	}
}

Move* getAllPossibleMoves(char turnOf, unsigned int* possibleMovesSize, char checked, KingRelatedSquares whiteKingRelatedSquares) {
	*possibleMovesSize = 0;
	Move* possibleMoves = (Move*) calloc(1, sizeof(Move));
	if (possibleMoves == NULL) {
		printf("Memory not allocated.\n");
		exit(0);
	}

	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		char piece = getPieceAt(i);

		if (piece == EMPTY)
			continue;

		if ((piece & 0x01) != (turnOf & 0x01))
			continue;

		MovesList movesList = getPieceMovement(&whiteKingRelatedSquares, piece, i, 1, checked);

		if (movesList.size == 0) {
			free(movesList.moves);
			continue;
		}

		*possibleMovesSize += movesList.size;
		possibleMoves = (Move*) realloc(possibleMoves, *possibleMovesSize * sizeof(Move));


		for (int j = 0; j < movesList.size; j++) {
			Move move = {i, movesList.moves[j]};
			possibleMoves[*possibleMovesSize - movesList.size + j] = move;
		}

		free(movesList.moves);
	}


	return possibleMoves;
}

BoardState* get_all_possible_next_board_states(StatesRequest request) {
	createBoardFromState(request.currentBoardState, request.turnOf == 1 ? 1 : 0);
	Move* possibleMoves = getAllPossibleMoves(0, request.boardStateSize, request.currentBoardState.checked, *request.currentBoardState.whiteKingsRelatedSquares);

	BoardState* possibleBoardStates = malloc(sizeof(BoardState) * (*request.boardStateSize));

	for (int i = 0; i < *request.boardStateSize; i++) {
		char movedPiece = getPieceAt(possibleMoves[i].origin);
		setPiece(EMPTY, possibleMoves[i].origin);
		char removedPiece = getPieceAt(possibleMoves[i].destination);
		setPiece(movedPiece, possibleMoves[i].destination);

		possibleBoardStates[i] = convertBoardToState();
		set_game_state(request.currentBoardState, possibleBoardStates[i], possibleMoves[i], 0);

		setPiece(movedPiece, possibleMoves[i].origin);
		setPiece(removedPiece, possibleMoves[i].destination);
	}

	free(possibleMoves);
	return possibleBoardStates;
}

void freeBoardStates(BoardState* boardStates, unsigned int boardStatesSize) {
	for (unsigned int i = 0; i < boardStatesSize; i++) {
		free(boardStates[i].pieces);
		free(boardStates[i].whiteKingsRelatedSquares);
		free(boardStates[i].blackKingRelatedSquares);
 	}
	free(boardStates);
}

void printAllPossibleBoards(char turnOf) {
	unsigned int size;

	setupBaseValues();
	setupBasePosition();

	KingRelatedSquares* kingRelatedSquares = getKingRelatedSquares(0);
	Move* moveList = getAllPossibleMoves(turnOf, &size, 0, *kingRelatedSquares);
	free(kingRelatedSquares);


	for (unsigned int i = 0; i < size; i++) {
		Move move = moveList[i];

		setupBaseValues();
		setupBasePosition();

		setPiece(getPieceAt(move.origin), move.destination);
		setPiece(EMPTY, move.origin);

		char destinationPiece = getPieceAt(move.destination);

		printBoard(NULL, NULL);

		setPiece(getPieceAt(move.destination), move.origin);
		setPiece(destinationPiece, move.destination);
	}

	free(moveList);
}

void printAllPossibleBoardStates(int turnOf) {
	unsigned int size;

	setupBaseValues();
	setupBasePosition();
	BoardState currentState = convertBoardToState();

	StatesRequest statesRequest = {currentState, &size, turnOf};

	BoardState* states = get_all_possible_next_board_states(statesRequest);

	for (unsigned int i = 0; i < size; i++) {
		createBoardFromState(states[i], 0);
		printBoard(NULL, NULL);
		//free(states[i].pieces);
	}

	freeBoardStates(states, size);
}

void print_pieces_array(Piece* array, unsigned int size) {
	for (unsigned int i = 0; i < size; i++) {
		printf("Piece %d: x = %d, y = %d, piece = %d\n", i, array[i].coordinates.x, array[i].coordinates.y, array[i].piece);
	}
}

void print_board_state(BoardState state) {
	printf("Pieces size: %d\n", state.piecesSize);
	printf("Status: %d\n", state.gameState);

	for (unsigned int i = 0; i < state.piecesSize; i++) {
		printf("Piece %d: x = %d, y = %d, piece = %d\n", i, state.pieces[i].coordinates.x, state.pieces[i].coordinates.y, state.pieces[i].piece);
	}
	printf("\n\n");
}

BoardState get_start_board() {
	setupBaseValues();
	setupBasePosition();
	return convertBoardToState();
}

void set_game_state(BoardState previousState, BoardState nextState, Move lastMove, char turnOf)
{
	char selectedPiece = getPieceAt(lastMove.origin);
	createBoardFromState(previousState, turnOf);

	char enPassant = handleEnPassant(&selectedPiece, &lastMove.destination);
	handleTurnCountReset(&selectedPiece, &lastMove.origin, &lastMove.destination);
	handleTakingPiece(&lastMove.destination, &enPassant);

	setPiece(EMPTY, lastMove.origin);
	setPiece(selectedPiece, lastMove.destination);


	char castle = handleKingMovement(&selectedPiece, &lastMove.origin, &lastMove.destination, *previousState.whiteKingsRelatedSquares);
	if (!castle)
	{
		handleRookMovement(&lastMove.origin, &lastMove.destination);
		handlePushedPawn(&selectedPiece, &lastMove.origin, &lastMove.destination);
	}

	handlePromotion(&selectedPiece, &lastMove.destination);


	// this optimization works most of the time but under certain circumstances it can cause a bug (still don't know why)
	// if (handleMovementRelationWithOpposingKing(&move.origin, &move.destination, &enPassant))
	//{
	char handledCheck = handleChecks(*previousState.blackKingRelatedSquares, 0);
	if (handledCheck == 2) {
		nextState.gameState = (char)((turnOf ^ 0b01) + 1);
		return;
	}
	if (handledCheck == 1) {
		nextState.gameState = 0;
		return;
	}
	//}

	nextState.checked = 0;
	nextState.gameState = handleDraw(*previousState.blackKingRelatedSquares,0);
}