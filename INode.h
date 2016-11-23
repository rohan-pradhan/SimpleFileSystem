//
// Created by Rohan Pradhan on 11/22/16.
//

#ifndef ASSIGNMENT_3_INODE_H
#define ASSIGNMENT_3_INODE_H
#include <setjmp.h>
#define NUMBER_OF_INODE_DIRECT_POINTERS 12
#define NUMBER_OF_INODES 40
#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) ){
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1)

typedef struct inode {
    int mode;
    int linkCnt;
    int uid;
    int gid;
    int size;
    unsigned iNodePointers[NUMBER_OF_INODE_DIRECT_POINTERS];
    unsigned indirectINodePointer;
}INode;

void copyINodeInformation(INode *original, INode *copy) {
    copy->mode = original->mode;
    copy->linkCnt = original->linkCnt;
    copy->uid = original->uid;
    copy->gid = original->gid;
    copy->size = original->size;
    for (int i = 0; i <NUMBER_OF_INODE_DIRECT_POINTERS; i++){
        copy->iNodePointers[i] = original->iNodePointers[i];
    }
    copy->indirectINodePointer = original->indirectINodePointer;
 }






void setNewINode(INode* INodeToSetup) {
    INodeToSetup->mode = 0;
    INodeToSetup->linkCnt = 0;
    INodeToSetup->uid = 0;
    INodeToSetup->gid = 0;
    INodeToSetup->size = 0;

    int i = 0;
    while (i <= NUMBER_OF_INODE_DIRECT_POINTERS && INodeToSetup->iNodePointers[i] != -1) {
        INodeToSetup->iNodePointers[i] = -1;
        i++;
    }
    INodeToSetup->indirectINodePointer = -1;

}


typedef struct inodeTable {
    INode table[NUMBER_OF_INODES];

} INodeTable;





#endif //ASSIGNMENT_3_INODE_H
