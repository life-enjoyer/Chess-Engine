#define EMPTY 0x00 //0000
#define KNIGHT 0x02 //0010
#define OOB 0x04 //0100
#define KING 0x06 //0110

#define PAWN 0x08 //1000
#define BISHOP 0x0A //1010
#define ROOK 0x0C //1100
#define QUEEN 0x0E //1110

typedef struct Coordinates
{
  unsigned int x;
  unsigned int y;
} Coordinates;

char getPieceAt(unsigned int positionIndex);

void setPiece(char piece, unsigned int positionIndex);
