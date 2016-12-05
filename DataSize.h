//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

#ifndef ASSIGNMENT_3_DATASIZE_H
#define ASSIGNMENT_3_DATASIZE_H
#include <stddef.h>
#include "SuperBlock.h"
int calculateNumberOfBlocksNeeded(size_t input) { // calculate how many blocks we will need
   int returnVal;
    returnVal = (input + (BLOCK_SIZE-1))/BLOCK_SIZE;
//    printf("Input Blocks: %d\n", input);
//    printf("Output Blocks: %d\n", returnVal);

    return returnVal;
}

int calculateSizeNeeded(size_t input) { //calculate how much size required in blocks
    int x;
    x = calculateNumberOfBlocksNeeded(input);
    int returnVal;
    returnVal = x * BLOCK_SIZE;
//    printf("Input Size: %d\n", input);
//    printf("Output Size: %d\n", returnVal);
    return returnVal;

}

int calculateWhichBlock(int dataByte) { // calculate which block (not adjusted for indirect)
    return dataByte / BLOCK_SIZE;
}

short checkIfWeAreIncreasingSizeOfFile(INodeTable *someINodeTable, int someWritePointer, int someLength, int someINodePointer){ //check if we are increasing the size of file
    if(someWritePointer + someLength > someINodeTable->table[someINodePointer].size) {
        return TRUE;
    }
    else
        return FALSE;
}

int findCorrespondingDataBlockPointerToDisk(INodeTable *someINodeTable, int someINodeIndex, int someBlockNumber) { //return which data pointer
    if (someBlockNumber < 0) {
        printf("\n SomeBlockNumber is less than 1 \n %d", someBlockNumber);
        return -1;
    }

    if (someBlockNumber > someINodeTable->table[someINodeIndex].size / BLOCK_SIZE) {
        printf("\n SomeBlock Number is too big! \n");
            return -1;
        }

    if (someBlockNumber < NUMBER_OF_INODE_DIRECT_POINTERS) {
        printf("\n value returned: %d", someINodeTable->table[someINodeIndex].iNodePointers[someBlockNumber]);
        return someINodeTable->table[someINodeIndex].iNodePointers[someBlockNumber];
    }
   return -1;
}


//int setNewDataBlock(INodeTable *someINodeTable, int someINodeIndex, int someBlockNumber, bitmapBlock *someBitMapBlock){
//    if (someBlockNumber < 0)
//        return -1;
//    if (someBlockNumber > 24) // check if we have to change later
//        return -1;
//    int tempVar = findFreeBit(someBitMapBlock);
//    uint8_t FirstUsedBitInBlock = ffs(someBitMapBlock->block[tempVar]) - 1;
//    someBitMapBlock->block[tempVar] = someBitMapBlock->block[tempVar] &  ~(1 << FirstUsedBitInBlock);
//    int BlockPointer = tempVar*8 + FirstUsedBitInBlock;
//    if (someBlockNumber < NUMBER_OF_INODE_DIRECT_POINTERS) {
//        someINodeTable->table[someINodeIndex].iNodePointers[someBlockNumber] = BlockPointer;
//    }
//    // HAVE TO ADD INDIRECT STUFF
//    return BlockPointer;
//
//}

int calculateWhereInBlock(int pointer) {
    return pointer % BLOCK_SIZE;
}


#endif //ASSIGNMENT_3_DATASIZE_H
