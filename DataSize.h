//
// Created by Rohan Pradhan on 11/23/16.
//

#ifndef ASSIGNMENT_3_DATASIZE_H
#define ASSIGNMENT_3_DATASIZE_H
#include <stddef.h>
#include "SuperBlock.h"
int calculateNumberOfBlocksNeeded(size_t input) {
   int returnVal;
    returnVal = (input + (BLOCK_SIZE-1))/BLOCK_SIZE;
//    printf("Input Blocks: %d\n", input);
//    printf("Output Blocks: %d\n", returnVal);

    return returnVal;
}

int calculateSizeNeeded(size_t input) {
    int x;
    x = calculateNumberOfBlocksNeeded(input);
    int returnVal;
    returnVal = x * BLOCK_SIZE;
//    printf("Input Size: %d\n", input);
//    printf("Output Size: %d\n", returnVal);
    return returnVal;

}

int calculateWhichBlock(int dataByte) {
    return dataByte / BLOCK_SIZE;
}


#endif //ASSIGNMENT_3_DATASIZE_H
