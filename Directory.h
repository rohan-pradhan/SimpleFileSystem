//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

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
    int fileUsed[40];
    int posistionInDirectory;

} directoryCache;

int validateFileName(char *input){ //validates the filename to strucutre specified
    int fileNameCounter = 0;
    while(input[fileNameCounter] != '.'){
        fileNameCounter++;
    }
    if (fileNameCounter>16 || fileNameCounter<1) { // checks if we have 16 or less chars before period
        return FALSE; // error
    }
    else {
        int extensionCounter=fileNameCounter+1;
        while(input[extensionCounter] != '\0'){
            extensionCounter++;
        }
        if(extensionCounter - (fileNameCounter+1) > 3 || extensionCounter - (fileNameCounter+1) < 1) // checks if we only have up to 3 chars after the period
            return FALSE; // error
        else {
            return TRUE;
        }

    }
}

int findIndexOfFileInDirectory(directoryCache *theDirectoryCache, char *fileName) { //loops through directory to find the index of file
    int i;
    for(i=0; i< 40; i++) {
        if( strcmp(theDirectoryCache->theDirectory[i].fileName, fileName) == 0) {
            return i; //success
        }
    }
    return -1; // file not found  error
}

void incrementPosistionInDirectory(directoryCache *theDirectoryCache) { //increment where we are in the directory (for get next file)
    theDirectoryCache->posistionInDirectory++;

}

int checkIfPosistionInDirectoryIsUsed(directoryCache *theDirectoryCache, int index) { //check if the spot in the directory is actually used
    if(theDirectoryCache->fileUsed[index] == TRUE)
        return TRUE;
    else
        return FALSE;
}

int checkIfPosistionIndDirectoryIsOpen(directoryCache *theDirectoryCache, int index) { // check if the spot in the directory is open for use
    if(theDirectoryCache->fileUsed[index] == FALSE)
        return TRUE;
    else
        return FALSE;
}

int getIndexOfNextOpenSpotInDirectory(directoryCache *theDirectoryCache) { // find the next open spot in the directory
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    int i =0;
    while((currentIndexOfPointer+i)<=40){ // loop through the directory
        int adjustedPointer = (currentIndexOfPointer +i) % 40;
        if(checkIfPosistionIndDirectoryIsOpen(theDirectoryCache, adjustedPointer) == TRUE)
            return adjustedPointer; // success
        else
            i++;
    }
    return -1; // no spots left
}

void markCurrentPosistionInDirectoryAsOpen(directoryCache *theDirectoryCache) { //mark the current posistion in directory as open
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    theDirectoryCache->fileUsed[currentIndexOfPointer] = FALSE;
}

void markCurrentPosistionInDirectoryAsUsed(directoryCache *theDirectoryCache) { // mark the current posistion in directory as used
    int currentIndexOfPointer = theDirectoryCache->posistionInDirectory;
    theDirectoryCache->fileUsed[currentIndexOfPointer] = TRUE;
}

void markSpecificPosistionInDirectoryAsOpen(directoryCache *theDirectoryCache, int index) { // mark a specific posistion in the directory as open
    int adjustedIndex = index % 40;
    theDirectoryCache->fileUsed[adjustedIndex] = FALSE;

}

void markSpecificPosistionInDirectoryAsUsed(directoryCache *theDirectoryCache, int index) { // mark a specific posistion in the directory as used
    int adjustedIndex = index % 40;
    theDirectoryCache->fileUsed[adjustedIndex] = TRUE;
}




#endif //ASSIGNMENT_3_DIRECTORY_H
