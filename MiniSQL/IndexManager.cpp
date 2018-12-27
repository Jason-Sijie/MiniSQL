#include "IndexManager.h"
IndexManager MyIM;
extern DB MyDB;

IndexManager::IndexManager()
{
	curHeaderPtr = nullptr;
	LPPtr = nullptr;
	curFileID = 0;
}

IndexManager::~IndexManager()
{
	delete curHeaderPtr;
}

bool IndexManager::CreateIndexFile(unsigned int fileIDValue, int type, int degree)
{
	if (curHeaderPtr != nullptr)
	{
		// write back the IndexFileHeader
		LPPtr->fileBlockID = 0;
		LPPtr->offset = 0;
		LPPtr->WriteIntoBuffer(&curHeaderPtr->type, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->degree, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->nodeLength, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->rootFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->rootOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->headLeafFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->headLeafOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr = nullptr;
	}
	
	File* myFilePtr;        //my file pointer
    myFilePtr = MyDB[fileIDValue];
	if (myFilePtr == nullptr)
	{
		return false;
	}
	LPPtr = &myFilePtr->filePtr;
	LPPtr->fileBlockID = 0;		// points to the header of index file
	LPPtr->offset = 0;
	LPPtr->WriteIntoBuffer(&type, 0);    //write type of key
    LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&degree, 0);  //write degree of the B+Tree
	LPPtr->ShiftOffset(4);

    int* nodeLength = new int;
    if(type == 0)
	{
		*nodeLength = 4*4 + 1 + 4 * 2 + 4 * (degree + 1) + 4 * 2 * (degree + 2);
	}
	else if (type < 0)
	{
		*nodeLength = 4 * 4 + 1 + 4 * 2 + 8 * (2 * degree + 3);
	}
	else
	{
		*nodeLength = 4 * 4 + 1 + 4 * 2 + type * (degree + 1) + 4 * 2 * (degree + 2);
	}
    LPPtr->WriteIntoBuffer(nodeLength, 0);   //write the length of one tree node
    LPPtr->ShiftOffset(4);

    int rootFileBlockID = LPPtr->fileBlockID;   // root's position
    int rootOffset = LPPtr->offset + 24;
    LPPtr->WriteIntoBuffer(&rootFileBlockID, 0);  //write root position
    LPPtr->ShiftOffset(4);
    LPPtr->WriteIntoBuffer(&rootOffset, 0);  
    LPPtr->ShiftOffset(4);

	LPPtr->WriteIntoBuffer(&rootFileBlockID, 0);  //write head leaf position
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&rootOffset, 0);
	LPPtr->ShiftOffset(4);

	LPPtr->WriteIntoBuffer(&rootFileBlockID, 0);  //write last empty position
	LPPtr->ShiftOffset(4);
	LPPtr->WriteIntoBuffer(&rootOffset, 0);
	LPPtr->ShiftOffset(4);

	delete nodeLength;
	return true;
}

bool IndexManager::OpenIndexFile(unsigned int fileIDValue)
{
	if (curHeaderPtr != nullptr && curFileID == LPPtr->fileID)
	{
		// write back the IndexFileHeader
		LPPtr->fileBlockID = 0;
		LPPtr->offset = 0;
		LPPtr->WriteIntoBuffer(&curHeaderPtr->type, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->degree, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->nodeLength, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->rootFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->rootOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->headLeafFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->headLeafOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosFileBlockID, 0);
		LPPtr->ShiftOffset(4);
		LPPtr->WriteIntoBuffer(&curHeaderPtr->lastPosOffset, 0);
		LPPtr->ShiftOffset(4);
		LPPtr = nullptr;
	}
	else
	{
		curHeaderPtr = new IndexFileHeader;
		if (curHeaderPtr == nullptr)
		{
			cout << "Error: no enough memory for allocating curHeaderPtr";
			exit(1);
		}
	}

	if (ReadIndexHeader(fileIDValue))
	{
		curFileID = fileIDValue;
		return true;
	}
	else
	{
		return false;
	}
	
}

bool IndexManager::DropIndex(unsigned int fileIDValue)
{
	if (!OpenIndexFile(fileIDValue))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IndexManager::ReadIndexHeader(unsigned int fileIDValue)
{
	curFileID = fileIDValue;
	File* myFilePtr;        //my file pointer
    myFilePtr = MyDB[curFileID];
	if (myFilePtr == NULL)
	{
		return false;
	}
	LPPtr = &myFilePtr->filePtr;
	LPPtr->fileBlockID = 0;
	LPPtr->offset = 0;

    // read index file head
    LPPtr->ReadFromBuffer(&curHeaderPtr->type, 0);
    LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->degree, 0);
    LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->nodeLength, 0);
    LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->rootFileBlockID, 0);
    LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->rootOffset, 0);
    LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->headLeafFileBlockID, 0);
	LPPtr->ShiftOffset(4);
	LPPtr->ReadFromBuffer(&curHeaderPtr->headLeafOffset, 0);
	LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->lastPosFileBlockID, 0);
    LPPtr->ShiftOffset(4);
    LPPtr->ReadFromBuffer(&curHeaderPtr->lastPosOffset, 0);
    LPPtr->ShiftOffset(4);
	return true;
}

IndexNode* IndexManager::ReadIndexNode(LocPtr* curLPPtr)
{
	IndexNode* nodePtr = AllocateIndexNode();
	
	curLPPtr->ReadFromBuffer(&(nodePtr->selfFileBlockID), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->ReadFromBuffer(&(nodePtr->selfOffset), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->ReadFromBuffer(&(nodePtr->parentFileBlockID), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->ReadFromBuffer(&(nodePtr->parentOffset), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->ReadFromBuffer(&(nodePtr->isLeaf), 1);
	curLPPtr->ShiftOffset(1);
	curLPPtr->ReadFromBuffer(&(nodePtr->childOnNode), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->ReadFromBuffer(&(nodePtr->keyOnNode), 0);
	curLPPtr->ShiftOffset(4);

	for (int i = 0; i < nodePtr->keyOnNode; i++)
	{
		if (curHeaderPtr->type > 0)	// char(n)
		{
			char* value = new char[curHeaderPtr->type];
			curLPPtr->ReadFromBuffer(value, curHeaderPtr->type);
			string temp = value;
			nodePtr->keys[i] = temp;
			curLPPtr->ShiftOffset(curHeaderPtr->type);
		}
		else if (curHeaderPtr->type == 0)	// int
		{
			int value;
			curLPPtr->ReadFromBuffer(&value, 0);
			stringstream stream;
			string temp;
			stream << value;
			stream >> temp;
			nodePtr->keys[i] = temp;
			curLPPtr->ShiftOffset(4);
		}
		else	// float
		{
			float value;
			curLPPtr->ReadFromBuffer(&value, -1);
			stringstream stream;
			string temp;
			stream << value;
			stream >> temp;
			nodePtr->keys[i] = temp;
			curLPPtr->ShiftOffset(8);
		}
	}
	for (int i = nodePtr->keyOnNode; i < curHeaderPtr->degree + 1; i++)
	{
		if (curHeaderPtr->type > 0) // char(n)
		{
			curLPPtr->ShiftOffset(curHeaderPtr->type);
		}
		else if (curHeaderPtr->type == 0) // int
		{
			curLPPtr->ShiftOffset(4);
		}
		else	// float
		{
			curLPPtr->ShiftOffset(8);
		}
	}
	for (int i = 0; i < nodePtr->childOnNode; i++)
	{
		curLPPtr->ReadFromBuffer(nodePtr->childPosBlock + i, 0);
		curLPPtr->ShiftOffset(4);
	}
	for (int i = nodePtr->childOnNode; i < curHeaderPtr->degree + 2; i++)
	{
		curLPPtr->ShiftOffset(4);
	}
	for (int i = 0; i < nodePtr->childOnNode; i++)
	{
		curLPPtr->ReadFromBuffer(nodePtr->childPosOffset + i, 0);
		curLPPtr->ShiftOffset(4);
	}
	curLPPtr->fileBlockID = nodePtr->selfFileBlockID;
	curLPPtr->offset = nodePtr->selfOffset;
	return nodePtr;
}

void IndexManager::WriteIndexNode(LocPtr* curLPPtr, IndexNode *nodePtr)
{
	curLPPtr->WriteIntoBuffer(&(nodePtr->selfFileBlockID), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->WriteIntoBuffer(&(nodePtr->selfOffset), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->WriteIntoBuffer(&(nodePtr->parentFileBlockID), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->WriteIntoBuffer(&(nodePtr->parentOffset), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->WriteIntoBuffer(&(nodePtr->isLeaf), 1);
	curLPPtr->ShiftOffset(1);
	curLPPtr->WriteIntoBuffer(&(nodePtr->childOnNode), 0);
	curLPPtr->ShiftOffset(4);
	curLPPtr->WriteIntoBuffer(&(nodePtr->keyOnNode), 0);
	curLPPtr->ShiftOffset(4);
	
	for (int i = 0; i < nodePtr->keyOnNode; i++)
	{
		char* value = (char *)nodePtr->keys[i].c_str();
		if (curHeaderPtr->type > 0) // char(n)
		{
			curLPPtr->WriteIntoBuffer(value, curHeaderPtr->type);
			curLPPtr->ShiftOffset(curHeaderPtr->type);
		}
		else if (curHeaderPtr->type == 0) // int
		{
			int temp = atoi(value);
			curLPPtr->WriteIntoBuffer(&temp, 0);
			curLPPtr->ShiftOffset(4);
		}
		else	// float
		{
			float temp = atof(value);
			curLPPtr->WriteIntoBuffer(&temp, -1);
			curLPPtr->ShiftOffset(8);
		}
	}
	for (int i = nodePtr->keyOnNode; i < curHeaderPtr->degree + 1; i++)
	{
		if (curHeaderPtr->type > 0) // char(n)
		{
			curLPPtr->ShiftOffset(curHeaderPtr->type);
		}
		else if (curHeaderPtr->type == 0) // int
		{
			curLPPtr->ShiftOffset(4);
		}
		else	// float
		{
			curLPPtr->ShiftOffset(8);
		}
	}
	for (int i = 0; i < nodePtr->childOnNode; i++)
	{
		curLPPtr->WriteIntoBuffer(nodePtr->childPosBlock + i, 0);
		curLPPtr->ShiftOffset(4);
	}
	for (int i = nodePtr->childOnNode; i < curHeaderPtr->degree + 2; i++)
	{
		curLPPtr->ShiftOffset(4);
	}
	for (int i = 0; i < nodePtr->childOnNode; i++)
	{
		curLPPtr->WriteIntoBuffer(nodePtr->childPosOffset + i, 0);
		curLPPtr->ShiftOffset(4);
	}
}

IndexNode* IndexManager::SplitIndexNode(IndexNode* &nodePtr)
{
	// allocate new tree node
	IndexNode* newNodePtr = AllocateIndexNode();

	// allocate buffer for the new tree node
	if (curHeaderPtr->lastPosOffset + curHeaderPtr->nodeLength >= BLOCK_SIZE)
	{
		curHeaderPtr->lastPosFileBlockID++;
		if (curHeaderPtr->lastPosFileBlockID > MAX_BLOCK_NUM)
		{
			cout << "Error: The size of Index File: " << curFileID << " is out of range!";
			exit(1);
		}
		curHeaderPtr->lastPosOffset = 0;
	}
	LPPtr->fileBlockID = curHeaderPtr->lastPosFileBlockID;
	LPPtr->offset = curHeaderPtr->lastPosOffset;
	
	newNodePtr->selfFileBlockID = LPPtr->fileBlockID;
	newNodePtr->selfOffset = LPPtr->offset;
	newNodePtr->parentFileBlockID = nodePtr->parentFileBlockID;
	newNodePtr->parentOffset = nodePtr->parentOffset;
	newNodePtr->isLeaf = nodePtr->isLeaf;
	// change the last position of the B+ Tree
	curHeaderPtr->lastPosOffset += curHeaderPtr->nodeLength;

	size_t i,j;
	if (nodePtr->isLeaf)		// leaf node
	{
		size_t minNode = (curHeaderPtr->degree + 1) / 2;
		// split the keys
		for (i = minNode, j = 0; i < nodePtr->keyOnNode; i++, j++)
		{
			newNodePtr->keys[j] = nodePtr->keys[i];
		}
		newNodePtr->keyOnNode = j;
		nodePtr->keyOnNode = minNode;
		// split the childPtrs
		for (i = minNode, j = 0; i < nodePtr->childOnNode; i++, j++)
		{
			newNodePtr->childPosBlock[j] = nodePtr->childPosBlock[i];
			newNodePtr->childPosOffset[j] = nodePtr->childPosOffset[i];
		}
		nodePtr->childPosBlock[minNode] = newNodePtr->selfFileBlockID;	// old node's next node is the new node
		nodePtr->childPosOffset[minNode] = newNodePtr->selfOffset;
		newNodePtr->childOnNode = j;
		nodePtr->childOnNode = minNode + 1;
	}
	else	// nonleaf node
	{
		size_t minNode = (curHeaderPtr->degree) / 2;
		// split the keys
		for (i = minNode + 1, j = 0; i < nodePtr->keyOnNode; i++, j++)
		{
			newNodePtr->keys[j] = nodePtr->keys[i];
		}
		newNodePtr->keyOnNode = j;
		nodePtr->keyOnNode = minNode;
		// split the childPtrs
		for (i = minNode + 1, j = 0; i < nodePtr->childOnNode; i++, j++)
		{
			newNodePtr->childPosBlock[j] = nodePtr->childPosBlock[i];
			newNodePtr->childPosOffset[j] = nodePtr->childPosOffset[i];
			LPPtr->fileBlockID = newNodePtr->childPosBlock[j];
			LPPtr->offset = newNodePtr->childPosOffset[j];
			IndexNode* temp = ReadIndexNode(LPPtr);
			temp->parentFileBlockID = newNodePtr->selfFileBlockID;
			temp->parentOffset = newNodePtr->selfOffset;
			WriteIndexNode(LPPtr, temp);
		}
		newNodePtr->childOnNode = j;
		nodePtr->childOnNode = minNode + 1;
	}
	return newNodePtr;
}

bool IndexManager::FindKey(IndexNode* nodePtr, string keyValue, int &index)
{
	if (nodePtr->keyOnNode == 0) // no values in the node
	{
		index = 0;
		return false;
	}
	else
	{
		if (curHeaderPtr->type > 0) // char(n)
		{
			// test if key are beyond the area of the keys array
			if (nodePtr->keys[nodePtr->keyOnNode - 1] < keyValue)
			{
				index = nodePtr->keyOnNode;
				return false;
			}
			else if (nodePtr->keys[0]> keyValue)
			{
				index = 0;
				return false;
			} // end of test
			else
			{
				for (size_t i = 0; i < nodePtr->keyOnNode; i++)
				{
					if (nodePtr->keys[i] == keyValue)
					{
						index = i;
						return true;
					}
					else if (nodePtr->keys[i] < keyValue)
						continue;
					else
					{
						index = i;
						return false;
					}
				}
			}
		}
		else if (curHeaderPtr->type == 0) // int
		{
			int key = atoi(keyValue.c_str());
			int value1 = atoi(nodePtr->keys[nodePtr->keyOnNode-1].c_str());
			int value2 = atoi(nodePtr->keys[0].c_str());
			if (value1 < key)
			{
				index = nodePtr->keyOnNode;
				return false;
			}
			else if (value2> key)
			{
				index = 0;
				return false;
			} // end of test
			else
			{
				for (size_t i = 0; i < nodePtr->keyOnNode; i++)
				{
					int value = atoi(nodePtr->keys[i].c_str());
					int key = atoi(keyValue.c_str());
					if (value == key)
					{
						index = i;
						return true;
					}
					else if (value < key)
						continue;
					else
					{
						index = i;
						return false;
					}
				}
			}
		}
		else // float
		{
			float key = atof(keyValue.c_str());
			float value1 = atof(nodePtr->keys[nodePtr->keyOnNode - 1].c_str());
			float value2 = atof(nodePtr->keys[0].c_str());
			if (value1 < key)
			{
				index = nodePtr->keyOnNode;
				return false;
			}
			else if (value2> key)
			{
				index = 0;
				return false;
			} // end of test
			else
			{
				for (size_t i = 0; i < nodePtr->keyOnNode; i++)
				{
					float value = atof(nodePtr->keys[i].c_str());
					float key = atof(keyValue.c_str());
					if (value == key)
					{
						index = i;
						return true;
					}
					else if (value < key)
						continue;
					else
					{
						index = i;
						return false;
					}
				}
			}
		}
	}
	return false;
}

IndexNode* IndexManager::AllocateIndexNode()
{
	IndexNode* nodePtr = new IndexNode;
	if (nodePtr == NULL)
	{
		cout << "Memory Leak: Can not allocate momery for 'nodePtr'in InsertKey!";
		exit(1);
	}
	for (int i = 0; i < curHeaderPtr->degree + 1; i++)
		nodePtr->keys.push_back("0");
	nodePtr->childPosBlock = new unsigned int[curHeaderPtr->degree + 2];
	nodePtr->childPosOffset = new unsigned int[curHeaderPtr->degree + 2];
	for(int i = 0; i<curHeaderPtr->degree + 2; i++)
	{
		nodePtr->childPosBlock[i] = 0;
		nodePtr->childPosOffset[i] = 0;
	}
	return nodePtr;
}

IndexNode* IndexManager::FindToLeaf(IndexNode* nodePtr, string keyValue, int& index, bool& isFind)
{
	isFind = false;
	if (FindKey(nodePtr, keyValue, index))// find the key in the node
	{
		if (nodePtr->isLeaf)
		{
			isFind = true;
			return nodePtr;
		}
		else // the node is not a leaf, continue search until the leaf level
		{
			LPPtr->fileBlockID = nodePtr->childPosBlock[index + 1];
			LPPtr->offset = nodePtr->childPosOffset[index + 1];
			nodePtr = ReadIndexNode(LPPtr);
			while (!nodePtr->isLeaf)
			{
				LPPtr->fileBlockID = nodePtr->childPosBlock[0];
				LPPtr->offset = nodePtr->childPosOffset[0];
				nodePtr = ReadIndexNode(LPPtr);
			}
			index = 0;
			isFind = true;
			return nodePtr;
		}

	}
	else // can not find the key in the node
	{
		if (nodePtr->isLeaf)
		{
			isFind = false;
			return nodePtr;
		}
		else
		{
			LPPtr->fileBlockID = nodePtr->childPosBlock[index];
			LPPtr->offset = nodePtr->childPosOffset[index];
			nodePtr = ReadIndexNode(LPPtr);
			return FindToLeaf(nodePtr, keyValue, index, isFind);
		}
	}
}

bool IndexManager::FindAddress(string keyValue, unsigned int &keyFileBlockID, unsigned int &keyOffset,
								unsigned int &selfFileBlockID, unsigned int &selfOffset, int &index)
{
	IndexNode* nodePtr = AllocateIndexNode();
	LPPtr->fileBlockID = curHeaderPtr->rootFileBlockID;
	LPPtr->offset = curHeaderPtr->rootOffset;
	nodePtr = ReadIndexNode(LPPtr);		// nodePtr points to the root

	bool isFind;
	nodePtr = FindToLeaf(nodePtr, keyValue, index, isFind);
	if (isFind)
	{
		keyFileBlockID = nodePtr->childPosBlock[index];
		keyOffset = nodePtr->childPosOffset[index];
		selfFileBlockID = nodePtr->selfFileBlockID;
		selfOffset = nodePtr->selfOffset;
		return true;
	}
	else
	{
		return false;
	}
}

bool IndexManager::FindAddressByCondition(string keyValue, int operation, vector<unsigned int>& keyFileBlockID, vector<unsigned int>& keyOffset)
{
	unsigned int targetFileBlockID, targetOffset, selfFileBlockID, selfOffset;
	int index, curIndex;
	if (!FindAddress(keyValue, targetFileBlockID, targetOffset, selfFileBlockID, selfOffset, index))
		return false;

	LPPtr->fileBlockID = curHeaderPtr->headLeafFileBlockID;
	LPPtr->offset = curHeaderPtr->headLeafOffset;
	IndexNode* leafPtr = ReadIndexNode(LPPtr);	// leaf head

	switch (operation)
	{
	case 0:	// =
		keyFileBlockID.push_back(targetFileBlockID);
		keyOffset.push_back(targetOffset);
		break;
	case 1:	// >
		LPPtr->fileBlockID = selfFileBlockID;
		LPPtr->offset = selfOffset;
		index++;
		while(!(LPPtr->fileBlockID == 0 && LPPtr->offset == 0))
		{
			leafPtr = ReadIndexNode(LPPtr);
			while (index != leafPtr->childOnNode)	// not the last child
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[index]);
				keyOffset.push_back(leafPtr->childPosOffset[index]);
				index++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
			index = 0;
		}
		break;
	case 2: // <
		while (!(leafPtr->selfFileBlockID == selfFileBlockID && leafPtr->selfOffset == selfOffset))
		{
			curIndex = 0;
			leafPtr = ReadIndexNode(LPPtr);
			while(curIndex != leafPtr->childOnNode)
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
				keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
				curIndex++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
		}
		curIndex = 0;
		leafPtr = ReadIndexNode(LPPtr);
		while(curIndex < index)
		{
			keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
			keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
			curIndex++;
		}
		break;
	case 3:	// >=
		LPPtr->fileBlockID = selfFileBlockID;
		LPPtr->offset = selfOffset;
		while (!(LPPtr->fileBlockID == 0 && LPPtr->offset == 0))
		{
			leafPtr = ReadIndexNode(LPPtr);
			while (index != leafPtr->childOnNode)	// not the last child
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[index]);
				keyOffset.push_back(leafPtr->childPosOffset[index]);
				index++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
			index = 0;
		}
		break;
	case 4:	// <=
		while (!(leafPtr->selfFileBlockID == selfFileBlockID && leafPtr->selfOffset == selfOffset))
		{
			curIndex = 0;
			leafPtr = ReadIndexNode(LPPtr);
			while (curIndex != leafPtr->childOnNode)
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
				keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
				curIndex++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
		}
		curIndex = 0;
		leafPtr = ReadIndexNode(LPPtr);
		while (curIndex <= index)
		{
			keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
			keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
			curIndex++;
		}
		break;
	case 5:	// !=
		// <
		while (!(leafPtr->selfFileBlockID == selfFileBlockID && leafPtr->selfOffset == selfOffset))
		{
			curIndex = 0;
			leafPtr = ReadIndexNode(LPPtr);
			while (curIndex != leafPtr->childOnNode)
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
				keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
				curIndex++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
		}
		curIndex = 0;
		leafPtr = ReadIndexNode(LPPtr);
		while (curIndex < index)
		{
			keyFileBlockID.push_back(leafPtr->childPosBlock[curIndex]);
			keyOffset.push_back(leafPtr->childPosOffset[curIndex]);
			curIndex++;
		}
		// >
		LPPtr->fileBlockID = selfFileBlockID;
		LPPtr->offset = selfOffset;
		index++;
		while (!(LPPtr->fileBlockID == 0 && LPPtr->offset == 0))
		{
			leafPtr = ReadIndexNode(LPPtr);
			while (index != leafPtr->childOnNode)	// not the last child
			{
				keyFileBlockID.push_back(leafPtr->childPosBlock[index]);
				keyOffset.push_back(leafPtr->childPosOffset[index]);
				index++;
			}
			LPPtr->fileBlockID = leafPtr->childPosBlock[leafPtr->childOnNode];
			LPPtr->offset = leafPtr->childPosOffset[leafPtr->childOnNode];
			index = 0;
		}

		break;
	default:
		break;
	}

	return true;
}

bool IndexManager::InsertKey(string keyValue, int keyBlock, int keyOffset)
{
    if(curHeaderPtr->lastPosFileBlockID == curHeaderPtr->rootFileBlockID && 
		curHeaderPtr->lastPosOffset == curHeaderPtr->rootOffset)    // empty index file
    {
		IndexNode* nodePtr = AllocateIndexNode();
		nodePtr->selfFileBlockID = curHeaderPtr->rootFileBlockID;
		nodePtr->selfOffset = curHeaderPtr->rootOffset;
		nodePtr->parentFileBlockID = 0;
		nodePtr->parentOffset = 0;
		nodePtr->isLeaf = true;
		nodePtr->childOnNode = 1;
		nodePtr->keyOnNode = 1;
		nodePtr->keys[0] = keyValue;
		nodePtr->childPosBlock[0] = keyBlock;
		nodePtr->childPosOffset[0] = keyOffset;

		LPPtr->fileBlockID = curHeaderPtr->rootFileBlockID;
		LPPtr->offset = curHeaderPtr->rootOffset;
		WriteIndexNode(LPPtr, nodePtr);
		curHeaderPtr->lastPosOffset += curHeaderPtr->nodeLength;
		curHeaderPtr->headLeafFileBlockID = curHeaderPtr->rootFileBlockID;
		curHeaderPtr->headLeafOffset = curHeaderPtr->rootOffset;
		return true;
    }
	else
	{
		IndexNode *nodePtr;
		LPPtr->fileBlockID = curHeaderPtr->rootFileBlockID;
		LPPtr->offset = curHeaderPtr->rootOffset;
		int index = 0;
		bool isFind = false;

		nodePtr = ReadIndexNode(LPPtr);	// nodePtr points to root
		nodePtr = FindToLeaf(nodePtr, keyValue, index, isFind);
		if (isFind)
		{
			cout << "Error:in insert key to index: the duplicated key!" << endl;
			return false;
		}
		else
		{
			// insert key into the current tree node
			for (int i = nodePtr->keyOnNode - 1; i >= index; i--)
			{
				nodePtr->keys[i + 1] = nodePtr->keys[i];
			}
			nodePtr->keys[index] = keyValue;

			// insert childPtr into the current tree node
			for (int i = nodePtr->childOnNode - 1; i >= index; i--)
			{
				nodePtr->childPosBlock[i + 1] = nodePtr->childPosBlock[i];
				nodePtr->childPosOffset[i + 1] = nodePtr->childPosOffset[i];
			}
			nodePtr->childPosBlock[index] = keyBlock;
			nodePtr->childPosOffset[index] = keyOffset;

			nodePtr->childOnNode++;		//update the number of keys and child
			nodePtr->keyOnNode++;
			if (nodePtr->keyOnNode == curHeaderPtr->degree + 1)	// if the node is full, then split it
			{
				AdjustAfterInsert(nodePtr);
			}
			
			LPPtr->fileBlockID = nodePtr->selfFileBlockID;	// write back current tree node
			LPPtr->offset = nodePtr->selfOffset;
			WriteIndexNode(LPPtr, nodePtr);		
			return true;
		}
	}

}

bool IndexManager::DeleteKey(string keyValue)
{
	if (curHeaderPtr->rootFileBlockID == curHeaderPtr->lastPosFileBlockID && curHeaderPtr->rootOffset == curHeaderPtr->lastPosOffset)
	{
		cout << "ERROR: In deleteKey, no nodes in the index file: " << curFileID << "!" << endl;
		return false;
	}
	else
	{
		IndexNode* nodePtr;
		LPPtr->fileBlockID = curHeaderPtr->rootFileBlockID;
		LPPtr->offset = curHeaderPtr->rootOffset;
		nodePtr = ReadIndexNode(LPPtr);		// nodePtr points to the root

		int index;
		bool isFind;
		nodePtr = FindToLeaf(nodePtr, keyValue, index, isFind);
		if (isFind == false)
		{
			cout << "ERROR: In deleteKey, no nodes in the index file: " << curFileID << "!" << endl;
			return false;
		}
		else // find the key
		{
			// The deleting node is root
			if (nodePtr->selfFileBlockID == curHeaderPtr->rootFileBlockID && nodePtr->selfOffset == curHeaderPtr->rootOffset)
			{
				for (size_t i = index; i < nodePtr->keyOnNode; i++)
				{
					nodePtr->keys[i] = nodePtr->keys[i + 1];
				}
				nodePtr->keyOnNode--;
				for (size_t i = index; i < nodePtr->childOnNode; i++)
				{
					nodePtr->childPosBlock[i] = nodePtr->childPosBlock[i + 1];
					nodePtr->childPosOffset[i] = nodePtr->childPosOffset[i + 1];
				}
				nodePtr->childOnNode--;

				AdjustAfterDelete(nodePtr);
			}
			else // it is a leaf node
			{
				// the key exists in the nonleaf node
				if (index == 0 && !(nodePtr->selfFileBlockID == curHeaderPtr->headLeafFileBlockID && nodePtr->selfOffset == curHeaderPtr->headLeafOffset))
				{
					IndexNode* parentPtr = AllocateIndexNode();
					LPPtr->fileBlockID = nodePtr->parentFileBlockID;
					LPPtr->offset =  nodePtr->parentOffset;
					parentPtr = ReadIndexNode(LPPtr);	

					int indexInParent = 0;
					bool isFindBranch = FindKey(parentPtr, keyValue, indexInParent);
					while (!isFindBranch)
					{
						// parentPtr still has parent
						if (!(parentPtr->selfFileBlockID == curHeaderPtr->rootFileBlockID && parentPtr->selfOffset == curHeaderPtr->rootOffset))
						{
							LPPtr->fileBlockID = parentPtr->parentFileBlockID;
							LPPtr->offset = parentPtr->parentOffset;
							parentPtr = ReadIndexNode(LPPtr);		// parentPtr points to its parent
							isFindBranch = FindKey(parentPtr, keyValue, indexInParent);
						}
						else
							break;
					}	// end of search the nonleaf node, which contains the keyValue

					// update the corresponding key in the nonleaf node
					parentPtr->keys[indexInParent] = nodePtr->keys[1];

					// write back the parentPtr into the buffer
					LPPtr->fileBlockID = parentPtr->selfFileBlockID;
					LPPtr->offset = parentPtr->selfOffset;
					WriteIndexNode(LPPtr, parentPtr);
					delete parentPtr;
				}

				// delete the key in the nodePtr
				for (size_t i = index; i < nodePtr->keyOnNode; i++)
				{
					nodePtr->keys[i] = nodePtr->keys[i+1];
				}
				nodePtr->keyOnNode--;
				// delete the childPtr in the nodePtr
				for (size_t i = index; i < nodePtr->childOnNode; i++)
				{
					nodePtr->childPosBlock[i] = nodePtr->childPosBlock[i + 1];
					nodePtr->childPosOffset[i] = nodePtr->childPosOffset[i + 1];
				}
				nodePtr->childOnNode--;
				// write back the nodePtr into the buffer
				LPPtr->fileBlockID = nodePtr->selfFileBlockID;
				LPPtr->offset = nodePtr->selfOffset;
				WriteIndexNode(LPPtr, nodePtr);

				AdjustAfterDelete(nodePtr);
			}
			return true;
		}
	}
}

void IndexManager::AdjustAfterInsert(IndexNode* nodePtr)
{
	IndexNode* newNodePtr = SplitIndexNode(nodePtr);
	// if nodePtr points to the root
	if (nodePtr->selfFileBlockID == curHeaderPtr->rootFileBlockID && nodePtr->selfOffset == curHeaderPtr->rootOffset)
	{
		IndexNode* root = AllocateIndexNode();
		// allocate buffer for the new tree node
		if (curHeaderPtr->lastPosOffset + curHeaderPtr->nodeLength >= BLOCK_SIZE)
		{
			curHeaderPtr->lastPosFileBlockID++;
			if (curHeaderPtr->lastPosFileBlockID > MAX_BLOCK_NUM)
			{
				cout << "Error: The size of Index File: " << curFileID << " is out of range!";
				exit(1);
			}
			curHeaderPtr->lastPosOffset = 0;
		}
		root->selfFileBlockID = curHeaderPtr->lastPosFileBlockID;
		root->selfOffset = curHeaderPtr->lastPosOffset;
		curHeaderPtr->lastPosOffset += curHeaderPtr->nodeLength;
		root->parentFileBlockID = 0;
		root->parentOffset = 0;
		root->isLeaf = false;
		root->keyOnNode = 1;
		curHeaderPtr->rootFileBlockID = root->selfFileBlockID;
		curHeaderPtr->rootOffset = root->selfOffset;

		IndexNode* tempPtr = AllocateIndexNode();
		tempPtr = newNodePtr;
		while (!tempPtr->isLeaf)
		{
			LPPtr->fileBlockID = tempPtr->childPosBlock[0];
			LPPtr->offset = tempPtr->childPosOffset[0];
			tempPtr = ReadIndexNode(LPPtr);
		}
		root->keys[0] = tempPtr->keys[0];

		root->childOnNode = 2;
		root->childPosBlock[1] = newNodePtr->selfFileBlockID;
		root->childPosOffset[1] = newNodePtr->selfOffset;
		root->childPosBlock[0] = nodePtr->selfFileBlockID;
		root->childPosOffset[0] = nodePtr->selfOffset;
		
		//change the parent info in nodePtr and newNodePtr
		nodePtr->parentFileBlockID = root->selfFileBlockID;
		nodePtr->parentOffset = root->selfOffset;
		newNodePtr->parentFileBlockID = root->selfFileBlockID;
		newNodePtr->parentOffset = root->selfOffset;

		//update the head leaf position
		if (nodePtr->isLeaf)
		{
			curHeaderPtr->headLeafFileBlockID = nodePtr->selfFileBlockID;
			curHeaderPtr->headLeafOffset = nodePtr->selfOffset;
		}

		//update the index in buffer
		LPPtr->fileBlockID = root->selfFileBlockID;
		LPPtr->offset = root->selfOffset;
		WriteIndexNode(LPPtr, root);
		LPPtr->fileBlockID = nodePtr->selfFileBlockID;
		LPPtr->offset = nodePtr->selfOffset;
		WriteIndexNode(LPPtr, nodePtr);
		LPPtr->fileBlockID = newNodePtr->selfFileBlockID;
		LPPtr->offset = newNodePtr->selfOffset;
		WriteIndexNode(LPPtr, newNodePtr);
	}

	else	// nodePtr is not the root
	{
		IndexNode* parentPtr;	// current node's parent
		LPPtr->fileBlockID = nodePtr->parentFileBlockID;
		LPPtr->offset = nodePtr->parentOffset;
		parentPtr = ReadIndexNode(LPPtr);
		// insert the newNode into parent node
		int index;
		string keyValue;
		IndexNode* tempPtr = AllocateIndexNode();
		tempPtr = newNodePtr;
		while (!tempPtr->isLeaf)
		{
			LPPtr->fileBlockID = tempPtr->childPosBlock[0];
			LPPtr->offset = tempPtr->childPosOffset[0];
			tempPtr = ReadIndexNode(LPPtr);
		}
		keyValue = tempPtr->keys[0];
		FindKey(parentPtr, keyValue, index);
		for (int i = parentPtr->keyOnNode - 1; i >= index; i--)
		{
			parentPtr->keys[i + 1] = parentPtr->keys[i];
		}
		parentPtr->keys[index] = keyValue;
		parentPtr->keyOnNode++;

		// insert childPtr into the current tree node
		for (int i = parentPtr->childOnNode - 1; i > index; i--)
		{
			parentPtr->childPosBlock[i + 1] = parentPtr->childPosBlock[i];
			parentPtr->childPosOffset[i + 1] = parentPtr->childPosOffset[i];
		}
		parentPtr->childPosBlock[index + 1] = newNodePtr->selfFileBlockID;
		parentPtr->childPosOffset[index + 1] = newNodePtr->selfOffset;
		parentPtr->childOnNode++;

		// update the tree node in the buffer
		LPPtr->fileBlockID = parentPtr->selfFileBlockID;	// write parent node back
		LPPtr->offset = parentPtr->selfOffset;
		WriteIndexNode(LPPtr, parentPtr);
		LPPtr->fileBlockID = nodePtr->selfFileBlockID;	// write nodePtr back
		LPPtr->offset = nodePtr->selfOffset;
		WriteIndexNode(LPPtr, nodePtr);		
		LPPtr->fileBlockID = newNodePtr->selfFileBlockID;	// write newNodePtr back
		LPPtr->offset = newNodePtr->selfOffset;
		WriteIndexNode(LPPtr, newNodePtr);

		if (parentPtr->keyOnNode == curHeaderPtr->degree + 1)
		{
			AdjustAfterInsert(parentPtr);
		}
	}

}

void IndexManager::AdjustAfterDelete(IndexNode* nodePtr)
{
	size_t minNode = (curHeaderPtr->degree + 1) / 2;
	LPPtr->fileBlockID = curHeaderPtr->rootFileBlockID;
	LPPtr->offset = curHeaderPtr->rootOffset;
	// it is the root
	if (nodePtr->selfFileBlockID == curHeaderPtr->rootFileBlockID && nodePtr->selfOffset == curHeaderPtr->rootOffset)
	{
		if (nodePtr->isLeaf)	// the root is the only node in the B+ tree
		{
			return;
		}
		else if (nodePtr->keyOnNode >= 1)
		{
			return;
		}
		else	// keyOnNode == 0
		{
			// change the root to its child
			curHeaderPtr->rootFileBlockID = nodePtr->childPosBlock[0];
			curHeaderPtr->rootOffset = nodePtr->childPosOffset[0];
		}
	}
	else	// it is leaf or nonleaf node
	{
		if (nodePtr->isLeaf && nodePtr->keyOnNode >= minNode || !nodePtr->isLeaf && nodePtr->keyOnNode >= curHeaderPtr->degree/2)	// no need to adjust
		{
			return;
		}
		else
		{
			// finding nodePtr's parent and left sibling
			IndexNode* parentPtr;
			LPPtr->fileBlockID = nodePtr->parentFileBlockID;
			LPPtr->offset = nodePtr->parentOffset;
			parentPtr = ReadIndexNode(LPPtr);
			size_t index;
			// index is the position of nodePtr in parentPtr's child
			for (index = 0; index < parentPtr->keyOnNode; index++)
			{
				if (curHeaderPtr->type > 0) // char(n)
				{
					if (parentPtr->keys[index] > nodePtr->keys[0])
						break;
					else
						continue;
				}
				else if (curHeaderPtr->type == 0) // int
				{
					int parentValue = atoi(parentPtr->keys[index].c_str());
					int nodeValue = atoi(nodePtr->keys[0].c_str());
					if (parentValue > nodeValue)
						break;
					else
						continue;
				}
				else	// float
				{
					float parentValue = atof(parentPtr->keys[index].c_str());
					float nodeValue = atof(nodePtr->keys[0].c_str());
					if (parentValue > nodeValue)
						break;
					else
						continue;
				}
			}// end of finding the index

			IndexNode* siblingPtr;	// siblingPtr is always the left sibling of nodePtr
			if (index == 0)	// if nodePtr is the first child
			{
				siblingPtr = nodePtr;	// sibling is always the left child
				index++;	// index is always the position of nodePtr
				LPPtr->fileBlockID = parentPtr->childPosBlock[1];
				LPPtr->offset = parentPtr->childPosOffset[1];
				nodePtr = ReadIndexNode(LPPtr);
			}
			else
			{
				siblingPtr = AllocateIndexNode();
				LPPtr->fileBlockID = parentPtr->childPosBlock[index - 1];
				LPPtr->offset = parentPtr->childPosOffset[index - 1];
				siblingPtr = ReadIndexNode(LPPtr);	// siblingPtr is the nodePtr's left sibling
			}

			if (nodePtr->isLeaf) // it is leaf node
			{
				// they can be merged into one node
				if (nodePtr->keyOnNode + siblingPtr->keyOnNode <= curHeaderPtr->degree)
				{
					for (size_t i = siblingPtr->keyOnNode; i < nodePtr->keyOnNode + siblingPtr->keyOnNode; i++)
					{
						siblingPtr->keys[i] = nodePtr->keys[i - siblingPtr->keyOnNode];
					}
					siblingPtr->keyOnNode += nodePtr->keyOnNode;
					for (size_t i = siblingPtr->childOnNode - 1; i < nodePtr->childOnNode + siblingPtr->childOnNode - 1; i++)
					{
						siblingPtr->childPosBlock[i] = nodePtr->childPosBlock[i - siblingPtr->childOnNode + 1];
						siblingPtr->childPosOffset[i] = nodePtr->childPosOffset[i - siblingPtr->childOnNode + 1];
					}
					siblingPtr->childOnNode += nodePtr->childOnNode - 1;
					// delete the key in parentPtr
					for (size_t i = index - 1; i<parentPtr->keyOnNode; i++)
					{
						parentPtr->keys[i] = parentPtr->keys[i + 1];
					}
					for (size_t i = index; i<parentPtr->childOnNode; i++)
					{
						parentPtr->childPosBlock[i] = parentPtr->childPosBlock[i + 1];
						parentPtr->childPosOffset[i] = parentPtr->childPosOffset[i + 1];
					}
					parentPtr->keyOnNode--;
					parentPtr->childOnNode--;
					// write back the tree nodes into buffer
					LPPtr->fileBlockID = parentPtr->selfFileBlockID;
					LPPtr->offset = parentPtr->selfOffset;
					WriteIndexNode(LPPtr, parentPtr);
					LPPtr->fileBlockID = siblingPtr->selfFileBlockID;
					LPPtr->offset = siblingPtr->selfOffset;
					WriteIndexNode(LPPtr, siblingPtr);

					AdjustAfterDelete(parentPtr);
				}
				// adjust the content in these two siblings
				else 	
				{
					// merge the keys and childPtrs in these two siblings
					vector<string> tempKeys;
					vector<int> tempChildBlock, tempChildOffset;
					for (size_t i = 0; i < siblingPtr->keyOnNode; i++)
					{
						tempKeys.push_back(siblingPtr->keys[i]);
						tempChildBlock.push_back(siblingPtr->childPosBlock[i]);
						tempChildOffset.push_back(siblingPtr->childPosOffset[i]);
					}
					for (size_t i = 0; i < nodePtr->keyOnNode; i++)
						tempKeys.push_back(nodePtr->keys[i]);
					for (size_t i = 0; i < nodePtr->childOnNode; i++)
					{
						tempChildBlock.push_back(nodePtr->childPosBlock[i]);
						tempChildOffset.push_back(nodePtr->childPosOffset[i]);
					}
					// rearrange the keys and childPtrs
					siblingPtr->keyOnNode = minNode;
					siblingPtr->childOnNode = minNode + 1;
					nodePtr->keyOnNode = tempKeys.size() - minNode;
					nodePtr->childOnNode = tempChildBlock.size() - minNode;
					for (size_t i = 0; i < siblingPtr->keyOnNode; i++)
					{
						siblingPtr->keys[i] = tempKeys[i];
						siblingPtr->childPosBlock[i] = tempChildBlock[i];
						siblingPtr->childPosOffset[i] = tempChildOffset[i];
					}
					siblingPtr->childPosBlock[minNode] = nodePtr->selfFileBlockID;
					siblingPtr->childPosOffset[minNode] = nodePtr->selfOffset;
					for (size_t i = 0; i < nodePtr->keyOnNode; i++)
					{
						nodePtr->keys[i] = tempKeys[i + minNode];
					}
					for (size_t i = 0; i < nodePtr->childOnNode; i++)
					{
						nodePtr->childPosBlock[i] = tempChildBlock[i + minNode];
						nodePtr->childPosOffset[i] = tempChildOffset[i + minNode];
					}
					// change the info in parentPtr
					parentPtr->keys[index - 1] = nodePtr->keys[0];
					// write back the tree nodes into buffer
					LPPtr->fileBlockID = parentPtr->selfFileBlockID;	// write parent
					LPPtr->offset = parentPtr->selfOffset;
					WriteIndexNode(LPPtr, parentPtr);
					LPPtr->fileBlockID = siblingPtr->selfFileBlockID;	// write sibling
					LPPtr->offset = siblingPtr->selfOffset;
					WriteIndexNode(LPPtr, siblingPtr);
					LPPtr->fileBlockID = nodePtr->selfFileBlockID;		// write node
					LPPtr->offset = nodePtr->selfOffset;
					WriteIndexNode(LPPtr, nodePtr);
					return;
				}
			}
			else // it is nonleaf node
			{
				// they can be merged into one node
				if (nodePtr->keyOnNode + siblingPtr->keyOnNode + 1 <= curHeaderPtr->degree)
				{
					siblingPtr->keys[siblingPtr->keyOnNode] = nodePtr->keys[0];
					siblingPtr->keyOnNode++;
					for (size_t i = 0; i < nodePtr->keyOnNode; i++)
					{
						siblingPtr->keys[i + siblingPtr->keyOnNode] = nodePtr->keys[i];
					}
					for (size_t i = 0; i < nodePtr->childOnNode; i++)
					{
						siblingPtr->childPosBlock[i + siblingPtr->childOnNode] = nodePtr->childPosBlock[i ];
						siblingPtr->childPosOffset[i + siblingPtr->childOnNode] = nodePtr->childPosOffset[i];
						LPPtr->fileBlockID = siblingPtr->childPosBlock[i + siblingPtr->childOnNode];
						LPPtr->offset = siblingPtr->childPosOffset[i + siblingPtr->childOnNode];
						IndexNode* temp = ReadIndexNode(LPPtr);
						temp->parentFileBlockID = siblingPtr->selfFileBlockID;
						temp->parentOffset = siblingPtr->selfOffset;
						WriteIndexNode(LPPtr, temp);
					}
					// delete the key in parentPtr
					for (size_t i = index - 1; i<parentPtr->keyOnNode; i++)
					{
						parentPtr->keys[i] = parentPtr->keys[i + 1];
					}
					for (size_t i = index; i<parentPtr->childOnNode; i++)
					{
						parentPtr->childPosBlock[i] = parentPtr->childPosBlock[i + 1];
						parentPtr->childPosOffset[i] = parentPtr->childPosOffset[i + 1];
					}
					parentPtr->keyOnNode--;
					parentPtr->childOnNode--;
					// write back the tree nodes into buffer
					LPPtr->fileBlockID = parentPtr->selfFileBlockID;
					LPPtr->offset = parentPtr->selfOffset;
					WriteIndexNode(LPPtr, parentPtr);
					LPPtr->fileBlockID = siblingPtr->selfFileBlockID;
					LPPtr->offset = siblingPtr->selfOffset;
					WriteIndexNode(LPPtr, siblingPtr);

					AdjustAfterDelete(parentPtr);
				}
				// adjust the content in these two siblings
				else 
				{
					size_t min_Node = curHeaderPtr->degree / 2;
					// merge the keys and childPtrs in these two siblings
					vector<string> tempKeys;
					vector<int> tempChildBlock, tempChildOffset;
					size_t i;
					for (i = 0; i < siblingPtr->keyOnNode; i++)
					{
						tempKeys.push_back(siblingPtr->keys[i]);
						tempChildBlock.push_back(siblingPtr->childPosBlock[i]);
						tempChildOffset.push_back(siblingPtr->childPosOffset[i]);
					}
					tempKeys.push_back(nodePtr->keys[0]);
					tempChildBlock.push_back(siblingPtr->childPosBlock[i]);
					tempChildOffset.push_back(siblingPtr->childPosOffset[i]);
					for (i = 0; i < nodePtr->keyOnNode; i++)
					{
						tempKeys.push_back(nodePtr->keys[i]);
						tempChildBlock.push_back(nodePtr->childPosBlock[i]);
						tempChildOffset.push_back(nodePtr->childPosOffset[i]);
					}
					tempChildBlock.push_back(nodePtr->childPosBlock[i]);
					tempChildOffset.push_back(nodePtr->childPosOffset[i]);
					// rearrange the keys and childPtrs
					siblingPtr->keyOnNode = minNode;
					siblingPtr->childOnNode = minNode + 1;
					for (i = 0; i < siblingPtr->keyOnNode; i++)
					{
						siblingPtr->keys[i] = tempKeys[i];
						siblingPtr->childPosBlock[i] = tempChildBlock[i];
						siblingPtr->childPosOffset[i] = tempChildOffset[i];
						LPPtr->fileBlockID = siblingPtr->childPosBlock[i];
						LPPtr->offset = siblingPtr->childPosOffset[i];
						IndexNode* temp = ReadIndexNode(LPPtr);
						temp->parentFileBlockID = siblingPtr->selfFileBlockID;
						temp->parentOffset = siblingPtr->selfOffset;
						WriteIndexNode(LPPtr, temp);
					}
					siblingPtr->childPosBlock[i] = tempChildBlock[i];
					siblingPtr->childPosOffset[i] = tempChildOffset[i];
					LPPtr->fileBlockID = siblingPtr->childPosBlock[i];
					LPPtr->offset = siblingPtr->childPosOffset[i];
					IndexNode* temp = ReadIndexNode(LPPtr);
					temp->parentFileBlockID = siblingPtr->selfFileBlockID;
					temp->parentOffset = siblingPtr->selfOffset;
					WriteIndexNode(LPPtr, temp);

					nodePtr->childOnNode = tempChildBlock.size() - siblingPtr->childOnNode;
					nodePtr->keyOnNode = nodePtr->childOnNode - 1;					
					for (size_t i = 0; i < nodePtr->keyOnNode; i++)
					{
						nodePtr->keys[i] = tempKeys[i + minNode + 1];
					}
					for (size_t i = 0; i < nodePtr->childOnNode; i++)
					{
						nodePtr->childPosBlock[i] = tempChildBlock[i + minNode + 1];
						nodePtr->childPosOffset[i] = tempChildOffset[i + minNode + 1];
						LPPtr->fileBlockID = nodePtr->childPosBlock[i];
						LPPtr->offset = nodePtr->childPosOffset[i];
						IndexNode* temp = ReadIndexNode(LPPtr);
						temp->parentFileBlockID = nodePtr->selfFileBlockID;
						temp->parentOffset = nodePtr->selfOffset;
						WriteIndexNode(LPPtr, temp);
					}
					// change the info in parentPtr
					IndexNode* childPtr = AllocateIndexNode();
					LPPtr->fileBlockID = nodePtr->childPosBlock[0];
					LPPtr->offset = nodePtr->childPosOffset[0];
					childPtr = ReadIndexNode(LPPtr);
					while (!childPtr->isLeaf)
					{
						LPPtr->fileBlockID = childPtr->childPosBlock[0];
						LPPtr->offset = childPtr->childPosOffset[0];
						childPtr = ReadIndexNode(LPPtr);
					}
					
					parentPtr->keys[index - 1] = nodePtr->keys[0];
					// write back the tree nodes into buffer
					LPPtr->fileBlockID = parentPtr->selfFileBlockID;	// write parent
					LPPtr->offset = parentPtr->selfOffset;
					WriteIndexNode(LPPtr, parentPtr);
					LPPtr->fileBlockID = siblingPtr->selfFileBlockID;	// write sibling
					LPPtr->offset = siblingPtr->selfOffset;
					WriteIndexNode(LPPtr, siblingPtr);
					LPPtr->fileBlockID = nodePtr->selfFileBlockID;		// write node
					LPPtr->offset = nodePtr->selfOffset;
					WriteIndexNode(LPPtr, nodePtr);
					return;
				}
			}
		}
	}

}

