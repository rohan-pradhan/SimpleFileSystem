//
// Created by Rohan Pradhan on 11/22/16.
//

#ifndef ASSIGNMENT_3_SUPERBLOCK_H
#define ASSIGNMENT_3_SUPERBLOCK_H
#include "INode.h"
#define BLOCK_SIZE 1024
#define NUMBER_OF_BLOCKS 400

typedef struct superblock {
    int magicNumber;
    int blockSize;
    int FSSize;
    int INodeTableLength;
    int rootDirINodePointer;


} superBlock;

#endif //ASSIGNMENT_3_SUPERBLOCK_H
