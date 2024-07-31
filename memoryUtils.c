
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void store6BitsValue(char *bytes, unsigned char index, char value)
{
    char valueToStore = (value << 2) & 0xFC;
    unsigned char ptrOnFirstByte = (index * 6) % 8;
    unsigned char firstByteID = (int)floor((index * 6) / 8);

    if (ptrOnFirstByte == 0)
        bytes[firstByteID] |= valueToStore;
    else
    {
        bytes[firstByteID] |= (((valueToStore >> ptrOnFirstByte) & ((unsigned char)(pow(2, 8 - ptrOnFirstByte) - 1))));
        bytes[firstByteID + 1] |= (valueToStore << (8 - ptrOnFirstByte));
    }
}

char retrieve6BitsValue(char *bytes, unsigned char index)
{
    if (((index * 6) % 8) == 0)
        return ((bytes[(int)floor((index * 6) / 8)] >> 2) & 0x3F);
    else
    {
        unsigned char ptrOnFirstByte = (index * 6) % 8;
        unsigned char firstByteIndex = (int)floor((index * 6) / 8);

        return ((((bytes[firstByteIndex] << ptrOnFirstByte) | // Retrieving value from the first byte and offsetting it
                  (((bytes[firstByteIndex + 1] >> (8 - (ptrOnFirstByte))) & (unsigned char)pow(2, ptrOnFirstByte) - 1))) >>
                 2) &
                0x3F); // Retrieving value from the second byte, offsetting it, remove the values from this byte that are not part of the value, offsetting it again and removing the last 2 bits
    }
}

void store3BitsValue(char *bytes, unsigned char index, char value)
{
    char valueToStore = (value << 5) & 0b11100000;
    unsigned char ptrOnFirstByte = (index * 3) % 8;
    unsigned char firstByteID = (int)floor((index * 3) / 8);

    if (ptrOnFirstByte == 0)
    {
        bytes[firstByteID] &= 0b00011111;
        bytes[firstByteID] |= valueToStore;
    }
    else
    {
        bytes[firstByteID] &= ~(0b11100000 >> ptrOnFirstByte);
        bytes[firstByteID] |= (((valueToStore >> ptrOnFirstByte) & ((unsigned char)(pow(2, 8 - ptrOnFirstByte) - 1))));
        if (ptrOnFirstByte >= 6)
        {
            bytes[firstByteID + 1] &= ~(0b11100000 << (8 - ptrOnFirstByte));
            bytes[firstByteID + 1] |= (valueToStore << (8 - ptrOnFirstByte));
        }
    }
}

char retrieve3BitsValue(char *bytes, unsigned char index)
{
    if (((index * 3) % 8) == 0)
    {
        return ((bytes[(int)floor((index * 3) / 8)] >> 5) & 0b0111);
    }
    else
    {
        unsigned char ptrOnFirstByte = (index * 3) % 8;
        unsigned char firstByteIndex = (int)floor((index * 3) / 8);

        return ((((bytes[firstByteIndex] << ptrOnFirstByte) | // Retrieving value from the first byte and offsetting it
                  (((bytes[firstByteIndex + 1] >> (8 - (ptrOnFirstByte))) & (unsigned char)pow(2, ptrOnFirstByte) - 1))) >>
                 5) &
                0b00000111); // Retrieving value from the second byte, offsetting it, remove the values from this byte that are not part of the value, offsetting it again and removing the last 2 bits
    }
}

char *allocateBytesForPositions(unsigned char positionsNumber)
{
    unsigned char bytesSize = (positionsNumber * 6 + 7) / 8;
    char *bytes = malloc(bytesSize);

    if (bytes == NULL) {
        printf("Error while allocating memory for bytes, exiting program\n");
        exit(1);
    }

    for (int i = 0; i < bytesSize; i++)
        bytes[i] = 0;

    return bytes;
}

char *reAllocateBytesForPositions(char *oldBytes, unsigned char oldPositionsSize, unsigned char positionsNumberToAdd)
{
    unsigned char newBytesSize = ((oldPositionsSize + positionsNumberToAdd) * 6 + 7) / 8;
    char *bytes = realloc(oldBytes, newBytesSize);

    for (char i = ((oldPositionsSize * 6) + 7) / 8; i < newBytesSize; i++)
        bytes[i] = 0;

    return bytes;
}