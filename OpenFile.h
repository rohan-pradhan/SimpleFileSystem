//
// Created by Rohan Pradhan on 11/22/16.
//

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

int getNextOpenSpotInFileDescriptorTable(openFileTable * fdTable){

    for (int i =0; i<MAX_NUMBER_OF_OPEN_FILES; i++){
        if (fdTable->slotInformationTable[i] == NOT_IN_USE)
            return i;
    }
    return -1; // Means there are no free file descriptors in the table

}

int checkIfSlotIsInUse(openFileTable *fdTable, int tableIndex) {
    if (fdTable->slotInformationTable[tableIndex] == IN_USE){
        return TRUE;
    }
    else {
        return FALSE;
    }
}

int SetSlotToInUse(openFileTable *fdTable, int index) {
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return -1; //failed
    else
        fdTable->slotInformationTable[index]=IN_USE;
        return 0; //successful
}

int SetSlotToNotInUse(openFileTable *fdTable, int index){
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return 1; //failed
    else
        fdTable->table[index].INode = -1;
        fdTable->table[index].readPointer = -1;
        fdTable->table[index].writePointer = -1;
        fdTable->slotInformationTable[index] = NOT_IN_USE;
        return 0;

}

#endif //ASSIGNMENT_3_OPENFILE_H
