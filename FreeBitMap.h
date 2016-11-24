//
// Created by Rohan Pradhan on 11/23/16.
//

#ifndef ASSIGNMENT_3_FREEBITMAP_H
#define ASSIGNMENT_3_FREEBITMAP_H
#include "SuperBlock.h"
static const int NUMBER_OF_FREE_BLOCKS = NUMBER_OF_BLOCKS - DATA_INDICIE;

typedef struct bitmapblock {
    short block[NUMBER_OF_FREE_BLOCKS];
   // short blockInUse;

} bitmapBlock;

//setToInUse(index, *bitmapblock){
//    bitmapBlock[index] = 1
//}
//
//
//int

#endif //ASSIGNMENT_3_FREEBITMAP_H
