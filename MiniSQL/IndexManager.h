#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H 1

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include "BufferV2.h"

using namespace std;

typedef struct {
    int type;
    int degree;
    int nodeLength;
    unsigned int rootFileBlockID;
	unsigned int rootOffset;
	unsigned int headLeafFileBlockID;
	unsigned int headLeafOffset;
	unsigned int lastPosFileBlockID;
	unsigned int lastPosOffset;
}IndexFileHeader;


typedef struct{
	unsigned int selfFileBlockID;
	unsigned int selfOffset;
	unsigned int parentFileBlockID;
	unsigned int parentOffset;
	bool    isLeaf;
    int     childOnNode;
    int     keyOnNode;
    vector<string>      keys;           // keys' value    
    unsigned int*    childPosBlock;  // children's location info
    unsigned int*    childPosOffset;
} IndexNode;


class IndexManager{
public:
    IndexManager();
    ~IndexManager();
    bool CreateIndexFile(unsigned int fileIDValue, int type, int degree);
	bool OpenIndexFile(unsigned int fileIDValue);	// Must Open the file before doing any operation in it	
	bool DropIndex(unsigned int fileIDValue);

	IndexNode* AllocateIndexNode();
	bool FindKey(IndexNode* nodePtr, string keyValue, int &index);	// Find the appropriate index for a keyValue in one tree node
	IndexNode* FindToLeaf(IndexNode* nodePtr, string keyValue, int &index,  bool& isFind);	// Find the position of key in the leaf node
	bool FindAddress(string keyValue, unsigned int &keyFileBlockID, unsigned int &keyOffset,
						unsigned int &selfFileBlockID, unsigned int &selfOffset, int &index);
	// 0:"=", 1:">", 2:"<", 3:">=", 4:"<=", 5"!="
	bool FindAddressByCondition(string keyValue, int operation, vector<unsigned int> &keyFileBlockID, vector<unsigned int> &keyOffset);
	bool InsertKey(string keyValue, int keyBlock, int keyOffset);
	bool DeleteKey(string keyValue);

private:
    IndexFileHeader* curHeaderPtr;  // store the current file header information
	LocPtr* LPPtr;
	unsigned int curFileID;

	// helping functions
	bool ReadIndexHeader(unsigned int fileIDValue);
	IndexNode* ReadIndexNode(LocPtr* curLPPtr);
	void WriteIndexNode(LocPtr* curLPPtr, IndexNode *nodePtr);
	IndexNode* SplitIndexNode(IndexNode* &nodePtr);
	void AdjustAfterInsert(IndexNode* nodePtr);
	void AdjustAfterDelete(IndexNode* nodePtr);
};

extern IndexManager MyIM;

#endif