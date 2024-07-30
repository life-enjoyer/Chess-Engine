#include "movementManager.h"

#define EMPTY 0x00 //0000
#define KNIGHT 0x02 //0010
#define OOB 0x04 //0100
#define KING 0x06 //0110

#define PAWN 0x08 //1000
#define BISHOP 0x0A //1010
#define ROOK 0x0C //1100
#define QUEEN 0x0E //1110

void getPieceName(char* binary, char* name);
void writeSelectionCharacter(unsigned int* possibleMoves, unsigned int possibleMovesLenght, unsigned char* selectedPiece, unsigned int position);
void printBoard(MovesList *moves, unsigned char* selectedPiece);
void printBinary(char byte);


