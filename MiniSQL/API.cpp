#include"API.h"

#define DEGREE 10

extern IndexManager MyIM;
extern RecordManager MyRM;

int main() {
	// login page

	// prompt line

	while (1) {
		vector<string> cmd;
		string oneLine;
		string oneCmd;
		cout << "MiniSQL>>";
		do {
			// init oneLine;
			oneLine = "";
			getline(cin, oneLine);
			cmd.push_back(oneLine);
		} while (!oneLine.empty() && oneLine.back() != ';');

		for (auto i = cmd.begin(); i != cmd.end(); i++) {
			oneCmd += *i;
		}

		myInter.initInterpreter();
		int flag = myInter.Parse(oneCmd);
		COMMAND cmdType = myInter.command;
		switch (cmdType) {
		case CREATETABLE: if(flag)onCreateTable(); 
			break;
		case CREATEINDEX: if (flag)onCreateIndex();
			break;
		case DROPTABLE: if (flag)onDropTable();
			break;
		case DROPINDEX: if (flag)onDropIndex();
			break;
		case SELECT: if (flag)onSelect();
			break;
		case DELETE: if (flag) onDelete();
			break;
		case INSERT: if (flag) onInsert();
			break;
		default:
			break;
		}
	}
}


void onCreateTable() {
	// transfer
	string newTableName = myInter.commandTableName;
	if (!myCatalog.CreateNewTable(newTableName))
	{
		cout << "Create table fails! The table already exists!" << endl << endl;
		return;
	}
	int pos = myCatalog.tableResult;
	myCatalog.TableCatalog[pos] = myInter.commandTable;

	// Create Record File
	Table* tablePtr = &myCatalog.TableCatalog[pos];
	File* myFilePtr = MyDB[newTableName.c_str()];
	unsigned int recordFileID = myFilePtr->GetFileID();
	int* attrList = myCatalog.GetAttributeList(newTableName);
	int attrNum = tablePtr->attributeNum;

	if (MyRM.CreateRecordFile(recordFileID, attrList, attrNum))
	{
		cout << "create table " << newTableName << " success!" << endl << endl;
	}
	// Create Index on primary key
	string attrName = tablePtr->tableAttribute[tablePtr->GetPrimaryKeyPosition()].attributeName;
	string indexName = newTableName + attrName;
	myCatalog.CreateNewIndex(indexName, newTableName, attrName);
	myFilePtr = MyDB[indexName.c_str()];
	unsigned int indexFileID = myFilePtr->GetFileID();
	int type = tablePtr->tableAttribute[tablePtr->GetPrimaryKeyPosition()].attributeType;

	if (MyIM.CreateIndexFile(indexFileID, type, DEGREE))
	{
		cout << "Create index on primary key success!" << endl << endl;
	}
	else
	{
		cout << "Fail to create index on primary key !" << endl << endl;
	}

}

void onCreateIndex() {
	string tableName = myInter.commandTableName;
	string indexAttrName = myInter.commandAtttributeName;
	string newIndexName = myInter.commandIndexName;
	myCatalog.CreateNewIndex(newIndexName, tableName, indexAttrName);
	int pos = myCatalog.indexResult;
	myCatalog.IndexCatalog[pos] = myInter.commandIndex;

	// Create Index File
	File* myFilePtr = MyDB[newIndexName.c_str()];
	unsigned int indexFileID = myFilePtr->GetFileID();
	int type = myCatalog.GetAttributeType(tableName, indexAttrName);

	if (MyIM.CreateIndexFile(indexFileID, type, DEGREE))
	{
		cout << "create index "<< newIndexName <<" success!" << endl << endl;
	}
	else
	{
		cout << "Fail to create index: " << newIndexName << " !" << endl << endl;
	}

	// Insert node into index file
	myCatalog.FindAttribute(indexAttrName, tableName);
	Table* tablePtr = &myCatalog.TableCatalog[myCatalog.tableResult];
	unsigned int attrIndex;
	for(attrIndex = 0; attrIndex<tablePtr->attributeNum; attrIndex++)
	{
		if (tablePtr->tableAttribute[attrIndex].attributeName == indexAttrName)
			break;
	}

	myFilePtr = MyDB[tableName.c_str()];
	unsigned int recordFileID = myFilePtr->GetFileID();
	vector<unsigned int> keyBlockFileID, keyOffset;
	MyRM.OpenRecordFile(recordFileID);
	MyRM.GetAllTuples(keyBlockFileID, keyOffset);
	for(int i = 0; i<keyBlockFileID.size(); i++)
	{
		string keyValue = MyRM.GetKeyValue(keyBlockFileID[i], keyOffset[i], attrIndex);
		onInsertThroughIndex(indexFileID, keyValue, keyBlockFileID[i], keyOffset[i]);
	}

}

void onDropTable() {
	string tableName = myInter.commandTableName;
	myCatalog.FindTable(tableName);
	if (myCatalog.tableResult == -1)
	{
		cout << "Can not find the corresponding table!" << endl << endl;
		return;
	}

	// Drop record file
	File* myFilePtr = MyDB[tableName.c_str()];
	unsigned int recordFileID = myFilePtr->GetFileID();
	if (MyRM.DropRecordFile(recordFileID))
	{
		cout << "Drop table: " << tableName << " success!" << endl << endl;
	}
	// Drop catalog index
	myCatalog.FindTable(tableName);
	Table* tablePtr = &myCatalog.TableCatalog[myCatalog.tableResult];
	for (int i = 0; i < tablePtr->attributeNum; i++)
	{
		if (tablePtr->tableAttribute[i].ifIndex)
		{
			myCatalog.DeleteOldIndex(tablePtr->tableAttribute[i].attributeName, tableName);
		}
	}
	// Drop catalog record
	myCatalog.DeleteOldTable(tableName);
}

void onDropIndex() {
	string indexName = myInter.commandIndexName;
	myCatalog.FindIndex(indexName);
	if (myCatalog.indexResult == -1)
	{
		cout << "Can not find the corresponding index!" << endl << endl;
		return;
	}
	string tableName = myInter.commandTableName;
	myCatalog.DeleteOldIndex(indexName);
	cout << "Drop the index " << indexName << " success!" << endl << endl;
}

void onSelect() {
	string tableName = myInter.commandTableName;
	myCatalog.FindTable(tableName);
	if (myCatalog.tableResult == -1)
	{
		cout << "Can not find the corresponding table!" << endl << endl;
		return;
	}
	myCatalog.FindTable(tableName);
	Table* tablePtr = &myCatalog.TableCatalog[myCatalog.tableResult];
	vector<Condition> conditions = myInter.commandCondition;
	vector<unsigned int> keyFileBlockID, keyOffset;
	bool* isSelect = new bool[conditions.size()];

	// Find address with index
	for (int i = 0; i<conditions.size(); i++) 
	{
		isSelect[i] = false;
		string indexName = myCatalog.FindIndexName(tableName, conditions[i].attributeName);
		myCatalog.FindIndex(indexName);
		if (myCatalog.indexResult != -1 && !indexName.empty()) 
		{ // found such an index
			File* myFilePtr = MyDB[indexName.c_str()];
			unsigned int indexFileID = myFilePtr->GetFileID();

			onSelectThroughIndex(indexFileID, conditions[i].Value, conditions[i].operation, 
									keyFileBlockID, keyOffset);
			isSelect[i] = true;
		}
	}

	// Find address without index
	for (int i = 0; i < conditions.size(); i++)
	{
		if (isSelect[i] == false)
		{
			unsigned int pos;
			for (pos = 0; pos < tablePtr->attributeNum; pos++)	// find the position of the attribute
			{
				if (conditions[i].attributeName == tablePtr->tableAttribute[pos].attributeName)
					break;
			}
			File* myFilePtr = MyDB[tableName.c_str()];
			unsigned int recordFileID = myFilePtr->GetFileID();

			onSelectThroughRecord(recordFileID, pos, conditions[i].Value, conditions[i].operation, keyFileBlockID, keyOffset);
		}
	}

	// Output the result
	bool *isOutput = new bool[tablePtr->attributeNum];
	for(int i = 0; i<tablePtr->attributeNum; i++)	// find the outputing lines
	{
		isOutput[i] = false;
		for(int j = 0; j<myInter.commandAttribute.size(); j++)
		{
			if(tablePtr->tableAttribute[i].attributeName == myInter.commandAttribute[j].attributeName)
			{
				isOutput[i] = true;
				break;
			}
		}
	}
	int width = 10;
	for (int j = 0; j<tablePtr->attributeNum; j++)
	{
		if(isOutput[j] == true)
		{
			cout << setw(width) << tablePtr->tableAttribute[j].attributeName;
			if (tablePtr->tableAttribute[j].attributeType > width)
				width = tablePtr->tableAttribute[j].attributeType;
		}
	}
	cout << endl;
	for (int i = 0; i<keyFileBlockID.size(); i++)
	{
		for(int j = 0; j<tablePtr->attributeNum; j++)
		{
			if(isOutput[j] == true)
			{
				string keyValue = MyRM.GetKeyValue(keyFileBlockID[i], keyOffset[i], j);
				cout << setw(width) << keyValue;
			}
		}
		cout << endl;
	}
	cout << endl;
}

void onDelete() {
	string tableName = myInter.commandTableName;
	myCatalog.FindTable(tableName);
	if (myCatalog.tableResult == -1)
	{
		cout << "Can not find the corresponding table!" << endl << endl;
		return;
	}
	myCatalog.FindTable(tableName);
	Table* tablePtr = &myCatalog.TableCatalog[myCatalog.tableResult];
	vector<Condition> conditions = myInter.commandCondition;
	vector<unsigned int> keyFileBlockID, keyOffset;

	// find the deleting tuples' address
	for (int i = 0; i < conditions.size(); i++)
	{
		unsigned int pos;
		for (pos = 0; pos < tablePtr->attributeNum; pos++)	// find the position of the attribute
		{
			if (conditions[i].attributeName == tablePtr->tableAttribute[pos].attributeName)
				break;
		}
		File* myFilePtr = MyDB[tableName.c_str()];
		unsigned int recordFileID = myFilePtr->GetFileID();

		onSelectThroughRecord(recordFileID, pos, conditions[i].Value, conditions[i].operation, keyFileBlockID, keyOffset);
	}
	
	// delete the indexes
	for (int k = 0; k < keyFileBlockID.size(); k++)
	{
		for (int i = 0; i < tablePtr->attributeNum; i++)
		{
			if (tablePtr->tableAttribute[i].ifIndex)
			{
				string keyValue = MyRM.GetKeyValue(keyFileBlockID[k], keyOffset[k], i);
				string indexName = myCatalog.FindIndexName(tableName, tablePtr->tableAttribute[i].attributeName);
				File* myFilePtr = MyDB[indexName.c_str()];
				unsigned int indexFileID = myFilePtr->GetFileID();
				MyIM.OpenIndexFile(indexFileID);
				MyIM.DeleteKey(keyValue);
			}
		}
	}
	// delete the tuples
	MyRM.DeleteTupleByAddress(keyFileBlockID, keyOffset);

	cout << "Delete " << keyFileBlockID.size() <<" tuple successfully!" << endl << endl;
	return;
}

void onInsert() {
	string tableName = myInter.commandTableName;
	myCatalog.FindTable(tableName);
	if(myCatalog.tableResult == -1)
	{
		cout << "Can not find the corresponding table!" << endl << endl;
		return;
	}
	Item & item = myInter.commandItem;
	vector<string> attrValues;
	for (auto it = item.columns.begin(); it != item.columns.end(); it++)
	{
		attrValues.push_back(*it);
	}
	// Insert into the record
	File* myFilePtr = MyDB[tableName.c_str()];
	unsigned int recordFileID = myFilePtr->GetFileID();
	unsigned int keyFileBlockID, keyOffset;
	onInsertThroughRecord(recordFileID, attrValues, keyFileBlockID, keyOffset);

	// Insert into the index file
	myCatalog.FindTable(tableName);
	Table* tablePtr = &myCatalog.TableCatalog[myCatalog.tableResult];
	for (int i = 0; i < tablePtr->attributeNum; i++)	// Find all the indexes in the table
	{
		if (tablePtr->tableAttribute[i].ifIndex)
		{
			string indexName = myCatalog.FindIndexName(tableName, tablePtr->tableAttribute[i].attributeName);
			myFilePtr = MyDB[indexName.c_str()];
			unsigned int indexFileID = myFilePtr->GetFileID();
			string keyValue = item.columns[i];
			
			onInsertThroughIndex(indexFileID, keyValue, keyFileBlockID, keyOffset);
		}
	}

	cout << "insert success!" << endl << endl;
}

bool onSelectThroughIndex(unsigned int indexFileID, string keyValue, OPERATION operation,
							vector<unsigned int> &keyFileBlockID, vector<unsigned int>& keyOffset)
{
	MyIM.OpenIndexFile(indexFileID);
	vector<unsigned int> tempFileBlockID, tempOffset, resultFileBlockID, resultOffset;

	switch (operation)
	{
	case EQUAL:MyIM.FindAddressByCondition(keyValue, 0, tempFileBlockID, tempOffset);
		break;
	case NOT_EQUAL:MyIM.FindAddressByCondition(keyValue, 5, tempFileBlockID, tempOffset);
		break;
	case LESS:MyIM.FindAddressByCondition(keyValue, 2, tempFileBlockID, tempOffset);
		break;
	case MORE:MyIM.FindAddressByCondition(keyValue, 1, tempFileBlockID, tempOffset);
		break;
	case LESS_EQUAL:MyIM.FindAddressByCondition(keyValue, 4, tempFileBlockID, tempOffset);
		break;
	case MORE_EQUAL:MyIM.FindAddressByCondition(keyValue, 3, tempFileBlockID, tempOffset);
		break;
	default:
		break;
	}

	if (keyFileBlockID.size() == 0)
	{
		keyFileBlockID = tempFileBlockID;
		keyOffset = tempOffset;
	}
	for (auto it1 = keyFileBlockID.begin(), it2 = keyOffset.begin(); it1 != keyFileBlockID.end(); it1++, it2++)
	{
		for (auto j1 = tempFileBlockID.begin(), j2 = tempOffset.begin(); j1 != tempFileBlockID.end(); j1++, j2++)
		{
			if (*it1 == *j1 && *it2 == *j2)
			{
				resultFileBlockID.push_back(*it1);
				resultOffset.push_back(*it2);
			}
		}
	}
	keyFileBlockID = resultFileBlockID;
	keyOffset = resultOffset;
	return true;
}

bool onSelectThroughRecord(unsigned int recordFileID, unsigned int attrIndex, string keyValue, OPERATION operation, 
							vector<unsigned int>& keyFileBlockID, vector<unsigned int>& keyOffset)
{
	MyRM.OpenRecordFile(recordFileID);
	vector<unsigned int> tempFileBlockID, tempOffset, resultFileBlockID, resultOffset;
	switch (operation)
	{
	case EQUAL:MyRM.SearchTupleByCondition(attrIndex, keyValue, 0, tempFileBlockID, tempOffset);
		break;
	case NOT_EQUAL:MyRM.SearchTupleByCondition(attrIndex, keyValue, 5, tempFileBlockID, tempOffset);
		break;
	case LESS:MyRM.SearchTupleByCondition(attrIndex, keyValue, 2, tempFileBlockID, tempOffset);
		break;
	case MORE:MyRM.SearchTupleByCondition(attrIndex, keyValue, 1, tempFileBlockID, tempOffset);
		break;
	case LESS_EQUAL:MyRM.SearchTupleByCondition(attrIndex, keyValue, 4, tempFileBlockID, tempOffset);
		break;
	case MORE_EQUAL:MyRM.SearchTupleByCondition(attrIndex, keyValue, 3, tempFileBlockID, tempOffset);
		break;
	default:
		break;
	}
	if (keyFileBlockID.empty())
	{
		keyFileBlockID = tempFileBlockID;
		keyOffset = tempOffset;
	}

	for (auto it1 = keyFileBlockID.begin(), it2 = keyOffset.begin(); it1 != keyFileBlockID.end(); it1++, it2++)
	{
		for (auto j1 = tempFileBlockID.begin(), j2 = tempOffset.begin(); j1 != tempFileBlockID.end(); j1++, j2++)
		{
			if (*it1 == *j1 && *it2 == *j2)
			{
				resultFileBlockID.push_back(*it1);
				resultOffset.push_back(*it2);
			}
		}
	}
	keyFileBlockID = resultFileBlockID;
	keyOffset = resultOffset;
	return true;
}

bool onInsertThroughRecord(unsigned int recordFileID, vector<string> attrValues, unsigned int & keyFileBlockID, unsigned int & keyOffset)
{
	MyRM.OpenRecordFile(recordFileID);
	if (MyRM.InsertTuple(attrValues, keyFileBlockID, keyOffset))
		return true;
	else
		return false;
}

bool onInsertThroughIndex(unsigned int indexFileID, string keyValue, unsigned int keyFileBlockID, unsigned int keyOffset)
{
	MyIM.OpenIndexFile(indexFileID);
	MyIM.InsertKey(keyValue, keyFileBlockID, keyOffset);
	return true;
}



