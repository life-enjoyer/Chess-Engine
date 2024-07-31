#include "UIManager.h"

#include "consts.h"

#ifdef _WIN32
#include <io.h>
#define access _access

#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <malloc.h>

extern char board[BOARD_MEMORY_SIZE];

void getPieceName(char* binary, char* name) {
    if ((*binary & 0x01) == 0x01) {
        *binary = *binary - 1;
        *(name + 1) = 'b'; //B
    } else {
        *(name + 1) = 'w'; //W
    }

    switch (*binary) {
        case EMPTY:
            *name = '*'; //*
            *(name + 1) = '*'; //*
            return;
        case PAWN:
            *name = 'p'; //p
            break;
        case KNIGHT:
            *name = 'k'; //k
            break;
        case BISHOP:
            *name = 'b'; //b
            break;
        case ROOK:
            *name = 'R'; //r
            break;
        case QUEEN:
            *name = 'Q'; //q
            break;
        case KING:
            *name = 'K'; //K
            break;
		case OOB:
			*name = 'O';
			break;
		default:
			*name = 'E';
			break;
    }
}

void writeSelectionCharacter(unsigned int* possibleMoves, unsigned int possibleMovesLenght, unsigned char* selectedPiece, unsigned int position) {
    if (position == *selectedPiece) {
        write(1, ">", 1);
        return;
    }

    for (unsigned int c = 0; c < possibleMovesLenght; c++) {
        if (possibleMoves[c] == position) {
            write(1, "x", 1);
            return;
        }
    }

    write(1, " ", 1);
}

void printBoard(MovesList *moves, unsigned char* selectedPiece) {
    printf("Board:\n");
    if (selectedPiece == NULL) {
        for (unsigned int line = 0; line < BOARD_SIZE; line++) {
            for (unsigned int column = 0; column < BOARD_SIZE/2; column++) {
				char plot = board[(line * (BOARD_SIZE/2)) + column];

                char firstPiece = ((plot >> 4) & 0x0F);
                char secondPiece = (plot & 0x0F);

                char name[2] = {};

                getPieceName(&firstPiece, &name[0]);
                write(1, &name, 2);
                write(1, ", ", 2);
                getPieceName(&secondPiece, &name[0]);
                write(1, &name, 2);
                write(1, ", ", 2);

            }
            write(1, "\n", 1);
        }
    } else {
		unsigned int* boardPossibleMoves = malloc(moves->size * sizeof(unsigned int));
        
        unsigned int c;
        for (c = 0; c < moves->size; c++) {
            boardPossibleMoves[c] = moves->moves[c];
        }

        for (unsigned int line = 0; line < BOARD_SIZE; line++) {
            for (unsigned int column = 0; column < BOARD_SIZE/2; column++) {
                unsigned int plotIndex = (line * BOARD_SIZE/2) + column;
                unsigned char plot = board[plotIndex];

                char firstPiece = plot >> 4;
                char secondPiece = plot & 0x0F;

                char name[2] = {};

                writeSelectionCharacter(boardPossibleMoves, moves->size, selectedPiece, (line * BOARD_SIZE) + (column * 2));
                getPieceName(&firstPiece, &name[0]);
                write(1, &name, 2);
                write(1, ", ", 2);
                writeSelectionCharacter(boardPossibleMoves, moves->size, selectedPiece, (line * BOARD_SIZE) + (column * 2) + 1);
                getPieceName(&secondPiece, &name[0]);
                write(1, &name, 2);
                write(1, ", ", 2);

            }
            write(1, "\n", 1);
        }

		free(boardPossibleMoves);
    }
}

void printBinary(char c) {
	printf(" ");
	for (int i = 7; i >= 0; i--) {
		char bit =  (char)(((c >> i) & 0x01) + '0');
		printf("%c", bit);
	}
	printf("\n");
}
