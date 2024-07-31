#include "game.h"
#include "consts.h"

#include <stdio.h>
#include <stdlib.h>

char board[BOARD_MEMORY_SIZE] = {0};
char turnOf = 0;
char lastPushedPawn = -1;
char hasKingMoved[2] = {0};
char hasRookMoved[4] = {0}; // 1 left white, 2 right white, 3 left black, 4 right black
char turnCount = 0;
char pieceCount[2] = {27, 27}; // every piece have a value of 2 but the knight and the bishop which have a value of 1 and the king which has no value, when a piece is captured the value of its corresponding piece count is decreased. When the piece count of both side is inferior to 2 it's a draw

void setupBaseValues()
{
    turnOf = 0;
    lastPushedPawn = -1;
    turnCount = 0;

    for (char i = 0; i < 2; i++)
    {
        hasRookMoved[i] = 0;
        pieceCount[i] = 27;
    }

    for (unsigned int i = 0; i < BOARD_MEMORY_SIZE; i++) {
		board[i] = 0;
	}

    for (char i = 0; i < 4; i++)
        hasRookMoved[i] = 0;
}

void setupBasePosition()
{
	Coordinates middle = {(BOARD_SIZE/2)-1, (BOARD_SIZE/2)-1};
	Coordinates blackOrigin = {middle.x - 3, middle.y - 3};
	Coordinates whiteOrigin = {middle.x - 3, middle.y + 3};

	for (unsigned int i = 0; i < 8; i++) {
		setPiece(PAWN | 1, ((blackOrigin.y + 1) * BOARD_SIZE) + blackOrigin.x + i);
		setPiece(PAWN, ((whiteOrigin.y) * BOARD_SIZE) + whiteOrigin.x + i);
	}

	setPiece(ROOK | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x);
	setPiece(ROOK | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 7);
	setPiece(KNIGHT | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 1);
	setPiece(KNIGHT | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 6);
	setPiece(BISHOP | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 2);
	setPiece(BISHOP | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 5);
	setPiece(QUEEN | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 3);
	setPiece(KING | 1, (blackOrigin.y) * BOARD_SIZE + blackOrigin.x + 4);

	setPiece(ROOK, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x);
	setPiece(ROOK, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 7);
	setPiece(KNIGHT, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 1);
	setPiece(KNIGHT, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 6);
	setPiece(BISHOP, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 2);
	setPiece(BISHOP, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 5);
	setPiece(QUEEN, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 3);
	setPiece(KING, (whiteOrigin.y+1) * BOARD_SIZE + whiteOrigin.x + 4);
}

/* UNUSED FUNCTION FOR NOW
char handleMovementRelationWithOpposingKing(char *basePos, char *finalPos, char *isEnPassant)
{
    // return true if the movement is related to the opposing king, false otherwise
    KingRelatedSquares *opposingKingRelatedSquaresPtr = &(kingsRelatedSquares[turnOf ^ 0b01]);

    char basePosRelation = isSquareRelatedToKing(opposingKingRelatedSquaresPtr, *basePos, 0);
    if (basePosRelation)
        updateKingRelatedPosition(opposingKingRelatedSquaresPtr, basePosRelation + 1);

    char enPassantRelation = 0;
    if (*isEnPassant)
    {
        enPassantRelation = isSquareRelatedToKing(opposingKingRelatedSquaresPtr, ((*finalPos) + (turnOf ? -8 : 8)), 0);
        if (enPassantRelation)
            updateKingRelatedPosition(opposingKingRelatedSquaresPtr, enPassantRelation + 1);
    }

    char finalPosRelation = isSquareRelatedToKing(opposingKingRelatedSquaresPtr, *finalPos, 1);
    if (finalPosRelation && finalPosRelation <= BOARD_SIZE)
        updateKingRelatedPosition(opposingKingRelatedSquaresPtr, finalPosRelation + 1);

    return basePosRelation || enPassantRelation || finalPosRelation;
}
*/

char handleChecks(KingRelatedSquares blackKingRelatedSquares, char printThings)
{
    // returns true if there is a checkmate, false otherwise

    if (isSquareVulnerable(blackKingRelatedSquares.kingPosition, turnOf))
    {
        for (unsigned int square = 0; square < BOARD_SIZE*BOARD_SIZE; square++)
        {
            char piece = getPieceAt(square);
            if (piece == EMPTY || (piece & 0x01) == turnOf)
                continue;

            MovesList moves = getPieceMovement(&blackKingRelatedSquares, piece, square, 1, 0);
            if (moves.size > 0)
            {
                free(moves.moves);
                return 1;
            }
            free(moves.moves);
        }

        return 2;
    }

    return 0;
}

char handleDraw(KingRelatedSquares blackKingRelatedSquares, char printThings)
{
    // returns true if there is a draw, false otherwise
    if (turnCount >= 50)
        return 4;

    if (pieceCount[0] < 2 && pieceCount[1] < 2)
        return 5;

    if (isSquareVulnerable(blackKingRelatedSquares.kingPosition, turnOf))
        return 0;

    for (unsigned int square = 0; square < (BOARD_SIZE*BOARD_SIZE); square++)
    {
        char piece = getPieceAt(square);
        if (piece == EMPTY || (piece & 0x01) == turnOf)
            continue;

        MovesList moves = getPieceMovement(&blackKingRelatedSquares, piece, square, 1, 0);
        if (moves.size > 0)
        {
            free(moves.moves);
            return 0;
        }
        free(moves.moves);
    }

    return 3;
}

void handleTurnCountReset(char *selectedPiece, unsigned int *origin, unsigned int *destination)
{
    // reset the turn count if a pawn is moved or a piece is captured
    if ((*selectedPiece & 0b1110) == PAWN || getPieceAt(*destination) != EMPTY)
        turnCount = 0;
}

char handleEnPassant(char *selectedPiece, unsigned int *destination)
{
    // return true if it is an en passant, false otherwise
    if (lastPushedPawn != -1 && (*selectedPiece & 0b1110) == PAWN && *destination == (((2 + ((*selectedPiece & 0x01) * 3)) << 3) | lastPushedPawn))
    {
        setPiece(EMPTY, (((3 + ((*selectedPiece & 0x01) * 2)) << 3) | lastPushedPawn));
        return 1;
    }

    return 0;
}

char handleKingMovement(char *selectedPiece, unsigned int  *origin, unsigned int *destination, KingRelatedSquares whiteKingRelatedSquares)
{
    // return true if a castle occured, false otherwise

    if ((*selectedPiece & 0b1110) != KING)
    {
        return 0;
    }

    if ((*selectedPiece & 0b1110) == KING)
    {
        setKingPosition(&whiteKingRelatedSquares, *destination);

        if (!hasKingMoved[turnOf] && (abs(*destination - *origin)) == 2)
        {
            char castleSide = (*destination > *origin); // 0 for left, 1 for right

            setPiece(EMPTY, *destination + (castleSide ? 1 : -2));
            setPiece(ROOK | turnOf, *destination + (castleSide ? -1 : 1));

            hasRookMoved[(turnOf * 2) + castleSide] = 1;
            hasKingMoved[turnOf] = 1;

            return 1;
        }

        hasKingMoved[turnOf] = 1;
    }
    return 0;
}

void handleRookMovement(unsigned int *origin, unsigned int *destination)
{
    if (*origin == 0 || *destination == 0)
        hasRookMoved[2] = 1;
    if (*origin == 7 || *destination == 7)
        hasRookMoved[3] = 1;
    if (*origin == 56 || *destination == 56)
        hasRookMoved[0] = 1;
    if (*origin == 63 || *destination == 63)
        hasRookMoved[1] = 1;
}

void handlePushedPawn(char *selectedPiece, unsigned int *selectedPosition, unsigned int *destinationPosition)
{
    if ((*selectedPiece & 0b1110) == PAWN && abs(*selectedPosition - *destinationPosition) == 16)
        lastPushedPawn = (*destinationPosition & 0b0111);
    else
        lastPushedPawn = -1;
}

void handlePromotion(char *selectedPiece, unsigned int *destinationPosition)
{
    if ((*selectedPiece & 0b1110) == PAWN && (*destinationPosition >> 3) == (turnOf ? 7 : 0))
    {
        setPiece(QUEEN | turnOf, *destinationPosition);
    }
}

void endTurn()
{
    turnOf = (~turnOf) & 0x01;
    if (turnOf == 0)
        turnCount++;
}

void handleTakingPiece(unsigned int *destination, char *isEnPassant)
{
    char takenPiece = (*isEnPassant) ? getPieceAt(*destination + (turnOf ? BOARD_SIZE : -BOARD_SIZE)) : getPieceAt(*destination);

    if (takenPiece == EMPTY)
        return;

    if ((takenPiece & 0b1110) == BISHOP || (takenPiece & 0b1110) == KNIGHT)
    {
        pieceCount[turnOf ^ 0b01] -= 1;
        return;
    }

    pieceCount[turnOf ^ 0b01] -= 2;
}