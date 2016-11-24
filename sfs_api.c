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



#define FS "RohanPradhan.fs"

superBlock theSuperBlock = {};
INodeTable *theINodeTable = NULL;
directoryCache *theDirectoryCache = NULL;
openFileTable *theOpenFileTable = NULL;
bitmapBlock *theBitMapBlock = NULL;


void mksfs(int fresh){
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


      char *test="Feeeeeeeeeeeeeed.txt";
      int testVar = validateFileName(test);
      printf("%d\n", testVar);


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



    printf("Old disk1!!!");
    printf("%d", theSuperBlock.magicNumber);

}
int sfs_get_next_file_name(char *fname){
    int indexOfCurrentPosistion = theDirectoryCache->posistionInDirectory;
    int i =0;
    while(indexOfCurrentPosistion+i < 40){
        int adjustedIndex = indexOfCurrentPosistion+i;
        if (checkIfPosistionInDirectoryIsUsed(theDirectoryCache,adjustedIndex)) {
            strcpy(fname, theDirectoryCache->theDirectory[adjustedIndex].fileName);
            return 1;
        }
        else
            i++;
    }
    theDirectoryCache->posistionInDirectory = 0;
  return 0;
}
int sfs_get_file_size(char* path){
  return 0;
}
int sfs_fopen(char *name){
  return 0;
}
int sfs_fclose(int fileID){
  return 0;
}
int sfs_frseek(int fileID, int loc){
  return 0;
}
int sfs_fwseek(int fileID, int loc){
  return 0;
}
int sfs_fwrite(int fileID, char *buf, int length){
  return 0;
}
int sfs_fread(int fileID, char *buf, int length){
  return 0;
}
int sfs_remove(char *file){
  return 0;
}
