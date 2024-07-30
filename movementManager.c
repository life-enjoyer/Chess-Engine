#include "movementManager.h"
#include "consts.h"

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#define BLACKS_FORWARD (2 * 4)

extern char board[BOARD_MEMORY_SIZE];
extern char lastPushedPawn;
extern char hasKingMoved[2];
extern char hasRookMoved[4];

char checkSlidingPieceMovement(char direction, char color, int *checkedPosition)
{
    // return codes: 0b00 - movement allowed, 0b10 - block next movement but allow this one, 0b11 - movement blocked
    char checkedPiece = getPieceAt(*checkedPosition);

    char isAlly = (char)(((checkedPiece & 0x0E) != EMPTY) && (color == ((checkedPiece & 0x01))));
    char sided = isASide(*checkedPosition, direction);
    return (isAlly * 0b11) + (!isAlly * (sided * 0b10)) + (!isAlly * !sided * (!((checkedPiece & 0x0E) == EMPTY) * 2));
}

char knightWentThroughtTheBoard(unsigned int *previousPosition, unsigned int *currentPosition)
{
	unsigned int previous_x, previous_y, current_x, current_y;
	previous_x = (*previousPosition) % BOARD_SIZE;
	previous_y = floor((double)(*previousPosition)/BOARD_SIZE);
	current_x = (*currentPosition) % BOARD_SIZE;
	current_y = floor((double)(*currentPosition)/BOARD_SIZE);

    return (((*currentPosition) < 0) ||
            ((previous_y < 2) && (current_y >= 6)) ||
            ((previous_y >= 6) && (current_y < 2)) ||
            ((previous_x < 2) && (current_x >= 6)) ||
            ((previous_x >= 6) && (current_x < 2)));
}

char isSquareVulnerable(unsigned int position, char color)
{
    // BISHOP CHECKS
    for (char index = 0; index < 4; index++)
    {
        if (isASide(position, index | 0b100))
            continue;

        for (unsigned int i = 1; i < BOARD_SIZE; i++)
        {
            int checkedPosition = ((index & 0b10) * (position + i) + (((index - 2) * 2) - 1) * (i * BOARD_SIZE)) + ((!(index & 0b10)) * ((position - i) + ((index * 2) - 1) * (i * BOARD_SIZE)));
            char returnCode = checkSlidingPieceMovement(index | 0b100, (color ^ 0b01), &checkedPosition);

            if (returnCode ^ 0b011)
            {
                if ((getPieceAt(checkedPosition) & 0b01011) == (0b01010 | color))
                    return 1;
            }

            if (returnCode & 0b10)
                break;
        }
    }

    // ROOK CHECKS
    for (char index = 0; index < 4; index++)
    {
        if (isASide(position, index))
            continue;

        for (unsigned int i = 1; i < BOARD_SIZE; i++)
        {
            int checkedPosition = ((index < 2) * (position + ((index * 2) - 1) * i)) + ((index >= 2) * (position + (((index - 2) * 2) - 1) * (i * BOARD_SIZE)));
            char returnCode = checkSlidingPieceMovement(index, (color ^ 0b01), &checkedPosition);

            if (returnCode ^ 0b011)
            {
                if ((getPieceAt(checkedPosition) & 0b01101) == (0b01100 | color))
                    return 1;
            }

            if (returnCode & 0b10)
                break;
        }
    }

    // KNIGHT CHECKS
    for (char direction = 0; direction < 4; direction++)
    {
        for (char c = 0; c < 2; c++)
        {
            int possibleJump = position + BOARD_SIZE * (1 - (2 * ((int)floor(direction / 2)))) * (c % 2 + 1) - (2 - c) * (2 * (direction % 2) - 1);
            if (knightWentThroughtTheBoard(&position, &possibleJump))
                continue;

            char pieceAtJump = getPieceAt(possibleJump);
            if (pieceAtJump == (KNIGHT | (color)))
                return 1;
        }
    }

    // KING CHECKS
    for (char direction = 0; direction < 8; direction++)
    {
        if (isASide(position, direction))
            continue;

        if ((((direction & 0b100) && getPieceAt(position + (((direction & 0b001) * 2) - 1) * BOARD_SIZE + ((((direction >> 1) & 0x01) * 2) - 1)) == (KING | (color)))) || ((!(direction & 0b100)) && (getPieceAt(position + (((direction & 0b001) * 2) - 1) * (1 + ((BOARD_SIZE-1) * ((direction >> 1) & 0x01)))) == (KING | color))))
            return 1;
    }

    // PAWN CHECKS
    char forward = ((color * 2) - 1) * - BLACKS_FORWARD;
    unsigned int firstSquareForward = position + forward;

    unsigned int firstRightDiagonal = firstSquareForward + 1;
    unsigned int firstLeftDiagonal = firstSquareForward - 1;

    if ((!isASide(position, 0b01) && (getPieceAt(firstRightDiagonal)) == (PAWN | color)) || (!isASide(position, 0b0) && (getPieceAt(firstLeftDiagonal)) == (PAWN | color)))
        return 1;

    return 0;
}

KingRelatedSquares* getKingRelatedSquares(char color) {
	KingRelatedSquares *kingRelatedSquares = malloc(sizeof(KingRelatedSquares));
	*kingRelatedSquares = (KingRelatedSquares){0, {0, 0, 0, 0, 0, 0, 0, 0}};

	for (unsigned int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		if (getPieceAt(i) == (KING | color)) {
			kingRelatedSquares->kingPosition = i;
			break;
		}
	}

	setKingPosition(kingRelatedSquares, kingRelatedSquares->kingPosition);

	return kingRelatedSquares;
}

void setKingPosition(KingRelatedSquares *kingRelatedSquares, unsigned int position)
{
    kingRelatedSquares->kingPosition = position;

    for (char i = 0; i < 8; i++)
    {
        updateKingRelatedPosition(kingRelatedSquares, i);
    }
}

void updateKingRelatedPosition(KingRelatedSquares *kingRelatedSquares, char index)
{
    int linePosition = (int)(floor((double)kingRelatedSquares->kingPosition/BOARD_SIZE));
	int columnPosition = (int)(kingRelatedSquares->kingPosition % BOARD_SIZE);

    if (index == 0)
    {
        for (int column = columnPosition - 1; column >= 1; column--)
        {
            if (column < 1)
                break;

            if (getPieceAt(linePosition | column) != EMPTY)
            {
                kingRelatedSquares->linesRelated[0] = column;
                return;
            }
        }
        kingRelatedSquares->linesRelated[0] = 0;
        return;
    }
    if (index == 1)
    {
        for (int column = columnPosition + 1; column < 7; column++)
        {
            if (column >= 7)
                break;

            if (getPieceAt(linePosition | column) != EMPTY)
            {
                kingRelatedSquares->linesRelated[1] = column;
                return;
            }
        }
        kingRelatedSquares->linesRelated[1] = BOARD_SIZE-1;
        return;
    }
    if (index == 2)
    {
        for (int lines = linePosition - BOARD_SIZE; lines >= BOARD_SIZE; lines -= BOARD_SIZE)
        {
            if (lines < BOARD_SIZE)
                break;

            if (getPieceAt(lines | columnPosition) != EMPTY)
            {
                kingRelatedSquares->linesRelated[2] = lines/BOARD_SIZE;
                return;
            }
        }
		kingRelatedSquares->linesRelated[2] = 0;
        return;
    }
    if (index == 3)
    {
        for (int lines = linePosition + BOARD_SIZE; lines < BOARD_SIZE*(BOARD_SIZE-1); lines += BOARD_SIZE)
        {
            if (lines >= BOARD_SIZE*(BOARD_SIZE-1))
                break;
            if (getPieceAt(lines | columnPosition) != EMPTY)
            {
                kingRelatedSquares->linesRelated[3] = lines/BOARD_SIZE;
                return;
            }
        }
        kingRelatedSquares->linesRelated[3] = BOARD_SIZE-1;
        return;
    }
    if (index == 4)
    {
        for (int i = (linePosition >> 3) - 1; i >= 1; i--)
        {
            if (i < 1)
                break;

            if (getPieceAt((i << 3) | (columnPosition - i)) != EMPTY)
            {
                kingRelatedSquares->linesRelated[4] = i;
                return;
            }
        }
        kingRelatedSquares->linesRelated[4] = 0;
        return;
    }
    if (index == 5)
    {
        for (int i = linePosition + 1; i < BOARD_SIZE-1; i++)
        {
            if (i >= 7)
                break;

            if (getPieceAt((i << 3) | (columnPosition + i)) != EMPTY)
            {
                kingRelatedSquares->linesRelated[5] = i;
                return;
            }
        }
        kingRelatedSquares->linesRelated[5] = BOARD_SIZE-1;
        return;
    }
    if (index == 6)
    {
        for (int i = linePosition - 1; i >= 1; i--)
        {
            if (i < 1)
                break;

            if (getPieceAt((i << 3) | (columnPosition + i)) != EMPTY)
            {
                kingRelatedSquares->linesRelated[6] = i;
                return;
            }
        }
        kingRelatedSquares->linesRelated[6] = 0;
        return;
    }
    if (index == 7)
    {
        for (int i = linePosition + 1; i < BOARD_SIZE-1; i++)
        {
            if (i >= 7)
                break;

            if (getPieceAt((i << 3) | (columnPosition - i)) != EMPTY)
            {
                kingRelatedSquares->linesRelated[7] = i;
                return;
            }
        }
        kingRelatedSquares->linesRelated[7] = BOARD_SIZE-1;
        return;
    }
}

char isSquareRelatedToKing(KingRelatedSquares *kingRelatedSquares, unsigned int position, char checkKnights)
{
    // returns 0 (false) if the square is unrelated, returns the index of the related line + 1 otherwise

    unsigned int linePosition = floor((double)position/BOARD_SIZE);
    unsigned int columnPosition = position % BOARD_SIZE;

    unsigned int kingLinePosition = (unsigned int)(floor((double)kingRelatedSquares->kingPosition/BOARD_SIZE));
    unsigned int kingColumnPosition = kingRelatedSquares->kingPosition % BOARD_SIZE;

    if (linePosition == kingLinePosition)
        return (char)(((char)((columnPosition >= kingRelatedSquares->linesRelated[0]) && (columnPosition <= kingRelatedSquares->linesRelated[1]))) *
               ((columnPosition >= kingColumnPosition) + 1));

    if (columnPosition == kingColumnPosition)
        return (char)(((linePosition >= kingRelatedSquares->linesRelated[2] && linePosition <= kingRelatedSquares->linesRelated[3]) *
                ((linePosition >= kingLinePosition) + 3)));

    if ((linePosition - columnPosition) == (kingLinePosition - kingColumnPosition))
        return (char)(((linePosition >= kingRelatedSquares->linesRelated[4] && linePosition <= kingRelatedSquares->linesRelated[5]) *
                ((linePosition >= kingLinePosition) + 5)));

    if ((columnPosition + linePosition) == (kingColumnPosition + kingLinePosition))
        return (char)((linePosition >= kingRelatedSquares->linesRelated[6] && linePosition <= kingRelatedSquares->linesRelated[7]) *
               ((linePosition <= kingLinePosition) + BOARD_SIZE-1));

    if (!checkKnights)
        return 0;

    for (char direction = 0; direction < 4; direction++)
    {
        for (char c = 0; c < 2; c++)
        {
            unsigned int possibleJump = position + BOARD_SIZE * (1 - (2 * ((int)floor((double)direction / 2)))) * (c % 2 + 1) - (2 - c) * (2 * (direction % 2) - 1);

            if (knightWentThroughtTheBoard(&position, &possibleJump) || possibleJump != kingRelatedSquares->kingPosition)
                continue;

            return 9;
        }
    }

    return 0;
}

void tryToAddPosition(KingRelatedSquares *kingRelatedSquares, MovesList *moves, char origininIsRelatedToKing, unsigned int origin, unsigned int destination, char pieceAtOrigin, char pieceAtDestination, char *checkVulnerability, char checked)
{
    if (checked && !isSquareRelatedToKing(kingRelatedSquares, destination, 0))
        return;

    if ((checked || origininIsRelatedToKing || isSquareRelatedToKing(kingRelatedSquares, destination, 0)) && *checkVulnerability)
    {
        setPiece(getPieceAt(origin), destination);
        setPiece(EMPTY, origin);

        char isAnEnPassant = lastPushedPawn != -1 && (pieceAtOrigin & 0b1110) == PAWN && (pieceAtDestination & 0b1110) == EMPTY && (destination - origin) % BOARD_SIZE != 0;

        if (isAnEnPassant)
            setPiece(EMPTY, destination + (BOARD_SIZE * (((pieceAtOrigin & 0x01) * 2) - 1)));

        if (!isSquareVulnerable(kingRelatedSquares->kingPosition, (pieceAtOrigin & 0x01) ^ 0b01))
        {
			moves->moves[moves->size] = destination;
            moves->size++;
        }

        if (isAnEnPassant)
            setPiece(PAWN, destination + (BOARD_SIZE * (((pieceAtOrigin & 0x01) * 2) - 1)));

        setPiece(pieceAtOrigin, origin);
        setPiece(pieceAtDestination, destination);

        return;
    }

	moves->moves[moves->size] = destination;
    moves->size++;
}

char isASide(unsigned int position, char side)
{
    // side: 0b000 - left, 0b001 - right, 0b010 - top, 0b011 - bottom, 0b100 - top left, 0b101 - bottom left,
	// 0b110 - top right, 0b111 - bottom right

	unsigned int x, y;
	x = position % BOARD_SIZE;
	y = (unsigned int)floor((double)position/BOARD_SIZE);

	return ((side == 0 && x == 0) ||
		(side == 1 && x == BOARD_SIZE-1) ||
		(side == 2 && y == 0) ||
		(side == 3 && y == BOARD_SIZE-1) ||
		(side == 4 && y == 0 && x == 0) ||
		(side == 5 && y == BOARD_SIZE-1 && x == 0) ||
		(side == 6 && y == 0 && x == BOARD_SIZE-1) ||
		(side == 7 && y == BOARD_SIZE-1 && x == BOARD_SIZE-1));
	;
}

void tryToAddPositionIfEmpty(KingRelatedSquares *kingRelatedSquares, MovesList *moves, char origininIsRelatedToKing, unsigned int origin, unsigned int destination, char pieceAtOrigin, char sideToCheck, char *checkVulnerability, char checked)
{
    char pieceAtDestination = getPieceAt(destination);

    if (!(isASide(origin, sideToCheck)) && (pieceAtDestination == EMPTY))
        tryToAddPosition(kingRelatedSquares, moves, origininIsRelatedToKing, origin, destination, pieceAtOrigin, pieceAtDestination, checkVulnerability, checked);
}

void tryToAddPositionIfEnemy(KingRelatedSquares *kingRelatedSquares, MovesList *moves, char origininIsRelatedToKing, unsigned int origin, unsigned int destination, char pieceAtOrigin, char sideToCheck, char *checkVulnerability, char checked)
{
    char pieceAtDestination = getPieceAt(destination);
    if (!(isASide(origin, sideToCheck)) && (pieceAtDestination != EMPTY && ((pieceAtDestination & 0x01) == ((pieceAtOrigin ^ 0x01) & 0x01))))
        tryToAddPosition(kingRelatedSquares, moves, origininIsRelatedToKing, origin, destination, pieceAtOrigin, pieceAtDestination, checkVulnerability, checked);
}

void tryToAddPositionIfEnemyOrEmpty(KingRelatedSquares *kingRelatedSquares, MovesList *moves, char origininIsRelatedToKing, unsigned int origin, unsigned int destination, char pieceAtOrigin, char sideToCheck, char *checkVulnerability, char checked)
{
    char pieceAtDestination = getPieceAt(destination);
    if ((!(sideToCheck != -1 && isASide(origin, sideToCheck))) && (pieceAtDestination == EMPTY || ((pieceAtDestination & 0x01) == ((pieceAtOrigin ^ 0x01) & 0x01))))
        tryToAddPosition(kingRelatedSquares, moves, origininIsRelatedToKing, origin, destination, pieceAtOrigin, pieceAtDestination, checkVulnerability, checked);
}

char canCastle(char *piece, unsigned int *position, char side)
{
    // side: 0 left, 1 right

    if (isSquareVulnerable(*position, (*piece & 0x01) ^ 0b01))
        return 0;

    if (hasKingMoved[(*piece & 0x01)] || hasRookMoved[((*piece & 0x01) * 2) + side])
        return 0;

    if (side == 0)
    {
        for (char i = 1; i < 4; i++)
        {
            if (getPieceAt(*position - i) != EMPTY || isSquareVulnerable(*position - i, (*piece & 0x01) ^ 0b01))
                return 0;
        }

        return 1;
    }

    for (char i = 1; i < 3; i++)
    {
        if (getPieceAt(*position + i) != EMPTY || isSquareVulnerable(*position + i, (*piece & 0x01) ^ 0b01))
            return 0;
    }

    return 1;
}

MovesList getPieceMovement(KingRelatedSquares *kingRelatedSquares, char piece, unsigned int position, char checkVulnerability, char checked)
{
    char isKingConcerned = isSquareRelatedToKing(kingRelatedSquares, position, 0);

    MovesList moveList;
	moveList.moves = malloc(4 * sizeof(unsigned int));
    moveList.size = 0;

    if ((piece & 0b1000) == 0b1000)
    {
        if ((piece & 0x0E) == PAWN)
        {
            int forward = (((piece & 0x01) * 2) - 1) * BLACKS_FORWARD;
            //printf("1\n");
            unsigned int firstSquareForward = position + forward;
            //printf("2\n");

            tryToAddPositionIfEmpty(kingRelatedSquares, &moveList, isKingConcerned, position, firstSquareForward, piece, ((piece & 0x01) | 0b10), &checkVulnerability, checked);
            //printf("3\n");
            tryToAddPositionIfEnemy(kingRelatedSquares, &moveList, isKingConcerned, position, firstSquareForward - 1, piece, 0b00, &checkVulnerability, checked);
            //printf("4\n");
            tryToAddPositionIfEnemy(kingRelatedSquares, &moveList, isKingConcerned, position, firstSquareForward + 1, piece, 0b01, &checkVulnerability, checked);
            //printf("5\n");

            if ((((position & 0b111000) >> 3) == (3 + ((piece & 0x01)))) && (lastPushedPawn != -1) && ((lastPushedPawn - 1) == (position & 0b111) || (lastPushedPawn + 1) == (position & 0b111)))
                tryToAddPosition(kingRelatedSquares, &moveList, isKingConcerned, position, ((2 + (((piece & 0x01) * 3))) << 3) | lastPushedPawn, piece, EMPTY, &checkVulnerability, checked);

            if (((position & 0b00111000) >> 3) == 1 + ((piece & 0x01) ^ 0x01) * 5)
                tryToAddPositionIfEmpty(kingRelatedSquares, &moveList, isKingConcerned, position, position + (forward * 2), piece, ((piece & 0x01) | 0b10), &checkVulnerability, checked);

            //printf("5\n");
            return moveList;
        }

        unsigned int maxMovesLength = 0;

        // has bishop bit
        if ((piece & 0b10) == 0b10)
        {
			moveList.moves = malloc(2*(BOARD_SIZE-1) * sizeof(unsigned int));
            maxMovesLength += 2*(BOARD_SIZE-1);

            for (char diagonal = 0; diagonal < 4; diagonal++)
            {
                if (isASide(position, diagonal | 0b100))
                    continue;

                for (unsigned int i = 1; i < BOARD_SIZE; i++)
                {
                    int checkedPosition = ((((diagonal & 0b10) >> 1) * ((position + i) + (((diagonal - 2) * 2) - 1) * (i * BOARD_SIZE))) + ((!((diagonal & 0b10) >> 1)) * ((position - i) + ((diagonal * 2) - 1) * (i * BOARD_SIZE))));
                    char returnCode = checkSlidingPieceMovement((diagonal | 0b100), (piece & 0x01), &checkedPosition);

                    if (returnCode ^ 0b11)
                        tryToAddPosition(kingRelatedSquares, &moveList, isKingConcerned, position, checkedPosition, piece, getPieceAt(checkedPosition), &checkVulnerability, checked);

                    if (returnCode & 0b10)
                        break;
                }
            }
        }

        // has rook bit
        if ((piece & 0b100) == 0b100)
        {
            unsigned int* temp = realloc(moveList.moves, sizeof(unsigned int) * (maxMovesLength + 2*(BOARD_SIZE-1)));
			maxMovesLength += 2*(BOARD_SIZE-1);

			if (temp == NULL) {
				printf("Allocation failed");
				exit(0);
			}
			moveList.moves = temp;

            for (char direction = 0; direction < 4; direction++)
            {
                if (isASide(position, direction))
                    continue;

                for (unsigned int i = 1; i < BOARD_SIZE; i++)
                {
					int checkedPosition = ((((direction & 0b10) >> 1) * (position + (((direction - 2) * 2) - 1) * (i * BOARD_SIZE))) + ((!((direction & 0b10) >> 1)) * (position + ((direction * 2) - 1) * BOARD_SIZE)));
                    char returnCode = checkSlidingPieceMovement(direction, (char)(piece & 0x01), &checkedPosition);

                    if (returnCode ^ 0b11)
                        tryToAddPosition(kingRelatedSquares, &moveList, isKingConcerned, position, checkedPosition, piece, getPieceAt(checkedPosition), &checkVulnerability, checked);

                    if (returnCode & 0b10)
                        break;
                }
            }
            return moveList;
        }
    }

    else
    {
        if ((piece & 0b1110) == KNIGHT)
        {
			moveList.moves = malloc(8 * sizeof(unsigned int));

            for (char direction = 0; direction < 4; direction++)
            {
                for (char c = 0; c < 2; c++)
                {
                    int possibleJump = position + BOARD_SIZE * (1 - (2 * ((int)floor(direction / 2)))) * (c % 2 + 1) - (2 - c) * (2 * (direction % 2) - 1);

                    if (knightWentThroughtTheBoard(&position, &possibleJump))
                        continue;
                    tryToAddPositionIfEnemyOrEmpty(kingRelatedSquares, &moveList, isKingConcerned, position, possibleJump, piece, -1, &checkVulnerability, checked);
                }
            }

            return moveList;
        }

        if ((piece & 0b1110) == KING)
        {
			moveList.moves = malloc(8 * sizeof(unsigned int));

            for (char direction = 0; direction < 8; direction++)
            {
                if (isASide(position, direction))
                    continue;

                unsigned int possibleMove = (unsigned int)((((direction & 0b100) >> 3) * (position + (((direction & 0b001) * 2) - 1) * BOARD_SIZE + ((((direction >> 1) & 0x01) * 2) - 1))) +
                                    (!((direction & 0b100) >> 3)) * (position + (((direction & 0b001) * 2) - 1) * (1 + ((BOARD_SIZE-1) * ((direction >> 1) & 0x01)))));

                char pieceAtDestination = getPieceAt(possibleMove);

                if (pieceAtDestination != EMPTY && ((pieceAtDestination & 0x01) == (piece & 0x01)))
                    continue;

                setPiece(EMPTY, position);

                if (isSquareVulnerable(possibleMove, (piece & 0x01) ^ 0b01))
                {
                    setPiece(piece, position);
                    continue;
                }
                setPiece(piece, position);

                moveList.moves[moveList.size] = possibleMove;
                moveList.size++;
            }

            if (canCastle(&piece, &position, 0))
            {
                moveList.moves[moveList.size] =  position - 2;
                moveList.size++;
            }

            if (canCastle(&piece, &position, 1))
            {
                moveList.moves[moveList.size] = position + 2;
                moveList.size++;
            }

            return moveList;
        }
    }

    return moveList;
}
