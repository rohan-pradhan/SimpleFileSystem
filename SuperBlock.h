//
// Created by Rohan Pradhan on 11/22/16.
//

#ifndef ASSIGNMENT_3_SUPERBLOCK_H
#define ASSIGNMENT_3_SUPERBLOCK_H
#include "INode.h"
#define BLOCK_SIZE 1024
#define NUMBER_OF_BLOCKS 4000
#define SB_INDICIE 0
#define INODE_INDICIE 1
#define DIRECTORY_INDICIE 40
#define FREE_BIT_MAP_INDICIE 50
#define DATA_INDICIE 60

typedef struct superblock {
    int magicNumber;
    int blockSize;
    int FSSize;
    int INodeTableLength;
    int rootDirINodePointer;


} superBlock;

#endif //ASSIGNMENT_3_SUPERBLOCK_H
