#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "BufferV2.h"

using namespace std;

typedef struct {
	int tupleSize;
	int attrNumber;
	int* attrTypes;
	unsigned int firstTupleFileBlockID;	// first tuple address
	unsigned int firstTupleOffset;
	unsigned int lastTupleFileBlockID;	// last tuple address
	unsigned int lastTupleOffset;
	unsigned int lastPosFileBlockID;	// first empty address
	unsigned int lastPosOffset;
}RecordFileHeader;


typedef struct {
	int type;
	void* value;
}Attribute;


typedef struct {
	unsigned int selfFileBlockID;
	unsigned int selfOffset;
	unsigned int nextTupleFileBlockID;	// =0 means no next tuple
	unsigned int nextTupleOffset;
	unsigned int preTupleFileBlockID;	// =0 means no previous tuple
	unsigned int preTupleOffset;		
}RecordTuple;

class RecordManager {
public:
	RecordManager();
	~RecordManager();
	bool CreateRecordFile(unsigned int fileIDValue, int* attrTypeList, int attrNumValue);
	bool OpenRecordFile(unsigned int fileIDValue);	// You must Open the Record File before doing any functions on it
	bool DropRecordFile(unsigned int fileIDValue);	// lazy delete
	
	// operation 0:"=", 1=">", 2="<", 3=">=", 4="<=", 5:"!="
	bool SearchTupleByCondition(unsigned int attrIndex, string keyValue, unsigned int operation, 
								vector<unsigned int> &tupleFileBlockID, vector<unsigned int> &tupleOffset); 
	bool InsertTuple(vector<string> attrValues, unsigned int &selfFileBlockID, unsigned int &selfOffset);
	bool DeleteTuple(unsigned int attrIndex, string keyValue, unsigned int operation);
	bool DeleteTupleByAddress(vector<unsigned int> tupleFileBlockID, vector<unsigned int> tupleOffset);
	void OutputTuples(vector<unsigned int> tupleFileBlockID, vector<unsigned int> tupleOffset, unsigned int width);
	void OutputTuples(unsigned int tupleFileBlockID, unsigned int tupleOffset, unsigned int width);
	string GetKeyValue(unsigned int tupleFileBlockID, unsigned int tupleOffset, unsigned int attrIndex);
	void GetAllTuples(vector<unsigned int> & keyBlockFileID, vector<unsigned int> &keyOffset);

private:
	unsigned int curFileID;
	LocPtr* LPPtr;	// local position pointer
	RecordFileHeader* curHeaderPtr;

	bool ReadRecordHeader(unsigned int fileIDValue);
	bool FindEmptyPosToWriteTuple();	// adjust the curHeaderPtr->lastPos
	void WriteRecordTuple(LocPtr* LPPtr, RecordTuple* tuplePtr, vector<string> attrValues);
	void WriteRecordTuple(LocPtr* LPPtr, RecordTuple* tuplePtr);
	RecordTuple* ReadRecordTuple(LocPtr* LPPtr);
	RecordTuple* ReadRecordTuple(LocPtr* LPPtr, vector<string> &attrValues);

};

extern RecordManager MyRM;