//
// Created by Rohan Pradhan on 11/22/16.
//

#ifndef ASSIGNMENT_3_INODE_H
#define ASSIGNMENT_3_INODE_H
#include <setjmp.h>
#include "SuperBlock.h"
#define NUMBER_OF_INODE_DIRECT_POINTERS 12


#define NUMBER_OF_INODES 40
#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) ){
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1)
static const int NUMBER_OF_INODE_INDIRECT_POINTERS = BLOCK_SIZE / sizeof(int);
#include "BoolConstants.h"
#include "FreeBitMap.h"
#include "disk_emu.h"

typedef struct inode {
    int mode;
    int linkCnt;
    int uid;
    int gid;
    int size;
    int iNodePointers[NUMBER_OF_INODE_DIRECT_POINTERS];
    int indirectINodePointer;
}INode;

typedef struct indirectblock {
    int indirectPointers[NUMBER_OF_INODE_INDIRECT_POINTERS];
} indirectBlock;



void copyINodeInformation(INode *original, INode *copy) {
    copy->mode = original->mode;
    copy->linkCnt = original->linkCnt;
    copy->uid = original->uid;
    copy->gid = original->gid;
    copy->size = original->size;
    for (int i = 0; i <NUMBER_OF_INODE_DIRECT_POINTERS; i++){
        copy->iNodePointers[i] = original->iNodePointers[i];
    }
    copy->indirectINodePointer = original->indirectINodePointer;
 }


void setNewINode(INode* INodeToSetup) {
    INodeToSetup->mode = 0;
    INodeToSetup->linkCnt = 0;
    INodeToSetup->uid = 0;
    INodeToSetup->gid = 0;
    INodeToSetup->size = 0;

    int i = 0;
    while (i <= NUMBER_OF_INODE_DIRECT_POINTERS && INodeToSetup->iNodePointers[i] != -1) {
        INodeToSetup->iNodePointers[i] = -1;
        i++;
    }
    INodeToSetup->indirectINodePointer = -1;

}

typedef struct inodeTable {
    INode table[NUMBER_OF_INODES];
    short usedTable[NUMBER_OF_INODES];

} INodeTable;


short checkIfPosistionInINodeTableIsOpen(INodeTable *table, int index) {
    if(table->usedTable[index] == FALSE)
        return TRUE;
    else
        return FALSE;
}

short checkIfPosistionInINodeTableIsUsed(INodeTable *table, int index){
    if(table->usedTable[index] == TRUE)
        return TRUE;
    else
        return FALSE;
}

int getIndexOfNextOpenSpotInINodeTable(INodeTable *table) {
    int i;
    for(i = 0; i < NUMBER_OF_INODES; i++) {
        if(checkIfPosistionInINodeTableIsOpen(table,i) == TRUE)
            return i;
    }
    return NOT_DEFINED;

}

void setPosistionInInodeTableToOpen(INodeTable *table, int index) {
    table->usedTable[index] = FALSE;
}

void setPosistionInInodeTableToUsed(INodeTable *table, int index) {
    table->usedTable[index] = TRUE;
}

void eraseAllINodeBlockPointerData(INodeTable *theTable, bitmapBlock *thebitMapBlock, int index) {
    int i;

    for(i=0; i < NUMBER_OF_INODE_DIRECT_POINTERS; i++) {
        if(theTable->table[index].iNodePointers[i] != NOT_DEFINED){
            void *temporaryData = calloc(1, BLOCK_SIZE);
            write_blocks(DATA_INDICIE+theTable->table[index].iNodePointers[i], 1, temporaryData);
            free(temporaryData);
            thebitMapBlock->block[theTable->table[index].iNodePointers[i]] = FALSE;
            theTable->table[index].iNodePointers[i] = NOT_DEFINED;
        }
    }

//   // INodeIndirectPointer *theIndirectPointer = NULL;
//    //theIndirectPointer = (1, sizeof(INodeIndirectPointer));
//    INodeIndirectPointer theIndirectPointer = {};
//    //int x = theTable->table[index].indirectINodePointer;
//    //printf("Value of indirect ptr: %d", x);
//    read_blocks(DATA_INDICIE+theTable->table[index].indirectINodePointer, 1, &theIndirectPointer);
//   // memcpy(theIndirectPointer, theTable->table[index].indirectINodePointer, sizeof(INodeIndirectPointer));
//
//    for(i=0; i< NUMBER_OF_INODE_DIRECT_POINTERS; i++) {
//
//    }

    setPosistionInInodeTableToOpen(theTable, index);
    setNewINode(&theTable->table[index]);
}

short checkIfDataPointerIsEmptyInINode(INodeTable *someINodeTable, int indexInTable, int dataPointerInINode){
    if (someINodeTable->table[indexInTable].iNodePointers[dataPointerInINode] <0){
        return TRUE;
    }
    else
        return FALSE;
}

int checkIfPointerIsInIndirectBlock(int index) {
    if (index >= NUMBER_OF_INODE_DIRECT_POINTERS)
        return TRUE;
    else
        return FALSE;
}

int calculateWhichIndirectBlock(int index) {
    if (checkIfPointerIsInIndirectBlock(index) == FALSE)
        return -1;
    else
        return index - NUMBER_OF_INODE_DIRECT_POINTERS;
}

int checkIfPointerToIndirectBlockIsEmpty(INodeTable *someINodeTable, int indexInTable) {
    if(someINodeTable->table[indexInTable].indirectINodePointer < 1) {
        return TRUE;
    }
    else{
        return FALSE;
    }
}

int checkIfIndirectDataPointerIsEmpty(indirectBlock *someIndirectBlock, int indexInTable){
    if(someIndirectBlock->indirectPointers[indexInTable] < 1 ){
        return TRUE;
    } else {
        return FALSE;
    }
}

int testForStackCorruption(indirectBlock *someIndirectBlock, int indexInTable){
    if(someIndirectBlock->indirectPointers[indexInTable] > NUMBER_OF_BLOCKS) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

#endif //ASSIGNMENT_3_INODE_H
