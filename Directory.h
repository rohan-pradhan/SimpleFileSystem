//
// Created by Rohan Pradhan on 11/23/16.
//

#ifndef ASSIGNMENT_3_DIRECTORY_H
#define ASSIGNMENT_3_DIRECTORY_H
#define FILE_NAME_LENGTH 16
#define FILE_EXTENSION_LENGTH 3
#define COMBINED_FILE_LENGTH 20
#include "OpenFile.h"

typedef struct directoryfile {
    char fileName[COMBINED_FILE_LENGTH+1];
    int pointerToINode;
} directoryFile;

typedef struct directorycache {
    directoryFile theDirectory[40];
    short fileUsed[40];
    short posistionInDirectory;

} directoryCache;

int validateFileName(char *input){
    int fileNameCounter = 0;
    while(input[fileNameCounter] != '.'){
        fileNameCounter++;
    }
    if (fileNameCounter>16 || fileNameCounter<1) {
        return FALSE;
    }
    else {
        int extensionCounter=fileNameCounter+1;
        while(input[extensionCounter] != '\0'){
            extensionCounter++;
        }
        if(extensionCounter - (fileNameCounter+1) > 3 || extensionCounter - (fileNameCounter+1) < 1)
            return FALSE;
        else {
            return TRUE;
        }

    }
}

int findIndexOfFileInDirectory(directoryCache *theDirectoryCache, char *fileName) {
    int i;
    for(i=0; i< 40; i++) {
        if( strcmp(theDirectoryCache->theDirectory[i].fileName, fileName) == 0) {
            return i;
        }
    }
    return -1;
}

void incrementPosistionInDirectory(directoryCache *theDirectoryCache) {
    theDirectoryCache->posistionInDirectory++;

}

int checkIfPosistionInDirectoryIsUsed(directoryCache *theDirectoryCache, int index) {
    if(theDirectoryCache->fileUsed[index] == TRUE)
        return TRUE;
    else
        return FALSE;
}

int checkIfPosistionIndDirectoryIsOpen(directoryCache *theDirectoryCache, int index) {
    if(theDirectoryCache->fileUsed[index] == FALSE)
        return TRUE;
    else
        return FALSE;
}

int getIndexOfNextOpenSpotInDirectory(directoryCache *theDirectoryCache) {
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    int i =0;
    while((currentIndexOfPointer+i)<=40){
        int adjustedPointer = (currentIndexOfPointer +i) % 40;
        if(checkIfPosistionIndDirectoryIsOpen(theDirectoryCache, adjustedPointer) == TRUE)
            return adjustedPointer;
        else
            i++;
    }
}

void markCurrentPosistionInDirectoryAsOpen(directoryCache *theDirectoryCache) {
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    theDirectoryCache->fileUsed[currentIndexOfPointer] = FALSE;
}

void markCurrentPosistionInDirectoryAsUsed(directoryCache *theDirectoryCache) {
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    theDirectoryCache->fileUsed[currentIndexOfPointer] = TRUE;
}

void markSpecificPosistionInDirectoryAsOpen(directoryCache *theDirectoryCache, int index) {
    int adjustedIndex = index % 40;
    theDirectoryCache->fileUsed[adjustedIndex] = FALSE;

}

void markSpecificPosistionInDirectoryAsUsed(directoryCache *theDirectoryCache, int index) {
    int adjustedIndex = index % 40;
    theDirectoryCache->fileUsed[adjustedIndex] = TRUE;
}




#endif //ASSIGNMENT_3_DIRECTORY_H
