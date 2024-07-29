void store6BitsValue(char *bytes, unsigned char index, char value);

char retrieve6BitsValue(char *bytes, unsigned char index);

void store3BitsValue(char *bytes, unsigned char index, char value);

char retrieve3BitsValue(char *bytes, unsigned char index);

unsigned int *allocateBytesForPositions(unsigned int positionsNumber);

char *reAllocateBytesForPositions(char *oldBytes, unsigned char oldPositionsSize, unsigned char positionsNumberToAdd);