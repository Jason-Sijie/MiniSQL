#include "RecordManager.h"
extern DB MyDB;
RecordManager MyRM;

RecordManager::RecordManager()
{
	curHeaderPtr = nullptr;
	LPPtr = nullptr;
	curFileID = 0;
}

RecordManager::~RecordManager()
{
	delete curHeaderPtr;
}

bool RecordManager::CreateRecordFile(unsigned int fileIDValue, int* attrTypeList, int attrNumValue)
{
	if (curHeaderPtr != nullptr && LPPtr->fileID == curFileID)
	{
		// write back the RecordFileHeader
		LPPtr->fileBlockID = 0;
		LPPtr->offset = 0;		// locate the LPPtr to the start of RecordHeader
		LPPtr->WriteIntoBuffer(&curHeaderPtr->tupleSize, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->attrNumber, 0);
		LPPtr->ShiftOffset(4);
		for (int i = 0; i < curHeaderPtr->attrNumber; i++)
		{
			LPPtr->WriteIntoBuffer(curHeaderPtr->attrTypes + i, 0);
			LPPtr->ShiftOffset(4);
		}
		LPPtr->WriteIntoBuffer(&curHeaderPtr->firstTupleFileBlockID, 0);	// write first tuple address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->firstTupleOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastTupleFileBlockID, 0);	// write last tuple address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastTupleOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosFileBlockID, 0);	// write last position address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr = nullptr;
		delete curHeaderPtr->attrTypes;
	}
	
	File* myFilePtr;        //my file pointer
	myFilePtr = MyDB[fileIDValue];
	if (myFilePtr == nullptr)
	{
		return false;
	}
	LPPtr = &myFilePtr->filePtr;	
	LPPtr->fileBlockID = 0;
	LPPtr->offset = 0;		// locate the LPPtr

	int numAttr, tupleSize;
	tupleSize = 4 * 6;
	numAttr = attrNumValue;
	for (int i = 0; i < numAttr; i++)
	{
		if (attrTypeList[i] > 0)	// char(n)
			tupleSize += attrTypeList[i];
		else if (attrTypeList[i] == 0)	// int
			tupleSize += 4;
		else // float
			tupleSize += 8;
	}
	LPPtr->WriteIntoBuffer(&tupleSize, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&numAttr, 0);
	LPPtr->ShiftOffset(4);
	for (int i = 0; i < numAttr; i++)
	{
		LPPtr->WriteIntoBuffer(attrTypeList + i, 0);
		LPPtr->ShiftOffset(4);
	}
	int firstTupleFileBlockID, firstTupleOffset;
	firstTupleFileBlockID = LPPtr->fileBlockID;
	firstTupleOffset = LPPtr->offset + 24;
	LPPtr->WriteIntoBuffer(&firstTupleFileBlockID, 0);	// write first tuple address
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&firstTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&firstTupleFileBlockID, 0);	// write last tuple address
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&firstTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&firstTupleFileBlockID, 0);	// write last position address (first empty place)
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&firstTupleOffset, 0);
	LPPtr->ShiftOffset(4);

	return true;
}

bool RecordManager::OpenRecordFile(unsigned int fileIDValue)
{
	if (curHeaderPtr != nullptr)
	{
		// write back the RecordFileHeader
		LPPtr->fileBlockID = 0;
		LPPtr->offset = 0;		// locate the LPPtr to the start of RecordHeader
		LPPtr->WriteIntoBuffer(&curHeaderPtr->tupleSize, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->attrNumber, 0);
		LPPtr->ShiftOffset(4);
		for (int i = 0; i < curHeaderPtr->attrNumber; i++)
		{
			LPPtr->WriteIntoBuffer(curHeaderPtr->attrTypes + i, 0);
			LPPtr->ShiftOffset(4);
		}
		LPPtr->WriteIntoBuffer(&curHeaderPtr->firstTupleFileBlockID, 0);	// write first tuple address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->firstTupleOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastTupleFileBlockID, 0);	// write last tuple address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastTupleOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosFileBlockID, 0);	// write last position address
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr = nullptr;
		delete curHeaderPtr->attrTypes;
	}
	else
	{
		curHeaderPtr = new RecordFileHeader;
		if (curHeaderPtr == nullptr)
		{
			cout << "Error: no enough memory for allocating curHeaderPtr";
			exit(1);
		}
	}
	if (ReadRecordHeader(fileIDValue))
	{
		curFileID = fileIDValue;
		return true;
	}
	else
	{
		return false;
	}
}

bool RecordManager::DropRecordFile(unsigned int fileIDValue)
{
	if (!OpenRecordFile(fileIDValue))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool RecordManager::ReadRecordHeader(unsigned int fileIDValue)
{
	curFileID = fileIDValue;
	File* myFilePtr;        //my file pointer
	myFilePtr = MyDB[curFileID];
	if (myFilePtr == nullptr)
	{
		return false;
	}
	LPPtr = &myFilePtr->filePtr;
	LPPtr->fileBlockID = 0;
	LPPtr->offset = 0;

	LPPtr->ReadFromBuffer(&curHeaderPtr->tupleSize, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->attrNumber, 0);
	LPPtr->ShiftOffset(4);
	if (curHeaderPtr->attrTypes == nullptr)
		delete curHeaderPtr->attrTypes;
	curHeaderPtr->attrTypes = new int[curHeaderPtr->attrNumber];
	for (int i = 0; i < curHeaderPtr->attrNumber; i++)
	{
		LPPtr->ReadFromBuffer(curHeaderPtr->attrTypes + i, 0);
		LPPtr->ShiftOffset(4);
	}
	LPPtr->ReadFromBuffer(&curHeaderPtr->firstTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->firstTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->lastTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->lastTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->lastPosFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->lastPosOffset, 0);
	LPPtr->ShiftOffset(4);
	return true;
}

bool RecordManager::SearchTupleByCondition(unsigned int attrIndex, string keyValue, unsigned int operation, 
											vector<unsigned int> &tupleFileBlockID, vector<unsigned int> &tupleOffset)
{
	// If the file is empty
	if (curHeaderPtr->firstTupleFileBlockID == curHeaderPtr->lastPosFileBlockID && curHeaderPtr->firstTupleOffset == curHeaderPtr->lastPosOffset)
	{
		return false;
	}
	RecordTuple* curTuplePtr;
	LPPtr->fileBlockID = curHeaderPtr->firstTupleFileBlockID;
	LPPtr->offset = curHeaderPtr->firstTupleOffset;
	curTuplePtr = ReadRecordTuple(LPPtr);
	while (!(curTuplePtr->selfFileBlockID == curHeaderPtr->lastPosFileBlockID && curTuplePtr->selfOffset == curHeaderPtr->lastPosOffset))
	{
		LPPtr->ShiftOffset(24);
		// Find the value of attrValues[attrIndex]
		for (unsigned int i = 0; i < attrIndex; i++)
		{
			if (curHeaderPtr->attrTypes[i] > 0)	// char(n)
				LPPtr->ShiftOffset(curHeaderPtr->attrTypes[i]);
			else if (curHeaderPtr->attrTypes[i] == 0)	// int 
				LPPtr->ShiftOffset(4);
			else // float
				LPPtr->ShiftOffset(8);
		}
		if (curHeaderPtr->attrTypes[attrIndex] > 0)	// char(n)
		{
			char* searchKeyValue = (char*)keyValue.c_str();
			char* oriValue = new char[curHeaderPtr->attrTypes[attrIndex]];
			LPPtr->ReadFromBuffer(oriValue, curHeaderPtr->attrTypes[attrIndex]);
			if (operation == 0 && strcmp(oriValue, searchKeyValue) == 0 || operation == 1 && strcmp(oriValue, searchKeyValue) > 0 ||
				operation == 2 && strcmp(oriValue, searchKeyValue) < 0 || operation == 3 && strcmp(oriValue, searchKeyValue) >= 0 ||
				operation == 4 && strcmp(oriValue, searchKeyValue) <= 0 || operation == 5 && strcmp(oriValue, searchKeyValue) != 0)
			{
				tupleFileBlockID.push_back(curTuplePtr->selfFileBlockID);
				tupleOffset.push_back(curTuplePtr->selfOffset);
			}
		}
		else if (curHeaderPtr->attrTypes[attrIndex] == 0)	// int 
		{
			int searchKeyValue = atoi(keyValue.c_str());
			int oriValue;
			LPPtr->ReadFromBuffer(&oriValue, 0);
			if (operation == 0 && oriValue == searchKeyValue || operation == 1 && oriValue > searchKeyValue || operation == 2 && oriValue < searchKeyValue ||
				operation == 3 && oriValue >= searchKeyValue || operation == 4 && oriValue <= searchKeyValue || operation == 5 && oriValue != searchKeyValue)
			{
				tupleFileBlockID.push_back(curTuplePtr->selfFileBlockID);
				tupleOffset.push_back(curTuplePtr->selfOffset);
			}
		}
		else	// float
		{
			float searchKeyValue = atof(keyValue.c_str());
			float oriValue;
			LPPtr->ReadFromBuffer(&oriValue, -1);
			if (operation == 0 && oriValue == searchKeyValue || operation == 1 && oriValue > searchKeyValue || operation == 2 && oriValue < searchKeyValue ||
				operation == 3 && oriValue >= searchKeyValue || operation == 4 && oriValue <= searchKeyValue || operation == 5 && oriValue != searchKeyValue)
			{
				tupleFileBlockID.push_back(curTuplePtr->selfFileBlockID);
				tupleOffset.push_back(curTuplePtr->selfOffset);
			}
		}
		if (curTuplePtr->nextTupleFileBlockID == 0 && curTuplePtr->nextTupleOffset == 0)
			break;
		LPPtr->fileBlockID = curTuplePtr->nextTupleFileBlockID;
		LPPtr->offset = curTuplePtr->nextTupleOffset;
		curTuplePtr = ReadRecordTuple(LPPtr);
	}
	if (tupleFileBlockID.size() == 0)
		return false;
	else
		return true;
}

bool RecordManager::InsertTuple(vector<string> attrValues, unsigned int & selfFileBlockID, unsigned int & selfOffset)
{
	// The record file is empty
	if (curHeaderPtr->firstTupleFileBlockID == curHeaderPtr->lastPosFileBlockID && curHeaderPtr->firstTupleOffset == curHeaderPtr->lastPosOffset)
	{
		selfFileBlockID = curHeaderPtr->lastPosFileBlockID;
		selfOffset = curHeaderPtr->lastPosOffset;
		RecordTuple* tuplePtr = new RecordTuple;
		tuplePtr->selfFileBlockID = selfFileBlockID;
		tuplePtr->selfOffset = selfOffset;
		tuplePtr->nextTupleFileBlockID = 0;
		tuplePtr->nextTupleOffset = 0;
		tuplePtr->preTupleFileBlockID = 0;
		tuplePtr->preTupleOffset = 0;

		// write the tuple into buffer
		LPPtr->fileBlockID = selfFileBlockID;
		LPPtr->offset = selfOffset;
		WriteRecordTuple(LPPtr, tuplePtr, attrValues);
		// update the curHeaderPtr
		curHeaderPtr->lastPosOffset += curHeaderPtr->tupleSize;
	}
	else // it is not empty
	{
		if (!FindEmptyPosToWriteTuple())	// find position for writing new tuple
		{
			return false;
		}
		selfFileBlockID = curHeaderPtr->lastPosFileBlockID;
		selfOffset = curHeaderPtr->lastPosOffset;
		RecordTuple* tuplePtr = new RecordTuple;	// initial the new tuple
		tuplePtr->selfFileBlockID = selfFileBlockID;
		tuplePtr->selfOffset = selfOffset;
		tuplePtr->nextTupleFileBlockID = 0;
		tuplePtr->nextTupleOffset = 0;
		tuplePtr->preTupleFileBlockID = curHeaderPtr->lastTupleFileBlockID;
		tuplePtr->preTupleOffset = curHeaderPtr->lastTupleOffset;
		// write the new tuple into buffer
		LPPtr->fileBlockID = selfFileBlockID;
		LPPtr->offset = selfOffset;
		WriteRecordTuple(LPPtr, tuplePtr, attrValues);
		// update the curHeaderPtr
		curHeaderPtr->lastTupleFileBlockID = selfFileBlockID;
		curHeaderPtr->lastTupleOffset = selfOffset;
		curHeaderPtr->lastPosOffset += curHeaderPtr->tupleSize;

		// Get tuplePtr's preTuple
		LPPtr->fileBlockID = tuplePtr->preTupleFileBlockID;
		LPPtr->offset = tuplePtr->preTupleOffset;
		RecordTuple* preTuplePtr = ReadRecordTuple(LPPtr);
		preTuplePtr->nextTupleFileBlockID = tuplePtr->selfFileBlockID;
		preTuplePtr->nextTupleOffset = tuplePtr->selfOffset;
		WriteRecordTuple(LPPtr, preTuplePtr);
	}
	return true;
}

bool RecordManager::DeleteTuple(unsigned int attrIndex, string keyValue, unsigned int operation)
{
	vector<unsigned int> tupleFileBlockID, tupleOffset;
	if (!SearchTupleByCondition(attrIndex, keyValue, operation, tupleFileBlockID, tupleOffset))
	{
		return false;
	}
	else
	{
		RecordTuple* curTuplePtr, *preTuplePtr, *nextTuplePtr;
		for (int i = 0; i < tupleFileBlockID.size(); i++)
		{
			LPPtr->fileBlockID = tupleFileBlockID[i];
			LPPtr->offset = tupleOffset[i];
			curTuplePtr = ReadRecordTuple(LPPtr);
			// If it is the first tuple in the file
			if (curTuplePtr->preTupleFileBlockID == 0 && curTuplePtr->preTupleOffset == 0)
			{
				//If it has only one tuple in the file
				if (curTuplePtr->nextTupleFileBlockID == curHeaderPtr->lastTupleFileBlockID && curTuplePtr->nextTupleOffset == curHeaderPtr->lastTupleOffset)
				{
					curHeaderPtr->lastTupleFileBlockID = curHeaderPtr->firstTupleFileBlockID;
					curHeaderPtr->lastTupleFileBlockID = curHeaderPtr->firstTupleOffset;
					curHeaderPtr->lastPosFileBlockID = curHeaderPtr->firstTupleFileBlockID;
					curHeaderPtr->lastPosOffset = curHeaderPtr->firstTupleOffset;
				}
				else // it has more than one tuple
				{
					LPPtr->fileBlockID = curTuplePtr->nextTupleFileBlockID;
					LPPtr->offset = curTuplePtr->nextTupleOffset;
					nextTuplePtr = ReadRecordTuple(LPPtr);
					// update next tuple
					nextTuplePtr->preTupleFileBlockID = 0;
					nextTuplePtr->preTupleOffset = 0;
					WriteRecordTuple(LPPtr, nextTuplePtr);
					// update the curHeader
					curHeaderPtr->firstTupleFileBlockID = nextTuplePtr->selfFileBlockID;
					curHeaderPtr->firstTupleOffset = nextTuplePtr->selfOffset;
				}
			}
			else if (curTuplePtr->nextTupleFileBlockID == 0 && curTuplePtr->nextTupleOffset == 0)	// If it is the last tuple
			{
				LPPtr->fileBlockID = curTuplePtr->preTupleFileBlockID;
				LPPtr->offset = curTuplePtr->preTupleOffset;
				preTuplePtr = ReadRecordTuple(LPPtr);
				// update pre tuple
				preTuplePtr->nextTupleFileBlockID = 0;
				preTuplePtr->nextTupleOffset = 0;
				WriteRecordTuple(LPPtr, preTuplePtr);
				// update the curHeader
				curHeaderPtr->lastTupleFileBlockID = preTuplePtr->selfFileBlockID;
				curHeaderPtr->lastTupleOffset = preTuplePtr->selfOffset;
				curHeaderPtr->lastPosFileBlockID = curTuplePtr->selfFileBlockID;
				curHeaderPtr->lastPosOffset = curTuplePtr->selfOffset;
			}
			else // it is in the middle
			{
				LPPtr->fileBlockID = curTuplePtr->nextTupleFileBlockID;
				LPPtr->offset = curTuplePtr->nextTupleOffset;
				nextTuplePtr = ReadRecordTuple(LPPtr);
				// update next tuple
				nextTuplePtr->preTupleFileBlockID = curTuplePtr->preTupleFileBlockID;
				nextTuplePtr->preTupleOffset = curTuplePtr->preTupleOffset;
				WriteRecordTuple(LPPtr, nextTuplePtr);

				LPPtr->fileBlockID = curTuplePtr->preTupleFileBlockID;
				LPPtr->offset = curTuplePtr->preTupleOffset;
				preTuplePtr = ReadRecordTuple(LPPtr);
				// update pre tuple
				preTuplePtr->nextTupleFileBlockID = curTuplePtr->nextTupleFileBlockID;
				preTuplePtr->nextTupleOffset = curTuplePtr->nextTupleOffset;
				WriteRecordTuple(LPPtr, preTuplePtr);
				
			}
		}
		return true;
	}

}

bool RecordManager::DeleteTupleByAddress(vector<unsigned int> tupleFileBlockID, vector<unsigned int> tupleOffset)
{
	if (tupleFileBlockID.size() == 0)
	{
		return false;
	}
	RecordTuple* curTuplePtr, *preTuplePtr, *nextTuplePtr;
	for (int i = 0; i < tupleFileBlockID.size(); i++)
	{
		LPPtr->fileBlockID = tupleFileBlockID[i];
		LPPtr->offset = tupleOffset[i];
		curTuplePtr = ReadRecordTuple(LPPtr);
		// If it is the first tuple in the file
		if (curTuplePtr->preTupleFileBlockID == 0 && curTuplePtr->preTupleOffset == 0)
		{
			//If it has only one tuple in the file
			if (curTuplePtr->nextTupleFileBlockID == curHeaderPtr->lastTupleFileBlockID && curTuplePtr->nextTupleOffset == curHeaderPtr->lastTupleOffset)
			{
				curHeaderPtr->lastTupleFileBlockID = curHeaderPtr->firstTupleFileBlockID;
				curHeaderPtr->lastTupleFileBlockID = curHeaderPtr->firstTupleOffset;
				curHeaderPtr->lastPosFileBlockID = curHeaderPtr->firstTupleFileBlockID;
				curHeaderPtr->lastPosOffset = curHeaderPtr->firstTupleOffset;
			}
			else // it has more than one tuple
			{
				LPPtr->fileBlockID = curTuplePtr->nextTupleFileBlockID;
				LPPtr->offset = curTuplePtr->nextTupleOffset;
				nextTuplePtr = ReadRecordTuple(LPPtr);
				// update next tuple
				nextTuplePtr->preTupleFileBlockID = 0;
				nextTuplePtr->preTupleOffset = 0;
				WriteRecordTuple(LPPtr, nextTuplePtr);
				// update the curHeader
				curHeaderPtr->firstTupleFileBlockID = nextTuplePtr->selfFileBlockID;
				curHeaderPtr->firstTupleOffset = nextTuplePtr->selfOffset;
			}
		}
		else if (curTuplePtr->nextTupleFileBlockID == 0 && curTuplePtr->nextTupleOffset == 0)	// If it is the last tuple
		{
			LPPtr->fileBlockID = curTuplePtr->preTupleFileBlockID;
			LPPtr->offset = curTuplePtr->preTupleOffset;
			preTuplePtr = ReadRecordTuple(LPPtr);
			// update pre tuple
			preTuplePtr->nextTupleFileBlockID = 0;
			preTuplePtr->nextTupleOffset = 0;
			WriteRecordTuple(LPPtr, preTuplePtr);
			// update the curHeader
			curHeaderPtr->lastTupleFileBlockID = preTuplePtr->selfFileBlockID;
			curHeaderPtr->lastTupleOffset = preTuplePtr->selfOffset;
			curHeaderPtr->lastPosFileBlockID = curTuplePtr->selfFileBlockID;
			curHeaderPtr->lastPosOffset = curTuplePtr->selfOffset;
		}
		else // it is in the middle
		{
			LPPtr->fileBlockID = curTuplePtr->nextTupleFileBlockID;
			LPPtr->offset = curTuplePtr->nextTupleOffset;
			nextTuplePtr = ReadRecordTuple(LPPtr);
			// update next tuple
			nextTuplePtr->preTupleFileBlockID = curTuplePtr->preTupleFileBlockID;
			nextTuplePtr->preTupleOffset = curTuplePtr->preTupleOffset;
			WriteRecordTuple(LPPtr, nextTuplePtr);

			LPPtr->fileBlockID = curTuplePtr->preTupleFileBlockID;
			LPPtr->offset = curTuplePtr->preTupleOffset;
			preTuplePtr = ReadRecordTuple(LPPtr);
			// update pre tuple
			preTuplePtr->nextTupleFileBlockID = curTuplePtr->nextTupleFileBlockID;
			preTuplePtr->nextTupleOffset = curTuplePtr->nextTupleOffset;
			WriteRecordTuple(LPPtr, preTuplePtr);

		}
	}
	return true;

}

void RecordManager::OutputTuples(vector<unsigned int> tupleFileBlockID, vector<unsigned int> tupleOffset, unsigned int width)
{
	RecordTuple* curTuplePtr;
	vector<string> attrValues;
	for (int i = 0; i < tupleFileBlockID.size(); i++)
	{
		attrValues.clear();
		LPPtr->fileBlockID = tupleFileBlockID[i];
		LPPtr->offset = tupleOffset[i];
		curTuplePtr = ReadRecordTuple(LPPtr, attrValues);
		for (int k = 0; k < curHeaderPtr->attrNumber; k++)
		{
			cout << setw(width) << attrValues[k];
		}
		cout << endl;
	}

}

void RecordManager::OutputTuples(unsigned int tupleFileBlockID, unsigned int tupleOffset, unsigned int width)
{
	RecordTuple* curTuplePtr;
	vector<string> attrValues;

	LPPtr->fileBlockID = tupleFileBlockID;
	LPPtr->offset = tupleOffset;
	curTuplePtr = ReadRecordTuple(LPPtr, attrValues);
	for (int k = 0; k < curHeaderPtr->attrNumber; k++)
	{
		cout << setw(width) << attrValues[k];
	}
	cout << endl;

}


string RecordManager::GetKeyValue(unsigned int tupleFileBlockID, unsigned int tupleOffset, unsigned int attrIndex)
{
	LPPtr->fileBlockID = tupleFileBlockID;
	LPPtr->offset = tupleOffset;
	vector<string> values;
	RecordTuple* tuplePtr = ReadRecordTuple(LPPtr, values);
	return values[attrIndex];
}

void RecordManager::GetAllTuples(vector<unsigned>& keyBlockFileID, vector<unsigned>& keyOffset)
{
	LPPtr->fileBlockID = curHeaderPtr->firstTupleFileBlockID;
	LPPtr->offset = curHeaderPtr->firstTupleOffset;
	RecordTuple* tuplePtr = ReadRecordTuple(LPPtr);		// first tuple
	while(!(tuplePtr->selfFileBlockID == curHeaderPtr->lastTupleFileBlockID && tuplePtr->selfOffset == curHeaderPtr->lastTupleOffset))
	{
		keyBlockFileID.push_back(tuplePtr->selfFileBlockID);
		keyOffset.push_back(tuplePtr->selfOffset);
		LPPtr->fileBlockID = tuplePtr->nextTupleFileBlockID;
		LPPtr->offset = tuplePtr->nextTupleOffset;
		tuplePtr = ReadRecordTuple(LPPtr);
	}
	keyBlockFileID.push_back(tuplePtr->selfFileBlockID);
	keyOffset.push_back(tuplePtr->selfOffset);
}

bool RecordManager::FindEmptyPosToWriteTuple()
{
	if (curHeaderPtr->lastPosOffset + curHeaderPtr->tupleSize >= BLOCK_SIZE)
	{
		curHeaderPtr->lastPosFileBlockID++;
		if (curHeaderPtr->lastPosFileBlockID > MAX_BLOCK_NUM)
		{
			cout << "Error: The size of Record File: " << curFileID << " is out of range!";
			return false;
		}
		curHeaderPtr->lastPosOffset = 0;
	}
	return true;
}

void RecordManager::WriteRecordTuple(LocPtr * LPPtr, RecordTuple * tuplePtr, vector<string> attrValues)
{
	LPPtr->WriteIntoBuffer(&tuplePtr->selfFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->selfOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->nextTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->nextTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->preTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->preTupleOffset, 0);
	LPPtr->ShiftOffset(4);

	for (int i = 0; i < curHeaderPtr->attrNumber; i++)
	{
		char* value = (char *)attrValues[i].c_str();
		if (curHeaderPtr->attrTypes[i] > 0) // char(n)
		{
			LPPtr->WriteIntoBuffer(value, curHeaderPtr->attrTypes[i]);
			LPPtr->ShiftOffset(curHeaderPtr->attrTypes[i]);
		}
		else if (curHeaderPtr->attrTypes[i] == 0) // int
		{
			int temp = atoi(value);
			LPPtr->WriteIntoBuffer(&temp, 0);
			LPPtr->ShiftOffset(4);
		}
		else
		{
			float temp = atof(value);
			LPPtr->WriteIntoBuffer(&temp, -1);
			LPPtr->ShiftOffset(8);
		}
	}

}

void RecordManager::WriteRecordTuple(LocPtr * LPPtr, RecordTuple * tuplePtr)
{
	LPPtr->WriteIntoBuffer(&tuplePtr->selfFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->selfOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->nextTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->nextTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->preTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&tuplePtr->preTupleOffset, 0);
	LPPtr->ShiftOffset(4);

	LPPtr->fileBlockID = tuplePtr->selfFileBlockID;
	LPPtr->offset = tuplePtr->selfOffset;
}

RecordTuple * RecordManager::ReadRecordTuple(LocPtr * LPPtr)
{
	RecordTuple* tuplePtr = new RecordTuple;
	LPPtr->ReadFromBuffer(&tuplePtr->selfFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->selfOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->nextTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->nextTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->preTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->preTupleOffset, 0);
	LPPtr->ShiftOffset(4);

	LPPtr->fileBlockID = tuplePtr->selfFileBlockID;
	LPPtr->offset = tuplePtr->selfOffset;
	return tuplePtr;
}

RecordTuple * RecordManager::ReadRecordTuple(LocPtr * LPPtr, vector<string>& attrValues)
{
	RecordTuple* tuplePtr = new RecordTuple;
	LPPtr->ReadFromBuffer(&tuplePtr->selfFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->selfOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->nextTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->nextTupleOffset, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->preTupleFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&tuplePtr->preTupleOffset, 0);
	LPPtr->ShiftOffset(4);

	for (int i = 0; i < curHeaderPtr->attrNumber; i++)
	{
		if (curHeaderPtr->attrTypes[i] > 0) // char(n)
		{
			char* value = new char[curHeaderPtr->attrTypes[i]];
			LPPtr->ReadFromBuffer(value, curHeaderPtr->attrTypes[i]);
			string temp = value;
			attrValues.push_back(temp);
			LPPtr->ShiftOffset(curHeaderPtr->attrTypes[i]);
		}
		else if (curHeaderPtr->attrTypes[i] == 0) // int
		{
			int value;
			LPPtr->ReadFromBuffer(&value, 0);
			stringstream stream;
			string temp;
			stream << value;
			stream >> temp;
			attrValues.push_back(temp);
			LPPtr->ShiftOffset(4);
		}
		else
		{
			float value;
			LPPtr->ReadFromBuffer(&value, -1);
			stringstream stream;
			string temp;
			stream << value;
			stream >> temp;
			attrValues.push_back(temp);
			LPPtr->ShiftOffset(8);
		}
	}
	return tuplePtr;
}
