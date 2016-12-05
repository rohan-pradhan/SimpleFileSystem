//Rohan N. Pradhan
//COMP 310 - Simple File System
//Student ID: 260613559

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
    printf("\n \n *** Beginning Make FS *** \n \n");
    theOpenFileTable = calloc(1,sizeof(openFileTable));
    for(int i =0; i<MAX_NUMBER_OF_OPEN_FILES; i++) {
        SetSlotInOpenFileTableToNotInUse(theOpenFileTable, i);
    }
  if (fresh ==1){
        remove(FS); // Remove existing FS if there already

        init_fresh_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS); // initialize new disk through emulator

        // SUPER BLOCK INITIALIZATION
        superBlock1[0]=rand();
        superBlock1[1]=BLOCK_SIZE;
        superBlock1[2]=NUMBER_OF_INODES;
        superBlock1[3]=NUMBER_OF_BLOCKS;
        superBlock1[4]= 0;
        write_blocks(0,1, superBlock1); // writes serialized superblock to disk through emulator function

      // **********************************************************
      // In-memory Data Structure Initilizations and Writes to Disk
      // **********************************************************

      //INodeTable
      free(theINodeTable);
      if(theINodeTable == NULL) {
          theINodeTable=calloc(1, sizeof(INodeTable)); //Initizilize and Allocate
      }

      void *temporaryData; // pointer to serve as buffer for writes to disk

      temporaryData=calloc(1,calculateSizeNeeded(sizeof(INodeTable)));

      memcpy(temporaryData, theINodeTable, sizeof(INodeTable)); //copy INodeTable to buffer
      write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData); // write buffer to disk
      free(temporaryData);

      //On-disk File Directory
      if (theDirectoryCache == NULL) {
          theDirectoryCache = calloc(1, sizeof(directoryCache)); //Initizilize and Allocate
      }

      temporaryData = calloc(1,calculateSizeNeeded((sizeof(directoryCache)))); // pointer to serve as buffer for writes to disk
      memcpy(temporaryData, theDirectoryCache, sizeof(directoryCache)); //copy Directory Cache to buffer
      write_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData); // write buffer to disk
      free(temporaryData);


      //Free Bitmap Block
      if (theBitMapBlock == NULL) {
          theBitMapBlock = calloc(1, sizeof(bitmapBlock)); // Initizilize and Allocate
      }

      temporaryData = calloc(1,calculateSizeNeeded(sizeof(bitmapBlock))); // pointer to serve as buffer for writes to disk
      memcpy(temporaryData, theBitMapBlock, sizeof(bitmapBlock)); //copy BitMap Block to buffer
      write_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded((sizeof(bitmapBlock))), temporaryData); // write buffer to disk
      free(temporaryData);


  }
    else { //file system not fresh

      init_disk(FS, BLOCK_SIZE, NUMBER_OF_BLOCKS); //initilize disk through emulator function
      read_blocks(0,1,superBlock1); // read superblock information into superblock array


      // **********************************************************
      // In-memory Data Structure Initilizations and Reads from Disk
      // **********************************************************


      //Free Bitmap Block
      int numberBitMapBlocks = DATA_INDICIE - FREE_BIT_MAP_INDICIE;  //calculate number of BitMapBlocks there is (found from data indicies for blocks)
      char bitMapBlockRestore[BLOCK_SIZE * numberBitMapBlocks]; // initialize buffer
      read_blocks(FREE_BIT_MAP_INDICIE, numberBitMapBlocks, bitMapBlockRestore); // read from disk to buffer
      memcpy(theBitMapBlock, bitMapBlockRestore, sizeof(bitmapBlock)); //copy from buffer to BitMap Block In-memory Data Structure


      void *temporaryData; //declare buffer for reads from disks

      //INodeTable
      temporaryData = calloc(1, calculateSizeNeeded(sizeof(INodeTable))); //allocate buffer of size INodeTable
      read_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), temporaryData); //read from disk to buffer
      memcpy(theINodeTable, temporaryData, sizeof(INodeTable)); // copy from buffer to INodeTable In-memory Data Structure
      printf("After read: %d\n", theINodeTable->table[0].size);
      free(temporaryData);


      //File Directory
      if (theDirectoryCache == NULL) {
          theDirectoryCache = calloc(1, sizeof(directoryCache)); // just incase it is not been allocated yet
      }
      printf("Size of directory cache: %lu\n", sizeof(directoryCache));
      temporaryData = calloc(1, calculateSizeNeeded(sizeof(directoryCache))); // allocate buffer of size File Directory Cache
      read_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), temporaryData); // read from disk to buffer
      memcpy(theDirectoryCache, temporaryData, sizeof(directoryCache)); // copy from buffer File Directory Cache In-memory Data Structure
      free(temporaryData);


//      if (theBitMapBlock == NULL) {
//          theBitMapBlock = calloc(1, sizeof(bitmapBlock));
//      }
//      temporaryData = calloc(1, calculateSizeNeeded(sizeof(bitmapBlock)));
//      read_blocks(FREE_BIT_MAP_INDICIE, calculateNumberOfBlocksNeeded(sizeof(bitmapBlock)), temporaryData);
//      memcpy(theBitMapBlock, temporaryData, sizeof(bitmapBlock));
//      free(temporaryData);

  }

}


int sfs_get_next_file_name(char *fname){
    int indexOfCurrentPosistion = theDirectoryCache->posistionInDirectory; //gets posistion fron Directory Cache struct
    int i =0;
    while(indexOfCurrentPosistion+i < 40){ // loops through all directory cache posistions
        int adjustedIndex = indexOfCurrentPosistion+i; //increments index

        if (checkIfPosistionInDirectoryIsUsed(theDirectoryCache,adjustedIndex)) {
            theDirectoryCache->posistionInDirectory++; //increment index in Directory Cache struct
            strcpy(fname, theDirectoryCache->theDirectory[adjustedIndex].fileName); //copy filename to fname pointer
            return 1; // return success

        }
        else
            i++; // increment i
    }

    //looped through the entire directory goes back to the beginning
    theDirectoryCache->posistionInDirectory = 0; //sets index in struct
    printf("Final Pos. in directory: %d \n", theDirectoryCache->posistionInDirectory);
    return 0; // returns that we have looped through the whole directory


}


int sfs_get_file_size(char* path){
    if (validateFileName(path) == TRUE) { //checks if the path supplied is even valid
        int i;
        for (i = 0; i < 40; i++) { // for loop to loop through directory struct
            if (strcmp(theDirectoryCache->theDirectory[i].fileName, path) == 0) { // checks if entry in directory struct is same as supplied name
                int INodeOfMatchingFile = theDirectoryCache->theDirectory[i].pointerToINode; // gets the matching INode
                int sizeToReturn = theINodeTable->table[INodeOfMatchingFile].size; //gets the size of respective INode
                return sizeToReturn; // returns Size of file
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

    if (validateFileName(name) == FALSE){ //check if the filename is even valid
        return -1; //return error if it is not valid
    }

    int indexInDirectory =  findIndexOfFileInDirectory(theDirectoryCache, name); // find which index in the Directory


    if (indexInDirectory != NOT_DEFINED){ //This means the file is found in the INDEX
        int iNodeNumber = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode; // get the corresponding INode
        printf("The INodeNumber is: %d\n", iNodeNumber);

        int fileDescriptorIndex = getIndexOfOpenFileTableForINode(*theOpenFileTable, iNodeNumber); //find the Index in the OpenFile Table
        printf("This is the value fileDescriptorIndex is returning: %d\n", fileDescriptorIndex);
        if (fileDescriptorIndex != -1){  // It Found the index in the open file table
            printf("Write Pointer inside location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);
            int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode; // for testing
            printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size); // for testing
            return fileDescriptorIndex; //returns the index in the file descriptor table

        } else { //not found in Open File table
            fileDescriptorIndex = getNextOpenSpotInFileDescriptorTable(theOpenFileTable); // find the next open spot in the open file table
            SetSlotInOpenFileTableToInUse(theOpenFileTable, fileDescriptorIndex); //set that slot to used
            theOpenFileTable->table[fileDescriptorIndex].INode = iNodeNumber; //set the INodeNumber
            theOpenFileTable->table[fileDescriptorIndex].readPointer = 0; // set the read pointer
            theOpenFileTable->table[fileDescriptorIndex].writePointer = theINodeTable->table[iNodeNumber].size; // set  the write pointer
            printf("Write Pointer location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);
            int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode; // for testing
            printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size); // for testing
            return fileDescriptorIndex; // returns the index in the file descriptor table
        }


    } else { //file not found in the file directory (ie file not created)
        int iNodeNumber = getIndexOfNextOpenSpotInINodeTable(theINodeTable); // find the next open spot in the INodeTable
        if (iNodeNumber != NOT_DEFINED) { // there is space in the INodeTable

            // setup the INode
            theINodeTable->table[iNodeNumber].size = 0;
            theINodeTable->usedTable[iNodeNumber] = IN_USE;
            theINodeTable->table[iNodeNumber].indirectINodePointer = -1;
            write_blocks(INODE_INDICIE, calculateNumberOfBlocksNeeded(sizeof(INodeTable)), theINodeTable); // flush to disk

            // setup the fileDescriptor
            int fileDescriptorIndex = getNextOpenSpotInFileDescriptorTable(theOpenFileTable); // find next open spot in the open file table
            if (fileDescriptorIndex != -1) { // there is space in the open file table
                SetSlotInOpenFileTableToInUse(theOpenFileTable, fileDescriptorIndex); // set that slot to used
                theOpenFileTable->table[fileDescriptorIndex].INode = iNodeNumber; // set the INodeNumber
                theOpenFileTable->table[fileDescriptorIndex].readPointer = 0; // set the read pointer
                theOpenFileTable->table[fileDescriptorIndex].writePointer = theINodeTable->table[iNodeNumber].size; // set the write pointer
                printf("Write Pointer location = %d \n", theOpenFileTable->table[fileDescriptorIndex].writePointer);


                //setup the Directory
                int indexInDirectory = getIndexOfNextOpenSpotInDirectory(theDirectoryCache); // find the next open spot in the directory cache
                theDirectoryCache->fileUsed[indexInDirectory] = IN_USE; // set that slot to used
                strcpy(theDirectoryCache->theDirectory[indexInDirectory].fileName, name); //copy the name
                theDirectoryCache->theDirectory[indexInDirectory].pointerToINode = iNodeNumber; //set the INodeNumber
                write_blocks(DIRECTORY_INDICIE, calculateNumberOfBlocksNeeded(sizeof(directoryCache)), theDirectoryCache); // flush to disk
                int iNodeIndex = theDirectoryCache->theDirectory[indexInDirectory].pointerToINode; //for testing
                printf("\n\nSize of Inode is: %d\n", theINodeTable->table[iNodeIndex].size); // for testing
                return fileDescriptorIndex; // return the index in the file descriptor table
            } else {
                return -1; // not enough space in the tables ERROR
            }


        }

    }
return -1; // not enough space in the tables ERROR
}


int sfs_fclose(int fileID){
    printf("\n -----Beginning the Close Function ------ \n");
    printf("The fileID we are closing is: %d\n", fileID);
    if (fileID < 0 || fileID > MAX_NUMBER_OF_OPEN_FILES -1) { // test to see if file even exists/can be closed
        printf("\n Invalid FileID! \n");
        return NOT_DEFINED; }

    if(checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE) { // check if that file is even open
        printf("\n Slot is already closed! \n");
        return NOT_DEFINED;
    }

    SetSlotInOpenFileTableToNotInUse(theOpenFileTable, fileID); // seet the slot to open in Open File Table

    removeDataFromSlotInOpenFileTable(theOpenFileTable, fileID); // removes data from the open file table
    printf("information about file table: %d\n", theOpenFileTable->table[fileID].INode); // testing
    printf("File Successfully closed!\n"); //testing

  return 0; // return success
}


int sfs_frseek(int fileID, int loc){
    printf("\n ------Beginning Read Seek------ \n");
    printf("\n \n TRYING TO SEEK WRITE to location: %d \n \n", loc);
    printf("FileID We are Read Seeking: %d\n", fileID);

    if (loc < 0) { // location invalid
        return -1; // Error
    }
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc){ //location invalid
        printf("The Size of the INode is: %d\n", theINodeTable->table[theOpenFileTable->table[fileID].INode].size);
        printf("The location to seek to is: %d\n", loc);
        return -1;} // Error

    theOpenFileTable->table[fileID].readPointer = loc; //seek the read pointer
    printf("Read Pointer Location = %d\n", theOpenFileTable->table[fileID].readPointer);
    return 0; // return success
}

int sfs_fwseek(int fileID, int loc){
    printf("\n ------Beginning Write Seek------ \n");
    printf("\n \n TRYING TO SEEK WRITE to location: %d \n \n", loc);
    if (loc < 0) // location invalid
        return -1; // Error
    if (theINodeTable->table[theOpenFileTable->table[fileID].INode].size < loc) // location invalid
        return -1; // Error
    theOpenFileTable->table[fileID].writePointer = loc; // seek the write pointer
    return 0;// return success

}


int sfs_fwrite(int fileID, char *buf, int length){

    printf("\n -----Beginning Write------\n");
    printf("This is what we are trying to write: %s\n", buf);
    printf("This is the length of that: %d\n\n", length);

    if (length <1){ // invalid length
        return -1;} // error

    printf("The fileID we are writing to is: %d\n", fileID);

    int bytesWrittenCounter = 0; // counter of how much has been written

    int bytesToWrite = length; // counter of how much is left to write

    if (checkIfFileIDIsOpen(theOpenFileTable, fileID) == FALSE) { // checks if file is even open
        return -1; // file not open error
    }

    int INodeIndex = theOpenFileTable->table[fileID].INode; // get INodeIndex
    printf("The INodeIndex For this write is: %d\n", INodeIndex);
    printf("The name of this file is: %s\n", theDirectoryCache->theDirectory[fileID].fileName);


    indirectBlock *theIndirectBlock; // declare the indirect block - may be needed
    theIndirectBlock = calloc(1, (NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int))); // allocates the indirect block

    int initialWritePointer = theOpenFileTable->table[fileID].writePointer;  // sets the initial posistion of the write pointer

    int initialEndLength = initialWritePointer + length; // calculates the estimated end length
    int initialBlockCalculation = calculateWhichBlock(initialEndLength); // calculates which block we will be in after write


    if (checkIfPointerIsInIndirectBlock(initialBlockCalculation) == TRUE) { // check if we will be in the indirect pointers

    if (checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) == TRUE) { // check if we have not setup the indirect pointers
        int i;
        for(i=0; i<NUMBER_OF_INODE_INDIRECT_POINTERS; i++){
            theIndirectBlock->indirectPointers[i] = NOT_DEFINED; //set all indiret pointers to not defined
        }
        int freeBlock = findFreeBit(theBitMapBlock); // find the free bit in the free bitmap block

        printf("The Free Block is: %d for the file: %s\n", freeBlock, theDirectoryCache->theDirectory[fileID].fileName );
        if (freeBlock == -1) { // no more space in free bit block
            return NOT_DEFINED; // error
        }
        theINodeTable->table[INodeIndex].indirectINodePointer = freeBlock; //sets location of where indirect block will be
        printf("Indirect BlockPointer is: %d\n\n", theINodeTable->table[INodeIndex].indirectINodePointer );
        theBitMapBlock->block[freeBlock] = IN_USE; // sets the block in the free bit map block to use

    } else {
        read_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer,1, theIndirectBlock); // reads data into allocated indirect block struct through emulator function
    } }

    while(bytesToWrite >0) { // loops until we have nothing more to write

        void *temporaryDiskData;
        temporaryDiskData = calloc(1, BLOCK_SIZE); // allocates and declares a buffer we will use to write to disk

        printf("\n**Entering the while loop!**\n");


        int PosistionOfWritePointer = initialWritePointer; // gets where we are in the file

        printf("Posistion of write poitner in loop: %d\n", PosistionOfWritePointer);

        int blockNumberInINode = calculateWhichBlock(PosistionOfWritePointer); // calcualtes which block pointer we are in the iNode


        int posistionInBlock = calculateWhereInBlock(PosistionOfWritePointer); // calculates where in the actual block we are

        int amountThatCanBeWrittenInSingleIteration;


        if(bytesToWrite > (BLOCK_SIZE - posistionInBlock)){

            amountThatCanBeWrittenInSingleIteration = BLOCK_SIZE - posistionInBlock; // resets how much we can write in one iteration so we don't run over the block

        }
        else {

            amountThatCanBeWrittenInSingleIteration = bytesToWrite;
        }


        int newBlockCreatedFlag = FALSE; // initially sets if we have created a new block to false

        int pointerToDisk;

        if(checkIfPointerIsInIndirectBlock(blockNumberInINode) == TRUE) { // checks if we are currently writing in the indirect portion of the INode

            int blockInIndirect = calculateWhichIndirectBlock(blockNumberInINode); // calculates which Indirect block
            if (checkIfIndirectDataPointerIsEmpty(theIndirectBlock, blockInIndirect) == TRUE){ // check if that block is empty
                newBlockCreatedFlag = TRUE; // we are creating a new block
                pointerToDisk =  findFreeBit(theBitMapBlock); // find a free place in the disk
                if (pointerToDisk <0) { // there is no space in the disk
                    return -1; // not enough space error
                    // /break; // break out of the loop
                }
                theBitMapBlock->block[pointerToDisk] = IN_USE; // mark in use
                theIndirectBlock->indirectPointers[blockInIndirect] = pointerToDisk; //set the pointer  in the indirect block to where on disk


            }

        } else { // we are not in the indirect portion of the INode

        if (checkIfDataPointerIsEmptyInINode(theINodeTable, INodeIndex, blockNumberInINode) == TRUE){ //checks if that direct block is empty

            newBlockCreatedFlag = TRUE; // if it is create a new block

            pointerToDisk = findFreeBit(theBitMapBlock); // find where to put information on disk

            if (pointerToDisk < 0) { // no more space on disk
                //break;
                return -1; // error
            }
            theBitMapBlock->block[pointerToDisk] = IN_USE; // mark in use
            theINodeTable->table[INodeIndex].iNodePointers[blockNumberInINode] = pointerToDisk; // set the pointer in the indirect block to where on disk
        } }


        if(newBlockCreatedFlag == FALSE) { // new block not created

            pointerToDisk = theINodeTable->table[INodeIndex].iNodePointers[blockNumberInINode]; // get pointer from INodeTable
            printf("Pointer To disk (block not new) = %d\n", pointerToDisk);
            read_blocks(DATA_INDICIE + pointerToDisk, 1, temporaryDiskData); // read information from disk to buffer

        }


        memcpy(temporaryDiskData+posistionInBlock, buf, (size_t) amountThatCanBeWrittenInSingleIteration); //adjust buffer to how much can be written
        printf("What buf is copying: %s\n", temporaryDiskData);

        write_blocks(DATA_INDICIE+pointerToDisk,1,temporaryDiskData); // write new information from buffer to disk

        initialWritePointer += amountThatCanBeWrittenInSingleIteration; // increment pointer in where we are in file (not changing actual write pointer)


        bytesWrittenCounter += amountThatCanBeWrittenInSingleIteration; // increment how much we have written
        bytesToWrite -= amountThatCanBeWrittenInSingleIteration; // decrement how much we have left to write
        buf += amountThatCanBeWrittenInSingleIteration; // increment where we are in the buf we are writing
        printf("writePointer location = %d\n", theOpenFileTable->table[fileID].writePointer );


        if(initialWritePointer > theINodeTable->table[INodeIndex].size) {
            theINodeTable->table[INodeIndex].size = initialWritePointer;
            printf("INodeSize = %d\n", theINodeTable->table[INodeIndex].size );
        }
        free(temporaryDiskData);
    }



    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) < 1) { //write indirect block to disk
        printf("%d\n", theINodeTable->table[INodeIndex].indirectINodePointer);

        write_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer, 1, theIndirectBlock);
    }

    // **********************************************************
    // In-memory Data Structure Initilizations and Writes to Disk
    // **********************************************************

    void *temporaryData;

    temporaryData=calloc(1,calculateSizeNeeded(sizeof(INodeTable)));


    memcpy(temporaryData, theINodeTable, sizeof(INodeTable));
    printf("Before write INodeSize: %d\n", theINodeTable->table[0].size);
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

    return bytesWrittenCounter; // return how much we wrote to disk


}
int sfs_fread(int fileID, char *buf, int length){
    printf(" \n ------Beginning to read-------- \n");
    printf("The file ID we are reading is: %d\n", fileID);
    printf("The Length of how much we are reading is: %d\n", length);
    printf("The file name we are reading is: %s\n", theDirectoryCache->theDirectory[fileID].fileName);
    if (length < 1) { // length invalid
        printf("Returned -1 because of invalid length\n");
        return -1; // error
    }

    if (checkIfSlotIsNotInUse(theOpenFileTable, fileID) == TRUE){ // file not open
        printf("Returned -1 because slot not in use\n");

        return -1; // error
    }

    int INodeIndex = theOpenFileTable->table[fileID].INode; // get iNodeIndex
    printf("The INodeIndex For this read is: %d\n", INodeIndex);


    int readPointerLocation = theOpenFileTable->table[fileID].readPointer; //get read pointer location
    printf("readpointerLocation = %d\n", readPointerLocation);
    printf("length = %d\n", length);
    printf("INodeSize = %d\n", theINodeTable->table[theOpenFileTable->table[fileID].INode].size);
    printf("INodeSize2 = %d\n", theINodeTable->table[INodeIndex].size);


    if(readPointerLocation + length > theINodeTable->table[theOpenFileTable->table[fileID].INode].size) // adjust read pointer if it is reading past file size
        length = theINodeTable->table[theOpenFileTable->table[fileID].INode].size - readPointerLocation;



    indirectBlock *theIndirectBlock; // declare and allocate indirect block struct just in case we need to use it
    theIndirectBlock = calloc(1, NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int));

    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, INodeIndex) == FALSE) { // check if we need to read from indirect block or no
        read_blocks(DATA_INDICIE+theINodeTable->table[INodeIndex].indirectINodePointer,1, theIndirectBlock); // copy information from disk to indirect struct
    }

    int initialBeginningBlock = calculateWhichBlock(readPointerLocation); // calculate which block we will start reading from
    int initialEndingBlock = calculateWhichBlock(readPointerLocation+length-1); // calculate which block we will read to

    int readCount = 0; // amount we have read
    int leftToReadCount = length; // amount we have left to read
    printf("Left to readcount is: %d\n", leftToReadCount);

    while (leftToReadCount > 0) { // loop until we have nothing left to read

        void *temporaryData;
        temporaryData = calloc(1, (initialEndingBlock - initialBeginningBlock +1)*BLOCK_SIZE); // declare and allocate buffer to deal with memory accesses to disk



        int beginningBlock = calculateWhichBlock(readPointerLocation); // calcualte which block we are in

        int posistionInBlock = calculateWhereInBlock(readPointerLocation); // calculate where in the actual block we are in


        int amountThatCanBeReadInSingleIteration; // calculate how much we can read in on one iteration

        if(leftToReadCount > BLOCK_SIZE - posistionInBlock){
            amountThatCanBeReadInSingleIteration = BLOCK_SIZE - posistionInBlock; // if it is past the block, reset the value that can be read in a single iteration
        }
        else {
            amountThatCanBeReadInSingleIteration = leftToReadCount;
        }


        if(checkIfPointerIsInIndirectBlock(beginningBlock) == TRUE) { // check if we are in the indirect block
            int indirectBlockIndex = calculateWhichIndirectBlock(beginningBlock);
            read_blocks(DATA_INDICIE+theIndirectBlock->indirectPointers[indirectBlockIndex],1, temporaryData); // read information on from indirect block
        } else {


        printf("\n \n The beginning block is: %d \n", beginningBlock);
        int pointerToDisk = theINodeTable->table[INodeIndex].iNodePointers[beginningBlock]; // calculate the poitner to disk from INode
        printf("\n \nThe pointer to disk is: %d\n", pointerToDisk);
        read_blocks(DATA_INDICIE+ pointerToDisk, 1, temporaryData);}


        printf("\n This is what temporary Disk Data has: %s\n\n", temporaryData);
        memcpy(buf, temporaryData + posistionInBlock, amountThatCanBeReadInSingleIteration); // copy information into the buffer
        printf("this is what buffer should have: %s\n", buf);
        theOpenFileTable->table[fileID].readPointer += amountThatCanBeReadInSingleIteration; // increment read pointer
        readCount += amountThatCanBeReadInSingleIteration; // increment amount we have read
        leftToReadCount -= amountThatCanBeReadInSingleIteration; // decrement how much we have left to read
        buf += amountThatCanBeReadInSingleIteration; // increment where in the buffer we are writing to
        printf("String Length of buf: %lu\n", strlen(buf));
        readPointerLocation += amountThatCanBeReadInSingleIteration;

        free(temporaryData);
    }

    printf("Readcount: %d \n", readCount);


    return readCount; // return how much we read


}
int sfs_remove(char *file){
    printf("\nEntering remove \n \n");
    int indexToRemoveFromDirectory = findIndexOfFileInDirectory(theDirectoryCache, file);
    if (indexToRemoveFromDirectory == -1) // check if the file even exists
        return -1; // return error if it doesnt
    int pointerToInode = theDirectoryCache->theDirectory[indexToRemoveFromDirectory].pointerToINode; // find the pointer to what iNode



    if (getIndexOfOpenFileTableForINode(*theOpenFileTable, pointerToInode) != -1) { //check if file is open
        sfs_fclose(pointerToInode); // if its close it
    }

    if(checkIfPointerToIndirectBlockIsEmpty(theINodeTable, pointerToInode) == FALSE || theINodeTable->table[pointerToInode].indirectINodePointer > -1) { //check if we are in the indirect pointer block
        indirectBlock *theIndirectBlock;
        theIndirectBlock = calloc(1, NUMBER_OF_INODE_INDIRECT_POINTERS* sizeof(int));
        read_blocks(DATA_INDICIE+theINodeTable->table[pointerToInode].indirectINodePointer,1, theIndirectBlock); // read information from indirect block
        int i;
        for (i=0; i<NUMBER_OF_INODE_INDIRECT_POINTERS; i++){ // set all information in indirect block to empty
            if(checkIfIndirectDataPointerIsEmpty(theIndirectBlock, i) == FALSE) {// *****
                if (testForStackCorruption(theIndirectBlock, i) == FALSE) {
                    printf("Location in indirectBlock: %d\n", theIndirectBlock->indirectPointers[i]);
                    void *temporaryData;
                    temporaryData = calloc(1, BLOCK_SIZE);
                    write_blocks(DATA_INDICIE + theIndirectBlock->indirectPointers[i], 1, temporaryData);
                    free(temporaryData);
                    printf("Location in free bitmap: %d\n", theBitMapBlock->block[theIndirectBlock->indirectPointers[i]]);
                    theBitMapBlock->block[theIndirectBlock->indirectPointers[i]] = FALSE;
                    theIndirectBlock->indirectPointers[i] = NOT_DEFINED;
                }
                else {
                    theIndirectBlock->indirectPointers[i] = NOT_DEFINED; // set to undefined
                    printf(" \n Warning Stack Corrupted \n"); // reading past size of disk (error handling)
                }
            }
        }
    }

    eraseAllINodeBlockPointerData(theINodeTable, theBitMapBlock, pointerToInode); // erase all information on a given INode

    setPosistionInInodeTableToOpen(theINodeTable, pointerToInode); //set posistion in INode table to open
    setNewINode(&theINodeTable->table[pointerToInode]); // set that pointer as a new INode since we are removing

    markSpecificPosistionInDirectoryAsOpen(theDirectoryCache, indexToRemoveFromDirectory); // open up space in the Directory Cache
    theDirectoryCache->theDirectory[indexToRemoveFromDirectory].pointerToINode = NOT_DEFINED; // set the INode Information as undefined
    strcpy(theDirectoryCache->theDirectory[indexToRemoveFromDirectory].fileName, ""); // set the filename as empty

    // **********************************************************
    // In-memory Data Structure Initilizations and Writes to Disk to Free information
    // **********************************************************

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

  return 0; // return success
}
