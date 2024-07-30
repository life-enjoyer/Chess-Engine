#include "UIManager.h"

void setupBaseValues();
void setupBasePosition();
char handleMovementRelationWithOpposingKing(char *basePos, char *finalPos, char *isEnPassant);
char handleChecks(KingRelatedSquares blackKingRelatedSquares, char printThings);
char handleDraw(KingRelatedSquares blackKingRelatedSquares, char printThings);
void handleTurnCountReset(char *selectedPiece, unsigned int *origin, unsigned int *destination);
char handleEnPassant(char *selectedPiece, unsigned int *destination);
char handleKingMovement(char *selectedPiece, unsigned int  *origin, unsigned int *destination, KingRelatedSquares whiteKingRelatedSquares);
void handleRookMovement(unsigned int *origin, unsigned int *destination);
void handlePushedPawn(char *selectedPiece, unsigned int *selectedPosition, unsigned int *destinationPosition);
void handlePromotion(char *selectedPiece, unsigned int *destinationPosition);
void endTurn();
void handleTakingPiece(unsigned int *destination, char *isEnPassant);
