//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

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


void setNewINode(INode* INodeToSetup) { //set up a new inode (essentially set everything to zero)
    INodeToSetup->mode = 0;
    INodeToSetup->linkCnt = 0;
    INodeToSetup->uid = 0;
    INodeToSetup->gid = 0;
    INodeToSetup->size = 0;

    int i = 0;
    while (i <= NUMBER_OF_INODE_DIRECT_POINTERS && INodeToSetup->iNodePointers[i] != -1) { // set the direct pointers to not defined
        INodeToSetup->iNodePointers[i] = NOT_DEFINED;
        i++;
    }
    INodeToSetup->indirectINodePointer = NOT_DEFINED;

}

typedef struct inodeTable {
    INode table[NUMBER_OF_INODES];
    short usedTable[NUMBER_OF_INODES];

} INodeTable;


short checkIfPosistionInINodeTableIsOpen(INodeTable *table, int index) { //check if posistion in table is open
    if(table->usedTable[index] == FALSE)
        return TRUE;
    else
        return FALSE;
}

short checkIfPosistionInINodeTableIsUsed(INodeTable *table, int index){ // check if posistion in table is used
    if(table->usedTable[index] == TRUE)
        return TRUE;
    else
        return FALSE;
}

int getIndexOfNextOpenSpotInINodeTable(INodeTable *table) { // get the next open spot in the INodeTable
    int i;
    for(i = 0; i < NUMBER_OF_INODES; i++) {
        if(checkIfPosistionInINodeTableIsOpen(table,i) == TRUE)
            return i;
    }
    return NOT_DEFINED; // error no more spots left

}

void setPosistionInInodeTableToOpen(INodeTable *table, int index) { //set spot open in table
    table->usedTable[index] = FALSE;
}

void setPosistionInInodeTableToUsed(INodeTable *table, int index) { //set spot to used in table
    table->usedTable[index] = TRUE;
}

void eraseAllINodeBlockPointerData(INodeTable *theTable, bitmapBlock *thebitMapBlock, int index) { // erase all information in table
    int i;

    for(i=0; i < NUMBER_OF_INODE_DIRECT_POINTERS; i++) {
        if(theTable->table[index].iNodePointers[i] != NOT_DEFINED){ //set pointers to undefined
            void *temporaryData = calloc(1, BLOCK_SIZE);
            write_blocks(DATA_INDICIE+theTable->table[index].iNodePointers[i], 1, temporaryData); //flush to disk
            free(temporaryData);
            thebitMapBlock->block[theTable->table[index].iNodePointers[i]] = FALSE; //free up space in free bit map block
            theTable->table[index].iNodePointers[i] = NOT_DEFINED; // free up space in the INodeTable
        }
    }

    setPosistionInInodeTableToOpen(theTable, index); // set posisition to open
    setNewINode(&theTable->table[index]);
}

short checkIfDataPointerIsEmptyInINode(INodeTable *someINodeTable, int indexInTable, int dataPointerInINode){ //check if the data pointer in a specific INode is empty
    if (someINodeTable->table[indexInTable].iNodePointers[dataPointerInINode] <0){
        return TRUE;
    }
    else
        return FALSE;
}

int checkIfPointerIsInIndirectBlock(int index) { // check if block pointer is in the indirect block
    if (index >= NUMBER_OF_INODE_DIRECT_POINTERS)
        return TRUE;
    else
        return FALSE;
}

int calculateWhichIndirectBlock(int index) { // calculate which indirect block it is in
    if (checkIfPointerIsInIndirectBlock(index) == FALSE)
        return -1;
    else
        return index - NUMBER_OF_INODE_DIRECT_POINTERS;
}

int checkIfPointerToIndirectBlockIsEmpty(INodeTable *someINodeTable, int indexInTable) { //check if the pointer to the indirect block pointer is null
    if(someINodeTable->table[indexInTable].indirectINodePointer < 1) {
        return TRUE;
    }
    else{
        return FALSE;
    }
}

int checkIfIndirectDataPointerIsEmpty(indirectBlock *someIndirectBlock, int indexInTable){ //check if the pointer to disk from the indirect block pointer is null
    if(someIndirectBlock->indirectPointers[indexInTable] < 1 ){
        return TRUE;
    } else {
        return FALSE;
    }
}

int testForStackCorruption(indirectBlock *someIndirectBlock, int indexInTable){ // test for overwrites stack corruption
    if(someIndirectBlock->indirectPointers[indexInTable] > NUMBER_OF_BLOCKS) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

#endif //ASSIGNMENT_3_INODE_H
