#include"BufferV2.h"
#include <string.h>

DB MyDB;
/**
*  function set: constructor and destructor for LocPtr

*
*  @return
*/
LocPtr::LocPtr() {
	fileID = 0;
	fileBlockID = 0;
	offset = 0;
}

LocPtr::LocPtr(unsigned int fileIDValue, unsigned int fileBlockIDValue, unsigned int offset) {
	fileID = fileIDValue;
	fileBlockID = fileBlockIDValue;
	offset = offset;
}

/**
*  function: give a shift offset to current position, use this to 
*			 shift pointer on a block
*  @param unsigned int offsetValue
*
*  @return void
*/
bool LocPtr::ShiftOffset(unsigned int offsetValue) {
	if(this->offset + offsetValue >= BLOCK_SIZE)
	{
		return false;
	}
	this->offset += offsetValue;
	return true;
}


void LocPtr::ShiftBlock(unsigned int fileBlockIDValue)
{
	this->fileBlockID = fileBlockIDValue;
	this->offset = 0;
}

// need to be done!!!
bool LocPtr::ReadFromBuffer(void * ptr, int type)
{
	FileBlockInfo* fileBlockInfo = MyDB[fileID]->GetFileBlockInfo(fileBlockID);
	if (!fileBlockInfo->isInBuffer) MyDB[fileID]->InputBlock(fileBlockID); //TODO: need to renew fileblockinfo
	void * curPtr = nullptr;
	curPtr = MemAddr();
	if (type == -1) {
		memcpy(ptr, curPtr, 8);
	}
	else if (type == 0) {
		memcpy(ptr, curPtr, 4);
	}
	else {
		memcpy(ptr, curPtr, type);
	}
	return true;
}

/*void LocPtr::PureTest() {
	cout << MyDB.curFileID <<endl;
}*/
bool LocPtr::WriteIntoBuffer(void * ptr, int type)
{
	FileBlockInfo* fileBlockInfo = MyDB[fileID]->GetFileBlockInfo(fileBlockID);
	if (!fileBlockInfo->isInBuffer) MyDB[fileID]->InputBlock(fileBlockID); //TODO: need to renew fileblockinfo
	void * curPtr = nullptr;
	curPtr = MemAddr();
	if (type == -1) {
		memcpy(curPtr, ptr, 8);
	}
	else if (type == 0) {
		memcpy(curPtr, ptr, 4);
	}
	else {
		memcpy(curPtr, ptr, type);
	}
	// do we need to get to the next point or something
	return true;
}


/**
*  function: analyze the actuall addr in Memory using fileBlockID and offset
*
*  @return : void * curAddr
*/
void * LocPtr::MemAddr() {
	//TODO: when accessing MemAddr, should we give a new block to current fileBlock;
	unsigned curBlockID =  MyDB[fileID]->GetFileBlockInfo(fileBlockID)->blockID;
	void * headAddr = MyDB.blockCLOCK->ptrAllBlocks[curBlockID]->ptr2BlockHeadAddr;
	char * curAddr = (char *)headAddr;
	curAddr += offset;
	return (void *)curAddr;
}




/**
*  function: Get data from Windows file by using fileID and fileBlockID
*  @param unsigned int fileIDValue
*  @param unsigned long fileBlockIDValue
*
*  @return void
*/
void BlockInfo::LoadFromFile(unsigned int fileIDValue, unsigned long fileBlockIDValue) {
	fstream loadFile;
	string storeFileName;
	string fileIDString = to_string(fileIDValue);
	string fileBlockIDString = to_string(fileBlockIDValue);
	storeFileName = "Data/" +fileIDString + "_" + fileBlockIDString+".txt"; //TODO:see if this file path is valid
	loadFile.open(storeFileName, ios::in);
	if (!loadFile.is_open()) { //build for the first time
		loadFile.open(storeFileName, ios::out);
		loadFile.close();
		loadFile.open(storeFileName, ios::in);
	}
	char curChar;
	char * charPtr2BlockHeadAddr = (char *)ptr2BlockHeadAddr;
	for (int i = 0; i < BLOCK_SIZE; i++) {
		loadFile.get(curChar);
		*(charPtr2BlockHeadAddr) = curChar;
		charPtr2BlockHeadAddr++;
	}
	loadFile.close();
}

/**
*  function: Write back to Disc for a certain block
*
*  @return void
*/
void BlockInfo::WriteBackFile() const {
	fstream writeFile;
	string storeFileName;
	string fileIDString = to_string(fileID);
	string fileBlockIDString = to_string(fileBlockID);
	storeFileName = "Data/" + fileIDString + "_" + fileBlockIDString+".txt";
	writeFile.open(storeFileName, ios::out);
	char curChar;
	char * charPtr2BlockHeadAddr = (char *)ptr2BlockHeadAddr;
	for (int i = 0; i < BLOCK_SIZE; i++) {
		curChar = *charPtr2BlockHeadAddr;
		writeFile << curChar;
		charPtr2BlockHeadAddr++;
	}

}
void BlockInfo::SetFileID(unsigned int fileIDValue) {
	fileID = fileIDValue;
}
void BlockInfo::SetFileBlockID(unsigned int fileBlockIDValue) {
	fileBlockID = fileBlockIDValue;
}
//void SetPin(bool TorF) {}
unsigned int BlockInfo::GetFileID() const{
	return fileID;
}
unsigned int BlockInfo::GetFileBlockID() const
{
	return this->fileBlockID;
}
unsigned int BlockInfo::GetBlockID() const{
	return blockID;
}

/**
*  function set: Constructor and destructor for BlockInfo
*/
BlockInfo::BlockInfo() {
	blockID = -1;
	fileID = -1;
	fileBlockID = -1;
	isLastUsed = false;
	isModified = false;
	isNull = true;
	next = nullptr;
	ptr2BlockHeadAddr = new char[BLOCK_SIZE];
}
BlockInfo::BlockInfo(unsigned int blockIDValue) {
	blockID = blockIDValue;
	fileID = MAX_FILE_ID;
	fileBlockID = MAX_FILE_BLOCK_ID ;
	isPinned = false;

	isLastUsed = false;
	isModified = false;
	isNull = true;
	next = nullptr;
	ptr2BlockHeadAddr = new char[BLOCK_SIZE];
}

BlockInfo::~BlockInfo() {
	delete [] ptr2BlockHeadAddr;
	ptr2BlockHeadAddr = nullptr;
}



/**
*  function: Print out information of this block
*
*  @return void
*/
void BlockInfo::PrintBlockInfo() const {
	unsigned int curFileID = fileID;
	unsigned long curFileBlockID = fileBlockID;
	unsigned int curBlockID = blockID;
	cout << "fileID : " << curFileID << endl
		<< "fileBlockID : " << curFileBlockID << endl
		<< "blockID : " << curBlockID << endl
		<< "isNull : " << isNull << endl
		<< "isLastUsed : " << isLastUsed << endl
		<< "isModified : " << isModified << endl;
}

void BlockInfo::PrintBlockInfoInARow()const {
	unsigned int curFileID = fileID;
	unsigned long curFileBlockID = fileBlockID;
	unsigned int curBlockID = blockID;

	cout << setw(10) << curBlockID << setw(10) << isNull
		<< setw(10) << isModified << setw(10) << isLastUsed
		<< setw(10) << isLastUsed << setw(10) << curFileID
		<< setw(10) << curFileBlockID << endl;
}


FileBlockInfo::FileBlockInfo() {

}

FileBlockInfo::FileBlockInfo(unsigned int blockIDValue, unsigned int fileBlockIDValue) {
	blockID = blockIDValue;
	fileBlockID = fileBlockIDValue;
	isInBuffer = false;
}
FileBlockInfo::~FileBlockInfo(){

}
void FileBlockInfo::PrintFileBlockInfo() const {

}

/**
*  function set: Constructor and destructor for MyCLOCK

*/
MyCLOCK::MyCLOCK() {
	firstNullBlockIndex = 0;
	lastNullBlockIndex = 0;
	curCLOCKBlockIndex = 0;
	clockSize = MAX_BLOCK_NUM;
	for (unsigned int i = 0; i < MAX_BLOCK_NUM; i++) {
		ptrAllBlocks[i] = new BlockInfo(i);
		if (i != 0) {
			ptrAllBlocks[i - 1]->next = ptrAllBlocks[i];
		}
	}
	lastNullBlockIndex = MAX_BLOCK_NUM - 1;
}

MyCLOCK::~MyCLOCK() {
	for (unsigned int i = 0; i < MAX_BLOCK_NUM; i++) {
		delete ptrAllBlocks[i];
		ptrAllBlocks[i] = nullptr;
	}

}

/**
*  function set: Print the usage of the whole pool

*/
void MyCLOCK::PrintCLOCKInfo() const {
	cout << "clockSize : " << clockSize << endl
		<< "curCLOCKBlockIndex : " << curCLOCKBlockIndex << endl
		<< "firstNullBlockIndex : " << firstNullBlockIndex << endl;
}

/*void MyCLOCK::PrintCLOCKInfo(unsigned int numDisplayRows) const {
	cout << "clockSize : " << clockSize << endl
		<< "curCLOCKBlockIndex : " << curCLOCKBlockIndex << endl
		<< "firstNullBlockIndex : " << firstNullBlockIndex << endl;
	cout << "The Head " << numDisplayRows << " Rows in the pool" << endl;
	cout << "BlockID   isNull   isModified   isLastUsed   fileID   fileBlockID" << endl;
	for (int i = 0; i < numDisplayRows; i++) {
		BlockInfo * cur = ptrAllBlocks[i];
		cur->PrintBlockInfoInARow();
	}
}*/

void MyCLOCK::PrintCLOCKInfo(unsigned int fileIDValue) const {
	cout << "clockSize : " << clockSize << endl
		<< "curCLOCKBlockIndex : " << curCLOCKBlockIndex << endl
		<< "firstNullBlockIndex : " << firstNullBlockIndex << endl;
	cout << "The blocks in FileID : " << fileIDValue << " Rows in the pool" << endl;
	cout << "BlockID   isNull   isModified   isLastUsed   fileID   fileBlockID" << endl;
	for (int i = 0; i < MAX_BLOCK_NUM + 1; i++) {
		BlockInfo * cur = ptrAllBlocks[i];
		if (cur->fileID == fileIDValue) cur->PrintBlockInfoInARow();
	}
}

/*void MyCLOCK::PrintCLOCKInfo(unsigned int numDisplayRows, unsigned int fileIDValue) const {

}*/



/**
*  function: set current block in the pool to be modified

*  @return void
*/
void MyCLOCK::SetBlockModified() {
	ptrAllBlocks[curCLOCKBlockIndex]->isModified = true;
}

/**
*  function: get one unsetted block(just initialized)
*			if not found, return -1 to denote.
*  @return unsigned int firstNullBlockIndex
*/
unsigned int MyCLOCK::GetNullBlock() {
	//we need to pop out one index
	unsigned int cur;
	if (firstNullBlockIndex != -1) {
		cur = firstNullBlockIndex;
		if(ptrAllBlocks[firstNullBlockIndex]->next != nullptr)
			firstNullBlockIndex = ptrAllBlocks[firstNullBlockIndex]->next->blockID;
		else firstNullBlockIndex = -1;
	}
	return cur;
}

/**
*  function: get one block not belonging to the current file
*			if not found return -1
*  @return unsigned int 
*/
unsigned int MyCLOCK::GetBlockNotThisFile(unsigned int targetFileIDValue) {
	for (int i = 0; i < MAX_BLOCK_NUM; i++) {
		if (ptrAllBlocks[i]->fileID != targetFileIDValue) return i;
	}
	return -1; // all block are of this file
}

/**
*  function: CLOCK algorithm, can be controlled with params
			,every function call only scan for one circle, so
			to implement the complete CLOCK algorithm, we need
			to call this function 4 times.
*  @param int isLastUsedValue�� bool type, define restrictions of target block
*  @param int isModifiedValue�� bool type, define restrictions of target block
*  @param int setUsedBit : bool type, if 1, during scanning, set all unmatch block's
							use bit to 0; otherwise just skip them and do nothing
*
*  @return unsigned int curCLOCKBlockIndex : the proper block index
*/

//TODO: skip the pinned page
unsigned int MyCLOCK::CLOCK(int isLastUsedValue , int isModifiedValue, int setUsedBit) {
	unsigned int curIndex = curCLOCKBlockIndex;
	for (curCLOCKBlockIndex = curIndex; curCLOCKBlockIndex < MAX_BLOCK_NUM + 1; curCLOCKBlockIndex++) {
		if (ptrAllBlocks[curCLOCKBlockIndex]->isLastUsed == isLastUsedValue  && ptrAllBlocks[curCLOCKBlockIndex]->isModified == isModifiedValue) {
			return curCLOCKBlockIndex;
		}
		else {
			if (setUsedBit) ptrAllBlocks[curCLOCKBlockIndex]->isLastUsed = 0;
		}
	}
	for (int curCLOCKBlockIndex = 0; curCLOCKBlockIndex < curIndex + 1; curCLOCKBlockIndex++) {
		if (ptrAllBlocks[curCLOCKBlockIndex]->isLastUsed == isLastUsedValue  && ptrAllBlocks[curCLOCKBlockIndex]->isModified == isModifiedValue) {
			return curCLOCKBlockIndex;
		}
		else {
			if (setUsedBit) ptrAllBlocks[curCLOCKBlockIndex]->isLastUsed = 0;
		}
	}
	return -1; //if failed, return false
}

/**
*  function: 1.check if there is a null block
			 2.check if there is a block not belonging to current file
			 3.find out the proper substitute block by CLOCK algorithm
*  @param unsigned int fileID, current fileID
*
*  @return unsigned int : the substitute block index
*/
unsigned int MyCLOCK::GetSubstituteBlock(unsigned int fileID) {
	unsigned int subBlockIndex = 0;
	unsigned int curIndex;
	if ((curIndex = GetNullBlock()) >= 0) subBlockIndex = curIndex;
	else if ((curIndex = GetBlockNotThisFile(fileID)) > 0) subBlockIndex = curIndex;
	else { 
		if (CLOCK(0, 0, 0) > 0) subBlockIndex = curCLOCKBlockIndex; 
		if (CLOCK(0, 1, 1) > 0) subBlockIndex = curCLOCKBlockIndex;
		if (CLOCK(0, 0, 0) > 0) subBlockIndex = curCLOCKBlockIndex;
		if (CLOCK(0, 1, 1) > 0) subBlockIndex = curCLOCKBlockIndex;
	}
	return subBlockIndex;
}

/**
*  function: get the block index in the pool with fileID and fileBlockID
*  @param unsigned int fileIDValue
*  @param unsigned int fileBlockIDValue
*
*  @return unsigned int :the index of given block
*/
/*unsigned int MyCLOCK::GetExistBlock(unsigned int fileIDValue, unsigned long fileBlockIDValue) {
	
}*/

/**
*  function: get the BlockInfo obj with fileID and fileBlockID
*  @param unsigned int fileIDValue
*  @param unsigned int fileBlockIDValue
*
*  @return unsigned int :the BlockInfo of given block
*/
/*BlockInfo * MyCLOCK::GetTargetBlockInfo(unsigned int fileIDValue, unsigned long fileBlockIDValue) {

}*/

/**
*  function set: constructor and destructor for File
*/
File::File() {

}

File::File(const string fileNameValue, unsigned int fileIDValue) {
	fileName = fileNameValue;
	fileID = fileIDValue;
	isNew = true;
	totalBlocks = INIT_FILE_BLOCK_NUM;
	firstFileBlockInfo = new FileBlockInfo(MAX_BLOCK_NUM,0);
	lastFileBlockInfo = firstFileBlockInfo;
	filePtr = LocPtr(fileIDValue, 0, 0);
}

/*File::File(const string fileNameValue, unsigned int fileIDValue, unsigned long totalBlocksValue) {
	*fileName = *fileNameValue;
	fileID = fileIDValue;
	isNew = true;
	totalBlocks = totalBlocksValue;
	firstBlockInfo = new BlockInfo();
	//TODO : need to link all the Blocks together
	fileHeadInfo = new FileHeadInfo();
	fileHeadInfo->InitialFileHeadInfo(totalBlocks, CURRENT_DATE_TIME);
}*/

File::~File() {
	//TODO we must check memory usage here
	FileBlockInfo * cur = firstFileBlockInfo;
	FileBlockInfo * Next = cur;
	while (Next != nullptr) {
		cur = Next;
		Next = Next->next;
		delete cur;
	}
	cur = nullptr;
	next = nullptr;
	firstFileBlockInfo = nullptr;
	lastFileBlockInfo = nullptr;
}

/**
*  function: get the BlockInfo according to fileBlockID of current file obj
*  @param unsigned long fileBlockIDValue
*
*  @return BlockInfo * 
*/
FileBlockInfo* File::GetFileBlockInfo(unsigned long fileBlockIDValue) {
	FileBlockInfo * cur = nullptr;
	if (fileBlockIDValue >= totalBlocks) {
		for (int i = totalBlocks; i <= fileBlockIDValue; i++) {
			cur = AddBlock();
		}
		return cur;
	}
	else {
		cur = firstFileBlockInfo;
		while (cur != nullptr) {
			if (cur->fileBlockID == fileBlockIDValue) return cur;
			cur = cur->next;
		}
	}
}

/**
*  function: get the BlockInfo of the first block of current file
*
*  @return BlockInfo *
*/
FileBlockInfo * File::GetFirstFileBlockInfo() const{
	if (firstFileBlockInfo != nullptr) return firstFileBlockInfo;
	else return nullptr;
}

/**
*  function: get the BlockInfo of the last block of current file
*
*  @return BlockInfo *
*/
FileBlockInfo * File::GetLastFileBlockInfo() const {
	if (lastFileBlockInfo != nullptr) return lastFileBlockInfo;
	else return nullptr;
}

/**
*  function: add an extra new block to tail of the list of blocks
*
*  return : BlockInfo * : the BlockInfo * of new block
*/
FileBlockInfo * File::AddBlock() {
	FileBlockInfo * newFileBlockInfo = new FileBlockInfo(MAX_BLOCK_NUM,totalBlocks);
	lastFileBlockInfo->next = newFileBlockInfo;
	newFileBlockInfo->next = nullptr;
	lastFileBlockInfo = newFileBlockInfo;
	//TODO: check the total numbers here
	totalBlocks++;
	return newFileBlockInfo;
}


/**
*  function: return the writable ptr of the given block head addr
*  @param unsigned long fileBlockIDValue
*
*  @return LocPtr: the head LocPtr of a block
*/
FileBlockInfo * File::operator [](unsigned long fileBlockIDValue)  {
	FileBlockInfo * cur = nullptr;
	if (fileBlockIDValue >= totalBlocks) {
		for (int i = totalBlocks; i <= fileBlockIDValue; i++) {
			cur = AddBlock();
		}
		return cur;
	}
	else {
		cur = firstFileBlockInfo;
		while (cur != nullptr) {
			if (cur->fileBlockID == fileBlockIDValue) return cur;
			cur = cur->next;
		}
	}
}
//still need this for sure

/**
*  function: access function of totalBlocks
*
*  @return unsigned long: total number of blocks of current File obj
*/
unsigned long File::GetTotalBlocks() const {
	return totalBlocks;
}

/**
*  function: Get the number of blocks in the buffer
*
*  return : unsigned int
*/
unsigned int File::GetBlocksInBuffer() const {
	return bufferBlocks;
}

unsigned int File::GetFileID() const
{
	return this->fileID;
}

/**
*  function: set the value of isPinned true of a given block by index
*  @param unsigned long fileBlockIDValue:
*
*  @return void
*/
void File::SetBlockPinned(unsigned long fileBlockIDValue) {
	// also need to check if this Block is in buffer
}

void File::InputBlock(unsigned int fileBlockIDValue) {
	unsigned int subBlockID = MyDB.blockCLOCK->GetSubstituteBlock(fileID);
	FileBlockInfo * cur = GetFileBlockInfo(fileBlockIDValue);
	BlockInfo * curBlockInfo = MyDB.blockCLOCK->ptrAllBlocks[subBlockID];
	//////over write the former one
	if (curBlockInfo->fileID < MAX_FILE_ID) {
		// here we overlap a Block
		curBlockInfo->WriteBackFile();
		MyDB[curBlockInfo->fileID]->bufferBlocks--; // set that file one less
		//set IsInBuffer false
		GetFileBlockInfo(curBlockInfo->fileBlockID)->isInBuffer = false;
		GetFileBlockInfo(curBlockInfo->fileBlockID)->blockID = MAX_BLOCK_NUM;
	}
	else {
		curBlockInfo->isNull = false;
	}
	// write the new block here
	curBlockInfo->LoadFromFile(fileID,fileBlockIDValue);
	// renew the BlockInfo 
	curBlockInfo->SetFileID(fileID);
	curBlockInfo->SetFileBlockID(fileBlockIDValue);
	curBlockInfo->isLastUsed = true;
	//set IsInBuffer true
	GetFileBlockInfo(fileBlockIDValue)->isInBuffer = true;
	GetFileBlockInfo(curBlockInfo->fileBlockID)->blockID = curBlockInfo->blockID;
	bufferBlocks++;
	
}

void File::OutputAllBlock() {
	FileBlockInfo * cur = GetFirstFileBlockInfo();
	while (cur != nullptr) {
		unsigned int absBlockID = cur->blockID;
		if (cur->isInBuffer) {
			MyDB.blockCLOCK->ptrAllBlocks[absBlockID]->WriteBackFile();
			//bufferBlocks--;
		}
		cur = cur->next;
	}
}
/**
*  function: set the value of isPinned false of a given block by index
*  @param unsigned long fileBlockIDValue:
*
*  @return void
*/
void File::SetBlockUnpinned(unsigned long fileBlockIDValue) {
	// also need to check if this Block is in buffer
}

DB::DB() {
	blockCLOCK = new MyCLOCK();
	ptrCurFile = nullptr;
	ptrFirstFile = nullptr;
	ptrLastFile = nullptr;
}
DB::~DB() {
	File * cur = ptrFirstFile;
	File * nextFile = ptrFirstFile;
	delete blockCLOCK;
	blockCLOCK = nullptr;
	while (nextFile != nullptr) {
		cur = nextFile;
		nextFile = nextFile->next;
		delete cur;
	}
	cur = nullptr;
	ptrFirstFile = nullptr;
	ptrLastFile = nullptr;
	ptrCurFile = nullptr;
}
/**
*  function:Create a new file in the DB
*  @param const string filename
*
*  @return File * : pointer to the new File created
*/
File * DB::NewFile(const string filename) {
	File * newFile = new File(filename, fileCount);
	fileCount++;
	if (ptrFirstFile == nullptr) {
		ptrFirstFile = newFile;
		ptrLastFile = newFile;
		fileName2fileID[filename] = newFile->fileID;
		newFile->next = nullptr;
		return newFile;
	}
	else {
		ptrLastFile->next = newFile;
		ptrLastFile = newFile;
		fileName2fileID[filename] = newFile->fileID;
		return newFile;
	}
}

/**
*  function: check if this file is a new file for a fileID
*  @param unsigned int fileIDValue
*
*  @return bool: is or not
*/
bool DB::IsNewFile(unsigned int fileIDValue) {
	File * curFile = ptrFirstFile;
	while (curFile != nullptr) {
		if (curFile->fileID == fileIDValue) {
			return curFile->isNew;
		}
		else curFile = curFile->next;
	}
}

/**
*  function: set a give File obj to a new file
*  @param unsigned int fileIDValue
*  @param bool isNewValue
*
*  @return void
*/
void DB::SetNewFile(unsigned int fileIDValue, bool isNewValue) {
	File * curFile = ptrFirstFile;
	while (curFile != nullptr) {
		if (curFile->fileID == fileIDValue) {
			curFile->isNew = isNewValue;
		}
		else curFile = curFile->next;
	}
}

bool DB::IsExist(unsigned int fileIDValue) const{
	File * cur = ptrFirstFile;
	while (cur != nullptr) {
		if (cur->fileID == fileIDValue) {
			return true;
		}
		else cur = cur->next;
	}
	return false;
}
bool DB::IsExist(const string fileNameValue) const{
	if (fileName2fileID.find(fileNameValue) == fileName2fileID.end()) return false;
	else return true;
}
/**
*  function: Get the total number of Blocks of the MyCLOCK obj, see how 
			many are being used
*  @param param1
*
*  @return
*/
unsigned long DB::GetTotalBlocks(unsigned int fileIDValue) const {
	if (IsExist(fileIDValue)) {
		File * curFile = ptrFirstFile;
		while (curFile != nullptr) {
			if (curFile->fileID == fileIDValue) {
				return curFile->totalBlocks;
			}
			else curFile = curFile->next;
		}
	}
	else return -1;

}

/**
*  function: Get the fileID of a file in the DB
*  @param const string filename
*
*  @return unsigned int fileID:
	if not found, return -1 instead;
*/
unsigned int DB::GetFileID(const string filename) {
	if(IsExist(filename))return fileName2fileID[filename];
	else return -1;
}


/**
*  function: GetCLOCK obj
*
*  @return MyCLOCK blockCLOCK
*/

MyCLOCK * DB::GetCLOCK() const {
	return blockCLOCK;
}

/**
*  function: operator for access through filename
*  @param param1
*
*  @return File * : the File ptr to the destination
				if not found, output errorcode.
*/
File * DB::operator[](const string filename)  {
	// need to build a new File obj
	if (!IsExist(filename)) { // need to create a new
		File * newFile =  NewFile(filename);
		curFileID = newFile->fileID;
		ptrCurFile = newFile;
		return newFile;
	}
	File * curFile = ptrFirstFile;
	while (curFile != nullptr) {
		if (curFile->fileName == filename) {
			//change the status of the currentFile
			curFileID = curFile->fileID;
			ptrCurFile = curFile;
			return curFile;
		}
		else curFile = curFile->next;
	}
	return curFile;
}

/**
*  function: Get the File obj by fileID of current DB
*  @param unsigned int fileIDValue
*
*  @return
*/

File * DB::operator [](unsigned int fileIDValue)  {
	// we have to set the ptrCurFile
	if (!IsExist(fileIDValue)) {
		//stderr : no such fileID
		//TODO : whether to create a new file or 
	}
	File * curFile = ptrFirstFile;
	while (curFile != nullptr) {
		if (curFile->fileID == fileIDValue) {
			// change current pointer
			ptrCurFile = curFile;
			curFileID = fileIDValue;
			return curFile;
		}
		else curFile = curFile->next;
	}
	//change curFilePtr and curFileIndex;
	ptrCurFile = curFile;
	return curFile;
}
/**
*  function set: print out the information 
*/

void DB::PrintFileInfo(const string fileName)  {
	if (!IsExist(fileName)) return;
	File * cur = (*this)[fileName];
	PrintFileInfo(cur);
}

void DB::PrintFileInfo(unsigned int fileID)  {
	if (!IsExist(fileID)) return;
	File * cur = (*this)[fileID];
	PrintFileInfo(cur);
}

void DB::PrintFileInfo(File * ptr2File) const {
	cout << "fileID: "<< ptr2File->fileID<<endl <<"fileName : " << ptr2File->fileName <<endl;
	cout << "Total Blocks : " << ptr2File->GetTotalBlocks() << endl ;
	cout << "Blocks in Buffer : " << ptr2File->GetBlocksInBuffer() << endl;
	cout << "isNew : " << ptr2File->isNew << endl;
	cout << endl;
}

void DB::PrintCLOCKInfo() const {
	blockCLOCK->PrintCLOCKInfo();
}

void DB::PrintDBInfo() const {
	PrintCLOCKInfo();
	PrintFileInfo(ptrCurFile);
}

/**
*  function: constructor for LocPtr
*  @param param1
*  @param param2
*  @param param3
*
*  @return
*/

void DB::Start() {
	blockCLOCK = new MyCLOCK();
	ptrCurFile = nullptr;
	ptrFirstFile = nullptr;
	ptrLastFile = nullptr;
}

/**
*  function: constructor for LocPtr
*  @param param1
*  @param param2
*  @param param3
*
*  @return
*/
void DB::End() {
	File * cur = ptrFirstFile;
	File * nextFile = ptrFirstFile;
	delete blockCLOCK;
	blockCLOCK = nullptr;
	while (nextFile != nullptr) {
		cur = nextFile;
		nextFile = nextFile->next;
		delete cur;
	}
	cur = nullptr;
}

/**
*  function: core function to manipulate MemoryWrite 
*  @param const void * : the src addr
*  @param size_t : the length of data to write
*  @param LocPtr : the dest addr(LocPtr)
*
*  @return LocPtr : the next and closet LocPtr in the same block
*/
