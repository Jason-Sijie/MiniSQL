#ifndef CATALOG 
#define CATALOG 1

#include <iostream>
#include <string>
#include <vector>
#include "BufferV2.h"
#include "Interpreter.h"

using namespace std;


class Catalog
{
public:
	/*===������===*/
	//Find����������string��return -1ʧ�ܣ�returnvalue = ������table�е�λ��
	void FindTable(string tableName);
	void FindIndex(string indexName);
	void FindAttribute(string attributeName, string tableName);
	
	//Delete����
	void DeleteOldTable(string tableName);
	void DeleteOldIndex(string indexName);
	void DeleteOldIndex(string attributeName, string tableName);
	
	//Create���� table��index
	bool CreateNewTable(string tableName);
	void CreateNewIndex(string indexName, string tableName, string attributeName);
	
	// Get table information
	int* GetAttributeList(string tableName);
	int GetAttributeType(string tableName, string attributeName);
	string FindIndexName(string tableName, string attributeName);
	
	int tableResult;		//��table�Ľ��
	int indexResult;		//��index�Ľ��
	int attributeResult;	//��att�Ľ��

	Table TableCatalog[MAXTABLENUM];
	Index IndexCatalog[MAXINDEXNUM];
private:
	/*===���ݲ�===*/
	int* attributeLengthCatalog;
	void InitTableCatalog(Table &table);
	void InitIndexCatalog(Index &index);
};

extern Catalog myCatalog;

#endif