#include "positionManager.h"

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "consts.h"

extern char board[BOARD_MEMORY_SIZE]; 	// each square is 4 bits long, the first three represent the piece according to the constants
						// defined in "positionManager.h" and the last bit is the color, 1 for black and 0 for white

char getPieceAt(unsigned int position)
{
    if (position > (BOARD_SIZE*BOARD_SIZE)-1)
        return OOB;

    return (char)((board[(unsigned int)(floor((double)(position) / 2))] //retrieving the byte
     >> ((position % 2) - 1) * -4) & 0x0F); //offsetting by 4 it if the position is even
}

void setPiece(char piece, unsigned int position)
{
    //todo: make that branchless
    if (position % 2 == 0)
    {
        board[position / 2] &= 0x0F;
        board[position / 2] |= (piece & 0x0F) << 4;
    }
    else
    {
		board[(unsigned int)floor((double)position / 2)] &= 0xF0;
		board[(unsigned int)floor((double)position / 2)] |= (piece & 0x0F);
	}
}