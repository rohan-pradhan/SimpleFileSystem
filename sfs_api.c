#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sfs_api.h"
#include "OpenFile.h"
#include "disk_emu.h"
#include "SuperBlock.h"

#define SB_INDICIE 0
#define INODE_INDICIE 1
#define DIRECTORY_INDICIE 40
#define DATA_INDICIE 50
#define FREE_BIT_MAP_INDICIE 350
#define FS "RohanPradhan.fs"

superBlock theSuperBlock = {};


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
      printf("New disk!!!\n");
      printf("%d",theSuperBlock.magicNumber);



  }
    else
      read_blocks(SB_INDICIE,1,&theSuperBlock);
    printf("Old disk1!!!");
    printf("%d", theSuperBlock.magicNumber);

}
int sfs_get_next_file_name(char *fname){
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
