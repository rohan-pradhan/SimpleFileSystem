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
        printf("%d\n", i);
        printf("fdtable: kk %d\n", fdTable);

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

int checkIfSlotIsNotInUse(openFileTable *fdTable, int tableIndex) {
    if (fdTable->slotInformationTable[tableIndex] == NOT_IN_USE){
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void SetSlotInOpenFileTableToInUse(openFileTable *fdTable, int index) {
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return; //failed
    else
        fdTable->slotInformationTable[index]=IN_USE;
         //successful
}

void SetSlotInOpenFileTableToNotInUse(openFileTable *fdTable, int index){
    if (index > MAX_NUMBER_OF_OPEN_FILES || index < 0)
        return;
    else {
        fdTable->table[index].INode = NOT_DEFINED;
        fdTable->table[index].readPointer = NOT_DEFINED;
        fdTable->table[index].writePointer = NOT_DEFINED;
        fdTable->slotInformationTable[index] = NOT_IN_USE; }
      //  return 0;


}

int checkIfINodeIsInOpenFileTable(openFileTable *fdTable, int someINode){
    int i;
    for(i=0; i<MAX_NUMBER_OF_OPEN_FILES; i++){
        if(fdTable->table[i].INode == someINode)
            return TRUE;
    }
    return FALSE;
}

int checkIfFileIDIsOpen(openFileTable *fdTable, int fileID) {
    if (fdTable->slotInformationTable[fileID] == 1) {
        return  TRUE;
    } else {
        return FALSE;
    }
}

//int returnIndexOfINodeFromOpenFileTable(openFileTable fdTable, int someINode) {
//    int i;
//    for(i=0; i<MAX_NUMBER_OF_OPEN_FILES; i++){
////        printf("The someInode is: %d\n", someINode);
////        printf("The Inode in table is: %d\n", fdTable.table[i].INode);
//        //int testVal = fdTable.table[i].INode;
//        if(fdTable.table[i].INode == someINode); {
//            //printf("value of testval = %d and value of someInode = %d\n",testVal, someINode);
//            //printf("Going to return now!\n");
//            return i;
//        }
//    }
//    return -1;
//}


int getIndexOfOpenFileTableForINode(openFileTable table, int iNodeIndex) {
    int i;
    for (i = 0; i < MAX_NUMBER_OF_OPEN_FILES; i++) {
        if (table.table[i].INode == iNodeIndex) {
            return i;
        }
    }

    // Error
    return -1;
}

void removeDataFromSlotInOpenFileTable(openFileTable *fdTable, int index) {
    fdTable->table[index].INode = NOT_DEFINED;
    fdTable->table[index].readPointer = NOT_DEFINED;
    fdTable->table[index].writePointer = NOT_DEFINED;
}

#endif //ASSIGNMENT_3_OPENFILE_H
