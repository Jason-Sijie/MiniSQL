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
	/*===方法层===*/
	//Find方法，输入string，return -1失败，returnvalue = 在整体table中的位置
	void FindTable(string tableName);
	void FindIndex(string indexName);
	void FindAttribute(string attributeName, string tableName);
	
	//Delete方法
	void DeleteOldTable(string tableName);
	void DeleteOldIndex(string indexName);
	void DeleteOldIndex(string attributeName, string tableName);
	
	//Create方法 table和index
	bool CreateNewTable(string tableName);
	void CreateNewIndex(string indexName, string tableName, string attributeName);
	
	// Get table information
	int* GetAttributeList(string tableName);
	int GetAttributeType(string tableName, string attributeName);
	string FindIndexName(string tableName, string attributeName);
	
	int tableResult;		//找table的结果
	int indexResult;		//找index的结果
	int attributeResult;	//找att的结果

	Table TableCatalog[MAXTABLENUM];
	Index IndexCatalog[MAXINDEXNUM];
private:
	/*===数据层===*/
	int* attributeLengthCatalog;
	void InitTableCatalog(Table &table);
	void InitIndexCatalog(Index &index);
};

extern Catalog myCatalog;

#endif