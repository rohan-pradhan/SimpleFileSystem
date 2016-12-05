//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

#ifndef ASSIGNMENT_3_FREEBITMAP_H
#define ASSIGNMENT_3_FREEBITMAP_H
#include "SuperBlock.h"
#define NOT_IN_USE 0
#define IN_USE 1
static const int NUMBER_OF_FREE_BLOCKS = NUMBER_OF_BLOCKS - DATA_INDICIE;

typedef struct bitmapblock {
    short block[NUMBER_OF_FREE_BLOCKS];


} bitmapBlock;

int findFreeBit(bitmapBlock *someBitMapBlock) { // find the next free bit in the free bit map block
    int i =0;

    while(i < NUMBER_OF_FREE_BLOCKS)
    {
        if (someBitMapBlock->block[i] == NOT_IN_USE)
            return i;
        else
            i++;
    }
    return -1; // no spots left




}


#endif //ASSIGNMENT_3_FREEBITMAP_H
