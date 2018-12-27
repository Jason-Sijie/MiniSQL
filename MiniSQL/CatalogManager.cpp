//ͳ����Ϣ��ʵʱ�仯��
//���ң�����λ��
//create/select table api����table��û�У�û�еĻ����ؿյ�FilesID, �еĻ���������


#include <string>
#include <iostream>
#include <vector>
#include "CatalogManager.h"

using namespace std;

Catalog myCatalog;

void Catalog::FindTable(string tableName)
{
	int result = -1;
	for (int i = 0; i < MAXTABLENUM; i++)
	{
		if (TableCatalog[i].tableName == tableName)
		{
			result = i;
		}
	}
	tableResult = result;
}

void Catalog::FindIndex(string indexName)
{
	int result = -1;
	string curTableName;
	for (int i = 0; i < MAXINDEXNUM; i++)
	{
		if (IndexCatalog[i].indexName == indexName)
		{
			result = i;
			curTableName = IndexCatalog[i].indexTableName;
			FindTable(curTableName);			//��tableResultҲ�ı���
		}
	}
	indexResult = result;
}

void Catalog::FindAttribute(string attributeName, string tableName)
{
	attributeResult = -1;
	FindTable(tableName);
	for (int i = 0; i < TableCatalog[tableResult].tableAttribute.size(); i++)
	{
		if (TableCatalog[tableResult].tableAttribute[i].attributeName == attributeName)
		{
			attributeResult = i;
		}
	}
}

bool Catalog::CreateNewTable(string tableName)
{
	for (int i = 0; i < MAXTABLENUM; i++)
	{
		if (TableCatalog[i].tableName == tableName)
		{
			tableResult = -1;
			return false;
		}
	}
	for (int i = 0; i < MAXTABLENUM; i++)
	{
		if (TableCatalog[i].tableName.empty())
		{
			tableResult = i;
			break;
		}
	}
	return true;
}

int * Catalog::GetAttributeList(string tableName)
{
	FindTable(tableName);
	if (tableResult < 0)
		return nullptr;
	Table* tablePtr = &TableCatalog[tableResult];
	int* attrList = new int[tablePtr->attributeNum];
	for (int i = 0; i < tablePtr->attributeNum; i++)
	{
		attrList[i] = tablePtr->tableAttribute[i].attributeType;
	}
	
	return attrList;
}

int Catalog::GetAttributeType(string tableName, string attributeNameValue)
{
	FindTable(tableName);
	for (int i = 0; i < TableCatalog[tableResult].attributeNum; i++)
	{
		if (TableCatalog[tableResult].tableAttribute[i].attributeName == attributeNameValue)
			return TableCatalog[tableResult].tableAttribute[i].attributeType;
	}
	return 0;
}

string Catalog::FindIndexName(string tableName, string attributeName)
{
	for (int i = 0; i < MAXINDEXNUM; i++)
	{
		if (IndexCatalog[i].indexTableName == tableName && IndexCatalog[i].indexAttributeName == attributeName)
		{
			return IndexCatalog[i].indexName;
		}
	}
	return "";
}


void Catalog::CreateNewIndex(string indexName, string tableName, string attributeName)
{
	//������⣬��index�ܷ�ɹ�����
	FindAttribute(attributeName, tableName);
	if (attributeResult == -1)
	{
		cout << "No such attribute" << endl;
		return;
	}
	if (tableResult == -1)
	{
		cout << "No such table" << endl;
		return;
	}
	if (TableCatalog[tableResult].tableAttribute[attributeResult].ifUnique != true)
	{
		cout << "The attribute is not unique! " << endl;
		return;
	}
	else
	{
		TableCatalog[tableResult].tableAttribute[attributeResult].ifIndex = true;
	}
	int curType = TableCatalog[tableResult].tableAttribute[attributeResult].attributeType;
	for (int i = 0; i < MAXINDEXNUM; i++)
	{
		if (IndexCatalog[i].indexName.empty() == 1)
		{
			indexResult = i;
		}
	}
	IndexCatalog[indexResult].indexAttributeName = attributeName;
	IndexCatalog[indexResult].indexName = indexName;
	IndexCatalog[indexResult].indexTableName = tableName;

}

void Catalog::DeleteOldTable(string tableName)
{
	FindTable(tableName);
	InitTableCatalog(TableCatalog[tableResult]);
}

void Catalog::DeleteOldIndex(string indexName)
{
	FindIndex(indexName);
	FindAttribute(IndexCatalog[indexResult].indexAttributeName, IndexCatalog[indexResult].indexTableName);
	TableCatalog[tableResult].tableAttribute[attributeResult].ifIndex = false;
	InitIndexCatalog(IndexCatalog[indexResult]);
}

void Catalog::DeleteOldIndex(string attributeName, string tableName)
{
	string indexName;
	for (int i = 0; i < MAXINDEXNUM; i++)
	{
		if (IndexCatalog[i].indexTableName == tableName && IndexCatalog[i].indexAttributeName == attributeName)
		{
			DeleteOldIndex(IndexCatalog[i].indexName);
		}
	}
}


void Catalog::InitTableCatalog(Table &table)
{
	table.attributeNum = 0;
	table.primarykeyLocation = -1;
	table.tableName = "";
	table.totalLength.clear();
	table.tableAttribute.clear();
}

void Catalog::InitIndexCatalog(Index &index)
{
	index.indexName = "";
	index.indexTableName = "";
	index.indexAttributeName = "";
}

