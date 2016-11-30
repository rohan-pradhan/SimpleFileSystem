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


INodeTable *theINodeTable = NULL;
directoryCache *theDirectoryCache = NULL;
openFileTable *theOpenFileTable = NULL;
bitmapBlock *theBitMapBlock = NULL;
unsigned superBlock1[256];


void mksfs(int fresh){
    //superBlock theSuperBlock = {};
    printf("\n \n *** Beginning Make FS *** \n \n");
    theOpenFileTable = calloc(1,sizeof(openFileTable));
    for(int i =0; i<MAX_NUMBER_OF_OPEN_FILES; i++) {
        SetSlotInOpenFileTableToNotInUse(theOpenFileTable, i);
    }
  if (fresh ==1){
        remove(FS);
      init_fresh_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS);
//      theSuperBlock.magicNumber= rand();
//      theSuperBlock.blockSize = BLOCK_SIZE;
//      theSuperBlock.INodeTableLength= NUMBER_OF_INODES;
//      theSuperBlock.FSSize = NUMBER_OF_BLOCKS;
//      theSuperBlock.rootDirINodePointer = 0;
//      write_blocks(SB_INDICIE, 1, &theSuperBlock);
        superBlock1[0]=rand();
        superBlock1[1]=BLOCK_SIZE;
        superBlock1[2]=NUMBER_OF_INODES;
        superBlock1[3]=NUMBER_OF_BLOCKS;
        superBlock1[4]= 0;
        write_blocks(0,1, superBlock1);

     //
      free(theINodeTable);
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
      write_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData);
      free(temporaryData);


//      char *test="Feeeeeeeeeeeeeed.txt";
//      int testVar = validateFileName(test);
//      printf("%d\n", testVar);


  }
    else {
      init_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS);
      read_blocks(0,1,superBlock1);

      int numberBitMapBlocks = DATA_INDICIE - FREE_BIT_MAP_INDICIE;
      char bitMapBlockRestore[BLOCK_SIZE * numberBitMapBlocks];
      read_blocks(FREE_BIT_MAP_INDICIE, numberBitMapBlocks, bitMapBlockRestore);
      memcpy(theBitMapBlock, bitMapBlockRestore, sizeof(theBitMapBlock));
      

//

      void *temporaryData;
      temporaryData = calloc(1, calculateSizeNeeded(sizeof(INodeTable)));
      read_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData);
      memcpy(theINodeTable, temporaryData, sizeof(INodeTable));
      printf("After read: %d\n", theINodeTable->table[0].size);
      free(temporaryData);

      if (theDirectoryCache == NULL) {
          theDirectoryCache = calloc(1, sizeof(directoryCache));
      }
      printf("Size of directory cache: %d\n", sizeof(directoryCache));
      temporaryData = calloc(1, calculateSizeNeeded(sizeof(directoryCache)));
      read_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData);
      memcpy(theDirectoryCache, temporaryData, sizeof(directoryCache));
      free(temporaryData);
//
      if (theBitMapBlock == NULL) {
          theBitMapBlock = calloc(1, sizeof(bitmapBlock));
      }
      temporaryData = calloc(1, calculateSizeNeeded(sizeof(bitmapBlock)));
      read_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded(sizeof(bitmapBlock)), temporaryData);
      memcpy(theBitMapBlock, temporaryData, sizeof(bitmapBlock));
      free(temporaryData);

  }

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
   printf("\n -----Beginning the Open Function ------ \n");

    if (validateFileName(name) == FALSE){
        return -1;
    }

    int indexInDirectory =  findIndexOfFileInDirectory(theDirectoryCache, name);

   // int indexInDirectory =
    if (indexInDirectory != NOT_DEFINED){
        int iNodeNumber = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode;
        printf("The INodeNumber is: %d\n", iNodeNumber);
        //int fileDescriptorIndex = returnIndexOfINodeFromOpenFileTable(*theOpenFileTable, iNodeNumber);
        int fileDescriptorIndex = getIndexOfOpenFileTableForINode(*theOpenFileTable, iNodeNumber);
        printf("This is the value fileDescriptorIndex is returning: %d\n", fileDescriptorIndex);
        if (fileDescriptorIndex != -1){
            printf("Write Pointer inside location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);
            int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode;
            printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size);
            return fileDescriptorIndex;

        } else {
            fileDescriptorIndex = getNextOpenSpotInFileDescriptorTable(theOpenFileTable);
            SetSlotInOpenFileTableToInUse(theOpenFileTable, fileDescriptorIndex);
            theOpenFileTable->table[fileDescriptorIndex].INode = iNodeNumber;
            theOpenFileTable->table[fileDescriptorIndex].readPointer = 0;
            theOpenFileTable->table[fileDescriptorIndex].writePointer = theINodeTable->table[iNodeNumber].size;
            printf("Write Pointer location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);
            int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode;
            printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size);
            return fileDescriptorIndex;
        }


    } else {
        int iNodeNumber = getIndexOfNextOpenSpotInINodeTable(theINodeTable);
        if (iNodeNumber != NOT_DEFINED) {
            theINodeTable->table[iNodeNumber].size = 0;
            theINodeTable->usedTable[iNodeNumber] = IN_USE;
            theINodeTable->table[iNodeNumber].indirectINodePointer = -1;
            write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), theINodeTable);
            int fileDescriptorIndex = getNextOpenSpotInFileDescriptorTable(theOpenFileTable);
            if (fileDescriptorIndex != -1) {
                SetSlotInOpenFileTableToInUse(theOpenFileTable, fileDescriptorIndex);
            theOpenFileTable->table[fileDescriptorIndex].INode = iNodeNumber;
            theOpenFileTable->table[fileDescriptorIndex].readPointer = 0;
            theOpenFileTable->table[fileDescriptorIndex].writePointer = theINodeTable->table[iNodeNumber].size;
                printf("Write Pointer location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);

            int indexInDirectory = getIndexOfNextOpenSpotInDirectory(theDirectoryCache);
            theDirectoryCache->fileUsed[indexInDirectory] = IN_USE;
            strcpy(theDirectoryCache->theDirectory[indexInDirectory].fileName, name);
            theDirectoryCache->theDirectory[indexInDirectory].pointerToINode = iNodeNumber;
            write_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), theDirectoryCache);
                int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode;
                printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size);
                return fileDescriptorIndex; }
            else {
                return -1;
            }


        }

    }

}
int sfs_fclose(int fileID){
    printf("\n -----Beginning the Close Function ------ \n");
    printf("The fildID we are closing is: %d\n", fileID);
    if (fileID < 0 || fileID > MAX_NUMBER_OF_OPEN_FILES -1) {
        printf("invalid fileid \n");
        return NOT_DEFINED; }

    if(checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE) {
        printf("slot already closed!");
        return NOT_DEFINED;
    }

    SetSlotInOpenFileTableToNotInUse(theOpenFileTable, fileID);

    removeDataFromSlotInOpenFileTable(theOpenFileTable, fileID);
    printf("information about file table: %d\n", theOpenFileTable->table[fileID].INode);
    printf("File Successfully closed!\n");

  return 0;
}


int sfs_frseek(int fileID, int loc){
    printf("\n***Beginning Read Seek***\n");
    printf("FileID We are Read Seeking: %d\n", fileID);
    //printf("\n \n TRYING TO SEEK READ to location: %d \n \n", loc);
    if (loc < 0) {
        return -1;
    }
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc){
        printf("The Size of the INode is: %d\n", theINodeTable->table[theOpenFileTable->table[fileID].INode].size);
        printf("The location to seek to is: %d\n", loc);
        return -1;}
    theOpenFileTable->table[fileID].readPointer = loc;
    printf("Read Pointer Location = %d\n", theOpenFileTable->table[fileID].readPointer);
    return 0;
}

int sfs_fwseek(int fileID, int loc){
    printf("\n***Beginning Write Seek***\n");
    //printf("\n \n TRYING TO SEEK WRITE to location: %d \n \n", loc);
    if (loc < 0)
        return -1;
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc)
        return -1;
    theOpenFileTable->table[fileID].writePointer = loc;
    return 0;

}


int sfs_fwrite(int fileID, char *buf, int length){

    printf("\n \n -----Beginning Write------\n");
    printf("This is what we are trying to write: %s\n", buf);
    printf("This is the length of that: %d\n\n", length);
    if (length <1){
        return -1;}
    printf("The fileID we are writing to is: %d\n", fileID);

    int bytesWrittenCounter = 0;

    int bytesToWrite = length;
    //printf("Bytestowrite = %d\n", bytesToWrite);
    if (checkIfFileIDIsOpen(theOpenFileTable, fileID) == FALSE) {
        return -1;
    }

    int INodeIndex = theOpenFileTable->table[fileID].INode;
    printf("The INodeIndex For this write is: %d\n", INodeIndex);
    printf("The name of this file is: %s\n", theDirectoryCache->theDirectory[fileID].fileName);
    void *temporaryDiskData;
    temporaryDiskData = calloc(1, BLOCK_SIZE);

    indirectBlock *theIndirectBlock; // *******RPCHANGE*********
    theIndirectBlock = calloc(1, (NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int)));

    int initialWritePointer = theOpenFileTable->table[fileID].writePointer;

    int initialEndLength = initialWritePointer + length;
    int initialBlockCalculation = calculateWhichBlock(initialEndLength);


    if (checkIfPointerIsInIndirectBlock(initialBlockCalculation) == TRUE) {

    if (checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) == TRUE) {
        int i;
        for(i=0; i<NUMBER_OF_INODE_INDIRECT_POINTERS; i++){
            theIndirectBlock->indirectPointers[i] = NOT_DEFINED;
        }
        int freeBlock = findFreeBit(theBitMapBlock);

        printf("the free block is: %d for this file: %s\n", freeBlock, theDirectoryCache->theDirectory[fileID].fileName );
        if (freeBlock == -1) {
            return NOT_DEFINED;
        }
        theINodeTable->table[INodeIndex].indirectINodePointer = freeBlock;
        printf("Indirect blockpointer is: %d\n\n", theINodeTable->table[INodeIndex].indirectINodePointer );
        theBitMapBlock->block[freeBlock] = IN_USE;
    } else {
        read_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer,1, theIndirectBlock);
    } }

    while(bytesToWrite >0) {
        memset(temporaryDiskData, '\0', BLOCK_SIZE);
        //void *temporaryDiskData;
        //temporaryDiskData = calloc(1, BLOCK_SIZE);

        printf("\n**Entering the while loop!**\n");


        int PosistionOfWritePointer = initialWritePointer;

        printf("Posistion of write poitner in loop: %d\n", PosistionOfWritePointer);

        int blockNumberInINode = calculateWhichBlock(PosistionOfWritePointer);


        int posistionInBlock = calculateWhereInBlock(PosistionOfWritePointer);

        int amountThatCanBeWrittenInSingleIteration;


        if(bytesToWrite > (BLOCK_SIZE - posistionInBlock)){

            amountThatCanBeWrittenInSingleIteration = BLOCK_SIZE - posistionInBlock;

        }
        else {

            amountThatCanBeWrittenInSingleIteration = bytesToWrite;
        }


        int newBlockCreatedFlag = FALSE;

        int pointerToDisk;

        if(checkIfPointerIsInIndirectBlock(blockNumberInINode) == TRUE) {

            int blockInIndirect = calculateWhichIndirectBlock(blockNumberInINode);
            if (checkIfIndirectDataPointerIsEmpty(theIndirectBlock, blockInIndirect) == TRUE){
                newBlockCreatedFlag = TRUE;
                pointerToDisk =  findFreeBit(theBitMapBlock);
                if (pointerToDisk <0) {
                    break;
                }
                theBitMapBlock->block[pointerToDisk] = IN_USE;
                theIndirectBlock->indirectPointers[blockInIndirect] = pointerToDisk;
                printf("Simi error: %d\n", theIndirectBlock->indirectPointers[blockInIndirect]);

            }

        } else {

        if (checkIfDataPointerIsEmptyInINode(theINodeTable, INodeIndex, blockNumberInINode) == TRUE){
           // printf("11\n");
          //  printf("Data pointer in inode: %d", theINodeTable->table[INodeIndex].iNodePointers[blockNumberInINode]);
            newBlockCreatedFlag = TRUE;
           // printf("12\n");
            pointerToDisk = findFreeBit(theBitMapBlock);
           // printf("\nPointer to disk found: %d\n\n", pointerToDisk );
            //printf("13\n");
         //   printf("Pointer to disk: %d\n",pointerToDisk);
            if (pointerToDisk < 0) {
                break;
            }
            theBitMapBlock->block[pointerToDisk] = IN_USE;
            theINodeTable->table[INodeIndex].iNodePointers[blockNumberInINode] = pointerToDisk;
        } }


        if(newBlockCreatedFlag == FALSE) {

            pointerToDisk = theINodeTable->table[INodeIndex].iNodePointers[blockNumberInINode];
            printf("Pointer To disk (block not new) = %d\n", pointerToDisk);
            read_blocks(DATA_INDICIE + pointerToDisk, 1, temporaryDiskData);

        }


        memcpy(temporaryDiskData+posistionInBlock, buf, (size_t) amountThatCanBeWrittenInSingleIteration);
        printf("What buf is copying: %s\n", temporaryDiskData);

        write_blocks(DATA_INDICIE+pointerToDisk,1,temporaryDiskData);

            initialWritePointer += amountThatCanBeWrittenInSingleIteration;


        bytesWrittenCounter += amountThatCanBeWrittenInSingleIteration;
        bytesToWrite -= amountThatCanBeWrittenInSingleIteration;
        buf += amountThatCanBeWrittenInSingleIteration;
        printf("writePointer location = %d\n", theOpenFileTable->table[fileID].writePointer );


        if(initialWritePointer > theINodeTable->table[INodeIndex].size) {
            theINodeTable->table[INodeIndex].size = initialWritePointer;
            printf("INodeSize = %d\n", theINodeTable->table[INodeIndex].size );
        }

    }
    free(temporaryDiskData);

    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) < 1) {
        printf("%d\n", theINodeTable->table[INodeIndex].indirectINodePointer);
        for(int i = 0; i<10; i++){
            printf("Corn: %d\n", theIndirectBlock->indirectPointers[i]);
        }
        write_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer, 1, theIndirectBlock);
    }


    void *temporaryData;

    temporaryData=calloc(1,calculateSizeNeeded(sizeof(INodeTable)));


    memcpy(temporaryData, theINodeTable, sizeof(INodeTable));
    printf("B4 write: InodeSize: %d\n", theINodeTable->table[0].size);
    write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), theINodeTable);
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
    write_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData);
    free(temporaryData);

    printf("Number of Bytes Written: %d \n", bytesWrittenCounter);

    return bytesWrittenCounter;


}
int sfs_fread(int fileID, char *buf, int length){
    printf("\n \n ------Beginning to read-------- \n");
    printf("The file ID we are reading is: %d\n", fileID);
    printf("The Length of how much we are reading is: %d\n", length);
    printf("The file name we are reading is: %s\n", theDirectoryCache->theDirectory[fileID].fileName);
    if (length < 1) {
        printf("returned -1 cause of length\n");
        return -1;
    }

    if (checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE){
        printf("returned -1 cause of slot not in use\n");

        return -1;
    }

    int INodeIndex = theOpenFileTable->table[fileID].INode;
    printf("The INodeIndex For this read is: %d\n", INodeIndex);


    int readPointerLocation = theOpenFileTable->table[fileID].readPointer;
    printf("readpointerLocation = %d\n", readPointerLocation);
    printf("length = %d\n", length);
    printf("INodeSize = %d\n", theINodeTable->table[theOpenFileTable->table[fileID].INode].size);
    printf("INodeSize2 = %d\n", theINodeTable->table[INodeIndex].size);


    if(readPointerLocation + length > theINodeTable->table[theOpenFileTable->table[fileID].INode].size)
        length = theINodeTable->table[theOpenFileTable->table[fileID].INode].size - readPointerLocation;



    indirectBlock *theIndirectBlock;
    theIndirectBlock = calloc(1, NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int));

    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) == FALSE) {
        read_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer,1, theIndirectBlock);
    }

    int initialBeginningBlock = calculateWhichBlock(readPointerLocation);
    int initialEndingBlock = calculateWhichBlock(readPointerLocation+length-1);

    int readCount = 0;
    int leftToReadCount = length;
    printf("left to readcount is: %d\n", leftToReadCount);
    void *temporaryData;
    temporaryData = calloc(1, (initialEndingBlock - initialBeginningBlock +1)*BLOCK_SIZE);
    while (leftToReadCount > 0) {
        memset(temporaryData, '\0', BLOCK_SIZE);
        printf("iteration %d\n", leftToReadCount);


        int beginningBlock = calculateWhichBlock(readPointerLocation);
        int endingBlock = calculateWhichBlock(readPointerLocation+length-1);
        int sizeOfInformationBeingReadIn = (endingBlock - beginningBlock +1)*BLOCK_SIZE;
        int posistionInBlock = calculateWhereInBlock(readPointerLocation);


        int amountThatCanBeReadInSingleIteration;

        if(leftToReadCount > BLOCK_SIZE - posistionInBlock){
            amountThatCanBeReadInSingleIteration = BLOCK_SIZE - posistionInBlock;
        }
        else {
            amountThatCanBeReadInSingleIteration = leftToReadCount;
        }


        if(checkIfPointerIsInIndirectBlock(beginningBlock) == TRUE) {
            int indirectBlockIndex = calculateWhichIndirectBlock(beginningBlock);
            read_blocks(DATA_INDICIE+theIndirectBlock->indirectPointers[indirectBlockIndex],1, temporaryData);
        } else {

           // void *temporaryData2;

        // IMPLEMENT INDIRECT BLOCK STUFF
        printf("\n \n The beginning block is: %d \n", beginningBlock);
        int pointerToDisk = theINodeTable->table[INodeIndex].iNodePointers[beginningBlock];
        printf("\n \nThe pointer to disk is: %d\n", pointerToDisk);
        read_blocks(DATA_INDICIE+ pointerToDisk, 1, temporaryData);}
        //read_blocks(DATA_INDICIE,1,temporaryData2);
       // printf("\n this is what temporary disk data 2 has: %s", temporaryData2);
        printf("\n \n this is where we are in the buffer: %d \n", buf);
        //printf("amount that can be read in single iteration: %d", amountThatCanBeReadInSingleIteration);
        printf("\n This is what temporary Disk Data has: %s\n\n", temporaryData);
        memcpy(buf, temporaryData + posistionInBlock, amountThatCanBeReadInSingleIteration);
        printf("this is what buffer should have: %s\n", buf);
        theOpenFileTable->table[fileID].readPointer += amountThatCanBeReadInSingleIteration;
        readCount += amountThatCanBeReadInSingleIteration;
        leftToReadCount -= amountThatCanBeReadInSingleIteration;
        buf += amountThatCanBeReadInSingleIteration;
        printf("String Length of buf: %d\n", strlen(buf));
        readPointerLocation += amountThatCanBeReadInSingleIteration;


    }

    free(temporaryData);
    printf("readcount: %d \n", readCount);
    printf("INodeSize kk = %d\n", theINodeTable->table[INodeIndex].size );

    return readCount;


}
int sfs_remove(char *file){
    printf("\nEntering remove \n \n");
    int indexToRemoveFromDirectory = findIndexOfFileInDirectory(theDirectoryCache, file);
    if (indexToRemoveFromDirectory == -1)
        return -1;
    int pointerToInode = theDirectoryCache->theDirectory[indexToRemoveFromDirectory].pointerToINode;



    if (getIndexOfOpenFileTableForINode(*theOpenFileTable, pointerToInode) != -1) {
        sfs_fclose(pointerToInode);
    }

    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, pointerToInode) == FALSE || theINodeTable->table[pointerToInode].indirectINodePointer > -1) {
        indirectBlock *theIndirectBlock;
        theIndirectBlock = calloc(1, NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int));
        printf("where the indirect pointer is pointing to: %d\n", theINodeTable->table[pointerToInode].indirectINodePointer );
        read_blocks(DATA_INDICIE+theINodeTable->table[pointerToInode].indirectINodePointer,1, theIndirectBlock);
        printf("Simi\n");
        int i;
        for (i=0; i<NUMBER_OF_INODE_INDIRECT_POINTERS; i++){
            if(checkIfIndirectDataPointerIsEmpty(theIndirectBlock, i) == FALSE) {// *****
                if (testForStackCorruption(theIndirectBlock, i) == FALSE) {
                    printf("Doin work %d \n", i);
                    printf("Location in indirectBlock: %d\n", theIndirectBlock->indirectPointers[i]);
                    void *temporaryData;
                    temporaryData = calloc(1, BLOCK_SIZE);
                    write_blocks(DATA_INDICIE + theIndirectBlock->indirectPointers[i], 1, temporaryData);
                    free(temporaryData);
                    printf("Location in free bitmap: %d\n",
                           theBitMapBlock->block[theIndirectBlock->indirectPointers[i]]);
                    theBitMapBlock->block[theIndirectBlock->indirectPointers[i]] = FALSE;
                    theIndirectBlock->indirectPointers[i] = NOT_DEFINED;
                }
                else {
                    theIndirectBlock->indirectPointers[i] = NOT_DEFINED;
                    printf(" \n Warning Stack Corrupted \n");
                }
            }
        }
    }

    eraseAllINodeBlockPointerData(theINodeTable, theBitMapBlock, pointerToInode);

    setPosistionInInodeTableToOpen(theINodeTable, pointerToInode);
    setNewINode(&theINodeTable->table[pointerToInode]);

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
    write_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData);
    free(temporaryData);

  return 0;
}
