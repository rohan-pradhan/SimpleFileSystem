#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "sfs_api.h"
#include "OpenFile.h"
#include "disk_emu.h"
#include "SuperBlock.h"
#include "DataSize.h"
#include "Directory.h"
#include "FreeBitMap.h"
#include "BoolConstants.h"


#define FS "RohanPradhan.fs"

superBlock theSuperBlock = {};
INodeTable *theINodeTable = NULL;
directoryCache *theDirectoryCache = NULL;
openFileTable *theOpenFileTable = NULL;
bitmapBlock *theBitMapBlock = NULL;


void mksfs(int fresh){
    theOpenFileTable = calloc(1,sizeof(openFileTable));
  if (fresh ==1){
        remove(FS);
      init_fresh_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS);
      theSuperBlock.magicNumber= rand();
      theSuperBlock.blockSize = BLOCK_SIZE;
      theSuperBlock.INodeTableLength= NUMBER_OF_INODES;
      theSuperBlock.FSSize = NUMBER_OF_BLOCKS;
      theSuperBlock.rootDirINodePointer = 0;
      write_blocks(SB_INDICIE, 1, &theSuperBlock);

     //
      if(theINodeTable == NULL) {
          theINodeTable=calloc(1, sizeof(INodeTable));
      }
//      theINodeTable = malloc(sizeof(INodeTable));
//      memset(theINodeTable, '\0', sizeof(INodeTable));
      void *temporaryData;

      temporaryData=calloc(1,calculateSizeNeeded(sizeof(INodeTable)));
//      void *temporaryData = malloc((size_t) calculateSizeNeeded(sizeof(INodeTable)));
//      memset(temporaryData, '\0', (size_t) calculateSizeNeeded(sizeof(INodeTable)) );

//      printf("%d\n", sizeof(temporaryData));
//      printf("%d\n", sizeof(theINodeTable));

      memcpy(temporaryData, theINodeTable, sizeof(INodeTable));
      write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData);
      free(temporaryData);

      if (theDirectoryCache == NULL) {
          theDirectoryCache = calloc(1, sizeof(directoryCache));
      }

      temporaryData = calloc(1,calculateSizeNeeded((sizeof(directoryCache))));
      memcpy(temporaryData, theDirectoryCache, sizeof(directoryCache));
      write_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData);
      free(temporaryData);

      if (theBitMapBlock == NULL) {
          theBitMapBlock = calloc(1, sizeof(bitmapBlock));
      }

      temporaryData = calloc(1,calculateSizeNeeded(sizeof(bitmapBlock)));
      memcpy(temporaryData, theBitMapBlock, sizeof(bitmapBlock));
      write_blocks(DATA_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData);
      free(temporaryData);


//      char *test="Feeeeeeeeeeeeeed.txt";
//      int testVar = validateFileName(test);
//      printf("%d\n", testVar);


  }
    else
      init_fresh_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    read_blocks(SB_INDICIE,1,&theSuperBlock);

    if(theINodeTable == NULL) {
        theINodeTable=calloc(1, sizeof(INodeTable));
    }
    void *temporaryData;
    temporaryData = calloc(1,calculateSizeNeeded(sizeof(INodeTable)));
    read_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData);
    memcpy(theINodeTable,temporaryData, sizeof(INodeTable));
    free(temporaryData);

    if (theDirectoryCache == NULL) {
        theDirectoryCache = calloc(1, sizeof(directoryCache));
    }
    temporaryData = calloc(1,calculateSizeNeeded(sizeof(directoryCache)));
    read_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData);
    memcpy(theDirectoryCache, temporaryData, sizeof(directoryCache));
    free(temporaryData);

    if (theBitMapBlock == NULL) {
        theBitMapBlock = calloc(1, sizeof(bitmapBlock));
    }
    temporaryData = calloc(1,calculateSizeNeeded(sizeof(bitmapBlock)));
    read_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded(sizeof(bitmapBlock)), temporaryData);
    memcpy(theDirectoryCache, temporaryData, sizeof(bitmapBlock));
    free(temporaryData);



//    printf("Old disk1!!!");
//    printf("%d", theSuperBlock.magicNumber);

}
int sfs_get_next_file_name(char *fname){
    int indexOfCurrentPosistion = theDirectoryCache->posistionInDirectory;
    int i =0;
    while(indexOfCurrentPosistion+i < 40){
        int adjustedIndex = indexOfCurrentPosistion+i;
//        printf("Loop Pos. in directory: %d \n", theDirectoryCache->posistionInDirectory);
//        printf("Value of I: %d\n", i);
        if (checkIfPosistionInDirectoryIsUsed(theDirectoryCache,adjustedIndex)) {
            theDirectoryCache->posistionInDirectory++;
            strcpy(fname, theDirectoryCache->theDirectory[adjustedIndex].fileName);
            return 1;

        }
        else
            i++;
    }
    theDirectoryCache->posistionInDirectory = 0;
    printf("Final Pos. in directory: %d \n", theDirectoryCache->posistionInDirectory);
    return 0;

//    int index;
//    for (index = theDirectoryCache->posistionInDirectory; index < 40; index++) {
//        if (checkIfPosistionInDirectoryIsUsed(theDirectoryCache, index) == TRUE) {
//            // Copy the
//            strcpy(fname, theDirectoryCache->theDirectory[index].fileName);
//            // Increase the index for next time
//            theDirectoryCache->posistionInDirectory++;
//            return 1;
//        }
//    }
//
//    // We're done
//    theDirectoryCache->posistionInDirectory = 0;
//    return 0;


}
int sfs_get_file_size(char* path){
    if (validateFileName(path) == TRUE) {
        for (int i = 0; i < 40; i++) {
            if (strcmp(theDirectoryCache->theDirectory[i].fileName, path) == 0) {
                int INodeOfMatchingFile = theDirectoryCache->theDirectory[i].pointerToINode;
                int sizeToReturn = theINodeTable->table[INodeOfMatchingFile].size;
                return sizeToReturn;
            }
        }
        return -1; //ERROR
    }
    else {
        return -1;  // ERROR

    }

}
int sfs_fopen(char *name){
    if (validateFileName(name) == FALSE) {
        return -1;
    }
    else {
        int indexOfFile = findIndexOfFileInDirectory(theDirectoryCache, name);
        if (indexOfFile == NOT_DEFINED) {
            int indexOfNextOpenSpotInINodeTable = getIndexOfNextOpenSpotInINodeTable(theINodeTable);
            int indexOfNextOpenSpotInFileDescriptorTable = getNextOpenSpotInFileDescriptorTable(theOpenFileTable);
            int indexOfNextOpenSpotInDirectoryCache = getIndexOfNextOpenSpotInDirectory(theDirectoryCache);
            //  printf("\n INDEX OF DIRCACHE :%d\n", indexOfNextOpenSpotInDirectoryCache);
            if (indexOfNextOpenSpotInFileDescriptorTable == -1 || indexOfNextOpenSpotInINodeTable == -1 ||
                indexOfNextOpenSpotInDirectoryCache == -1) {
                return -1; // no open spaces in one of the tables
            } else {
                markSpecificPosistionInDirectoryAsUsed(theDirectoryCache, indexOfNextOpenSpotInDirectoryCache);
//                char* testName = calloc(1, sizeof(name)*strlen(name));
//                memcpy(testName, name,sizeof(name)*strlen(name));
//                printf("\n the name is: %s", testName);
                memcpy(theDirectoryCache->theDirectory[indexOfNextOpenSpotInDirectoryCache].fileName, name,
                       sizeof(name) * strlen(name));
                theDirectoryCache->theDirectory[indexOfNextOpenSpotInDirectoryCache].pointerToINode = NOT_DEFINED;
                printf("\n the name copied into directory cache is: %s \n",
                       theDirectoryCache->theDirectory[indexOfNextOpenSpotInDirectoryCache].fileName);

                printf("\n the name copied into inode cache is: %d \n",
                       theDirectoryCache->theDirectory[indexOfNextOpenSpotInDirectoryCache].pointerToINode);
                indexOfFile = indexOfNextOpenSpotInDirectoryCache;

            }
        }
            int thePointerToInode = theDirectoryCache->theDirectory[indexOfFile].pointerToINode;
            if(thePointerToInode == NOT_DEFINED) {
                if (getIndexOfNextOpenSpotInINodeTable(theINodeTable) == NOT_DEFINED)
                    return NOT_DEFINED;
                else {
                    thePointerToInode = getIndexOfNextOpenSpotInINodeTable(theINodeTable);
                    setPosistionInInodeTableToUsed(theINodeTable, thePointerToInode);
                    setNewINode(&theINodeTable->table[thePointerToInode]);
                    theDirectoryCache->theDirectory[indexOfFile].pointerToINode = thePointerToInode;

                }
            }
            int indexInOpenFileDescriptorTable;
                if(checkIfINodeIsInOpenFileTable(theOpenFileTable,thePointerToInode) == FALSE) {
                    indexInOpenFileDescriptorTable = getNextOpenSpotInFileDescriptorTable(theOpenFileTable);
                    SetSlotInOpenFileTableToInUse(theOpenFileTable, indexInOpenFileDescriptorTable);
                    printf("Slot closed in FTable");
                    theOpenFileTable->table[indexInOpenFileDescriptorTable].INode = thePointerToInode;
                    theOpenFileTable->table[indexInOpenFileDescriptorTable].readPointer = 0;
                    theOpenFileTable->table[indexInOpenFileDescriptorTable].writePointer = theINodeTable->table[thePointerToInode].size;

                }
                else {
                    indexInOpenFileDescriptorTable = returnIndexOfINodeFromOpenFileTable(theOpenFileTable, thePointerToInode);
                    SetSlotInOpenFileTableToInUse(theOpenFileTable, indexInOpenFileDescriptorTable);
                    printf("Slot closed in FTable");
                }
            return indexInOpenFileDescriptorTable;
            }







  return 0;
}
int sfs_fclose(int fileID){
    if (fileID < 0 || fileID > MAX_NUMBER_OF_OPEN_FILES -1) {
        printf("invalid fileid \n");
        return NOT_DEFINED; }

    if(checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE) {
        printf("slot already closed!");
        return NOT_DEFINED;
    }

    SetSlotInOpenFileTableToNotInUse(theOpenFileTable, fileID);

    removeDataFromSlotInOpenFileTable(theOpenFileTable, fileID);

  return 0;
}
int sfs_frseek(int fileID, int loc){
    if (loc < 0)
        return -1;
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc)
        return -1;
    theOpenFileTable->table[fileID].readPointer = loc;
    return 0;
}

int sfs_fwseek(int fileID, int loc){
    if (loc < 0)
        return -1;
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc)
        return -1;
    theOpenFileTable->table[fileID].writePointer = loc;
    return 0;

}
int sfs_fwrite(int fileID, char *buf, int length){
    if (length <1){
        return -1;
    }



  return 0;
}
int sfs_fread(int fileID, char *buf, int length){
    if (length < 1) {
        return -1;
    }

    if (checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE){
        return -1;
    }

    int readPointerLocation = theOpenFileTable->table[fileID].readPointer;

    if(readPointerLocation + length > theINodeTable->table[theOpenFileTable->table[fileID].INode].size)
        length = theINodeTable->table[theOpenFileTable->table[fileID].INode].size - readPointerLocation;

    int beginningBlock = calculateWhichBlock(readPointerLocation);
    int endingBlock = calculateWhichBlock(readPointerLocation+length-1);
    int sizeOfInformationBeingReadIn = (endingBlock - beginningBlock +1)*BLOCK_SIZE;

    char temporaryReadBuffer[sizeOfInformationBeingReadIn];

    int i;
    // ******** //
    for(i = beginningBlock; i<=endingBlock; i++){
        read_blocks(i, 1, temporaryReadBuffer + (beginningBlock == 0 ? i : (i % beginningBlock)) * BLOCK_SIZE);
    }
    // ********* //

    memcpy(buf, temporaryReadBuffer + (theOpenFileTable->table[fileID].readPointer % BLOCK_SIZE), length);
    sfs_frseek(fileID,theOpenFileTable->table[fileID].readPointer+length -1);


    return length;
}
int sfs_remove(char *file){
    int indexToRemoveFromDirectory = findIndexOfFileInDirectory(theDirectoryCache, file);
    if (indexToRemoveFromDirectory == -1)
        return -1;
    int pointerToInode = theDirectoryCache->theDirectory[indexToRemoveFromDirectory].pointerToINode;
    eraseAllINodeBlockPointerData(theINodeTable, theBitMapBlock, pointerToInode);
    markSpecificPosistionInDirectoryAsOpen(theDirectoryCache, indexToRemoveFromDirectory);
    theDirectoryCache->theDirectory[indexToRemoveFromDirectory].pointerToINode = NOT_DEFINED;
    strcpy(theDirectoryCache->theDirectory[indexToRemoveFromDirectory].fileName, "");

    void * temporaryData;
    temporaryData = calloc(1,calculateSizeNeeded(sizeof(INodeTable)));
    memcpy(temporaryData, theINodeTable, sizeof(INodeTable));
    write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData);
    free(temporaryData);

    temporaryData = calloc(1,calculateSizeNeeded((sizeof(directoryCache))));
    memcpy(temporaryData, theDirectoryCache, sizeof(directoryCache));
    write_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData);
    free(temporaryData);

    temporaryData = calloc(1,calculateSizeNeeded(sizeof(bitmapBlock)));
    memcpy(temporaryData, theBitMapBlock, sizeof(bitmapBlock));
    write_blocks(DATA_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData);
    free(temporaryData);

  return 0;
}
