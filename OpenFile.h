//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

#include "INode.h"

#ifndef ASSIGNMENT_3_OPENFILE_H
#define ASSIGNMENT_3_OPENFILE_H

#define MAX_NUMBER_OF_OPEN_FILES 20
#define IN_USE 1
#define NOT_IN_USE 0
#define TRUE 1
#define FALSE 0

typedef struct openfile{
    int INode;
    int readPointer;
    int writePointer;
} openFile;

typedef struct openfiletable{
    openFile table[MAX_NUMBER_OF_OPEN_FILES];
    int slotInformationTable[MAX_NUMBER_OF_OPEN_FILES];

} openFileTable;

int getNextOpenSpotInFileDescriptorTable(openFileTable * fdTable){ // find the next spot in the open file table

    for (int i =0; i<MAX_NUMBER_OF_OPEN_FILES; i++){
        printf("%d\n", i);

        if (fdTable->slotInformationTable[i] == NOT_IN_USE)

            return i;
    }
    return -1; // Means there are no free file descriptors in the table error

}

int checkIfSlotIsInUse(openFileTable *fdTable, int tableIndex) { //check if the slot is in use
    if (fdTable->slotInformationTable[tableIndex] == IN_USE){
        return TRUE;
    }
    else {
        return FALSE;
    }
}

int checkIfSlotIsNotInUse(openFileTable *fdTable, int tableIndex) { // check if the slot is not in use
    if (fdTable->slotInformationTable[tableIndex] == NOT_IN_USE){
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void SetSlotInOpenFileTableToInUse(openFileTable *fdTable, int index) { //set a certain slot to in use
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return; //failed
    else
        fdTable->slotInformationTable[index]=IN_USE;
         //successful
}

void SetSlotInOpenFileTableToNotInUse(openFileTable *fdTable, int index){ // set a certain slot to not in use
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return;
    else {
        fdTable->table[index].INode = NOT_DEFINED;
        fdTable->table[index].readPointer = NOT_DEFINED;
        fdTable->table[index].writePointer = NOT_DEFINED;
        fdTable->slotInformationTable[index] = NOT_IN_USE; }
      //  return 0;


}

int checkIfINodeIsInOpenFileTable(openFileTable *fdTable, int someINode){ // check if an INode is actually in the open file table
    int i;
    for(i=0; i<MAX_NUMBER_OF_OPEN_FILES; i++){
        if(fdTable->table[i].INode == someINode)
            return TRUE;
    }
    return FALSE;
}

int checkIfFileIDIsOpen(openFileTable *fdTable, int fileID) { // check if a certain file ID is actually open
    if (fdTable->slotInformationTable[fileID] == 1) {
        return  TRUE;
    } else {
        return FALSE;
    }
}


int getIndexOfOpenFileTableForINode(openFileTable table, int iNodeIndex) { //return the Index in the open file table for a certain INode
    int i;
    for (i = 0; i < MAX_NUMBER_OF_OPEN_FILES; i++) {
        if (table.table[i].INode == iNodeIndex) {
            return i;
        }
    }

    // Error
    return -1;
}

void removeDataFromSlotInOpenFileTable(openFileTable *fdTable, int index) { //remove information from the open file table (for remove)
    fdTable->table[index].INode = NOT_DEFINED;
    fdTable->table[index].readPointer = NOT_DEFINED;
    fdTable->table[index].writePointer = NOT_DEFINED;
}

#endif //ASSIGNMENT_3_OPENFILE_H
