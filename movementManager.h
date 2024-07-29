#include "positionManager.h"
#include "memoryUtils.h"

typedef struct Move
{
  unsigned int origin;
  unsigned int destination;
} Move;

typedef struct MovesList
{
    unsigned int *moves;
    unsigned int size;
    
} MovesList;

typedef struct KingRelatedSquares
{
    unsigned int kingPosition;
    unsigned int linesRelated[8]; /*Each position is 3 bit long
                          the first position is the minimal column to be concerned, the second the maximal, the third the minimal line and the forth the maximal
                          the fifth is the minimal first diagonal, the second the maximal, the third the minimal second diagonal and the forth the maximal
                          */
} KingRelatedSquares;


char isSquareVulnerable(unsigned int position, char color);

void setKingPosition(KingRelatedSquares *kingRelatedSquares, unsigned int position);

void updateKingRelatedPosition(KingRelatedSquares *kingRelatedSquares, char index);

char isSquareRelatedToKing(KingRelatedSquares *kingRelatedSquares, unsigned int position, char checkKnights);

char isASide(unsigned int position, char side);

char canCastle(char* piece, unsigned int* position, char side);

MovesList getPieceMovement(KingRelatedSquares *kingRelatedSquares, char piece, unsigned int position, char checkVulnerability);
