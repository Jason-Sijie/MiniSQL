#ifndef API
#define API 1

#include"IndexManager.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include<iostream>
#include<vector>

using namespace std;

void onCreateTable();
void onCreateIndex();
void onDropTable();
void onDropIndex();
void onSelect();
void onDelete();
void onInsert();
void onError();


bool onSelectThroughIndex(unsigned int indexFileID, string keyValue, OPERATION operation,
							vector<unsigned int> &keyFileBlockID, vector<unsigned int>& keyOffset);
bool onSelectThroughRecord(unsigned int recordFileID, unsigned int attrIndex, string keyValue, OPERATION operation,
							vector<unsigned int> &keyFileBlockID, vector<unsigned int>& keyOffset);
bool onInsertThroughRecord(unsigned int recordFileID, vector<string> attrValues, 
							unsigned int & keyFileBlockID, unsigned int &keyOffset);
bool onInsertThroughIndex(unsigned int indexFileID, string keyValue,
							unsigned int keyFileBlockID, unsigned int keyOffset);


#endif
