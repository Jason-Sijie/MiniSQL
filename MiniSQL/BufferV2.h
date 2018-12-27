#ifndef BUFFER_H
#define BUFFER_H 1

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<map>
using namespace std;
#define MAX_FILENAME_LENGTH 20
#define BLOCK_SIZE 4096
#define MAX_BLOCK_NUM 40960
#define INIT_FILE_BLOCK_NUM 1
#define CURRENT_DATE_TIME "19990513"
#define DEFAULT_FILENAME  "default"
#define DEFAULT_FILE_ID -1
#define DEFAULT_FILEBLOCK_ID -1
#define DEFAULT_BLOCK_ID -1
#define MAX_FILE_ID 100
#define MAX_FILE_BLOCK_ID 100

/*
float: -1
int: 0
char(n): integer representing the length
*/


/*****************************
Class Name : FileHeadInfo
Class Description :
The FileInformation container for
blocks to access file information,
used as an attribute in File class

******************************/
/*typedef struct {
	unsigned long totalBlocks; // number of blocks in current file
	char lastModifiedDate[20]; // the date of the most recent modification, with pattern yyyymmdd ;
	void InitialFileHeadInfo(unsigned long totalBlocksValue,
		char  lastModifiedDateValue[]);// initialization function of FileHeadInfo
}FileHeadInfo; */

/*****************************
Class Name : LocPtr (needs change!!!!!!)
Class Description :
The most used class to access buffer
manager, the API for Index.cpp and
Record.cpp to manipulate each block
write and read operation. Use offset
to denote which byte in the block to
write.

******************************/
class LocPtr {
public:
	LocPtr();
	LocPtr(unsigned int fileIDValue, unsigned int fileBlockIDValue, unsigned int offset);
	//~LocPtr(); 
	friend class DB;
	bool ShiftOffset(unsigned int offsetValue); // shift cur LocPtr and add
	void ShiftBlock(unsigned int fileBlockIDValue);
	bool ReadFromBuffer(void* ptr, int type);	
	bool WriteIntoBuffer(void* ptr, int type);	
	void PureTest();
	unsigned int fileID;	// 
	unsigned int fileBlockID; // denote this block is the X th block of current file
	unsigned int offset; // denote the distance from target addr to head addr of block
private:
	void * MemAddr();
};


/*****************************
Class Name : BlockInfo
Class Description :
The access for a Block, it is designed
as a class because when CLOCK algorithm
visit a block, it doesn't need to what
specific data is in this block, and only
need to know whether it is recently visited
or modified. This BlockInfo is the idea
of hiearachy design.

******************************/
class BlockInfo {
public:
	BlockInfo();
	BlockInfo(unsigned int blockIDValue);
	~BlockInfo();
	friend class MyCLOCK;
	friend class LocPtr;
	friend class File;
	friend class DB;
	//Mutation Funciton
	void SetFileID(unsigned int fileIDValue);
	void SetFileBlockID(unsigned int fileBlockIDValue);
	//void SetPin(bool TorF);
	//Access Function
	unsigned int GetFileID() const;
	unsigned int GetFileBlockID() const;
	unsigned int GetBlockID() const;
	//
	void PrintBlockInfo() const;
	void PrintBlockInfoInARow()const;
	void LoadFromFile(unsigned int fileIDValue,
					  unsigned long fileBlockIDValue); // read data from Disc
	void WriteBackFile() const; // write back to Disc
	//LocPtr* GetPtr2BlockHeadAddr();

private:
	bool isNull;     // initialized or not. 
	bool isLastUsed; // UseBit in CLOCK algorithm, if visited lately
	bool isModified; // ModifyBit in CLOCK algorithm, if modified, then try not to subsititute this one
	BlockInfo * next; // point to next BlockInfo(Block access)

	unsigned long blockID; // the overall blockID in the block pool of MyCLOCK;
	bool isPinned; // is true, can not write this block or write back
	unsigned int fileID; // which file does this block belong to
	unsigned int fileBlockID; // index among all blocks belonging to this file
	//FileHeadInfo * ptr2FileHeader;// all blocks from one file points to the same FileHeadInfo

	// the most important pointer
	void * ptr2BlockHeadAddr; //point to the head address in memory of a block
							  //BlockHeadInfo * ptr2BlockHeader;
};

class FileBlockInfo {
	friend class File;
	friend class LocPtr;
public:
	FileBlockInfo();
	FileBlockInfo(unsigned int blockIDValue, unsigned int fileBlockIDValue);
	~FileBlockInfo();
	void PrintFileBlockInfo() const;
private:
	unsigned int blockID;
	unsigned int fileBlockID;
	bool isInBuffer;
	FileBlockInfo * next;
};
/*****************************
Class Name : MyCLOCK
Class Description :
Contain a block pool to simulate
buffer, and a function CLOCK to
find the best substitute block
if the buffer is full. And this
class give every block an index.

******************************/
class MyCLOCK {
public:
	friend class DB;
	friend class File;
	friend class LocPtr ;// this need to be modified;
	MyCLOCK();
	~MyCLOCK();
	void SetBlockModified(); // Set a block ModifyBit true, used in CLOCK()
	unsigned int GetNullBlock(); // Get the index of a null(not used) block(), prior choice
	unsigned int GetBlockNotThisFile(unsigned int fileIDValue); // Get the index of a block not belonging to current file
	unsigned int CLOCK(int isLastUsed,
		int isModified,
		int setUsedBit); // scan the pool once to find needed block
	unsigned int GetSubstituteBlock(unsigned int fileID);// call the 3 functions above to decide which one is best block
	unsigned int GetExistBlock(unsigned int fileIDValue,
		unsigned long fileBlockIDValue); // Get an index of a loaded block of a given file
	BlockInfo * GetTargetBlockInfo(unsigned int fileIDValue,
		unsigned long fileBlockIDValue);// Get the access of a block by fileID and fileBlockID
	void PrintCLOCKInfo() const;
	//void PrintCLOCKInfo(unsigned int numDisplayRows) const;
	void PrintCLOCKInfo(unsigned int fileIDValue) const;
	void PrintCLOCKInfo(unsigned int numDisplayRows, unsigned int fileIDValue) const;
private:
	unsigned int clockSize; // the bytes this buffer has
	unsigned int curCLOCKBlockIndex; // the index of the current block in CLOCK's scanning procedure
	unsigned int firstNullBlockIndex; // store the index of the head of the null block
	unsigned int lastNullBlockIndex;
	BlockInfo * ptrAllBlocks[MAX_BLOCK_NUM + 1]; // the block pool
};

/*****************************
Class Name : File
Class Description :
The basic data structure in the DB, can
be used to save a table, or an index file
or catalog file. Used by upper layer users
to implement their design.

******************************/
class File {
public:
	friend class DB;
	friend class Block;
	friend class LocPtr;
	File();
	File(const string fileNameValue,
		unsigned int fileIDValue);
	File(const string fileNameValue,
		unsigned int fileIDValue,
		unsigned long totalBlocksValue);
	~File();
	unsigned long GetTotalBlocks() const; // return the totalBlocks this file has
	unsigned int GetBlocksInBuffer() const;
	unsigned int GetFileID() const; // Get File ID
	FileBlockInfo * GetFileBlockInfo(unsigned long fileBlockIDValue) ; // Get Block access by fileBlockID
	FileBlockInfo * GetFirstFileBlockInfo() const; // Get the Block access of the first block in the list
	FileBlockInfo * GetLastFileBlockInfo() const; // Get the Block access of the last block in the list
	FileBlockInfo * AddBlock(); // allocate an extra block to this file
	FileBlockInfo * operator [](unsigned long fileBlockIDValue) ; // return the head LocPtr to a block
	void SetBlockPinned(unsigned long fileBlockIDValue); // set block pinned
	void SetBlockUnpinned(unsigned long fileBlockIDValue);// set block unpinned
	void InputBlock(unsigned int fileBlockIDValue);
	void OutputAllBlock();
	LocPtr filePtr;
private:
	unsigned int fileID; // the ID of current file in the DB
	unsigned long totalBlocks; // the number of blocks the file have
	unsigned int bufferBlocks;
	bool isNew; // if this is a new file
	string fileName; // the name of this file
	//FileHeadInfo * fileHeadInfo; // HeadInfo of this file, for its blocks to visit
	File * next; // pointer to the next File obj
	FileBlockInfo * firstFileBlockInfo; // the head access of block list
	FileBlockInfo * lastFileBlockInfo;
	

};
/*****************************
Class Name : DB
Class Description :
The service provider of a database
buffer system. Define an instance to
use all the services of a buffer
manager. Pay attention to its memory
usage to prevent leakage.

******************************/
class DB {
public:
	DB();
	~DB();
	friend class Block;
	friend class BlockInfo;
	friend class File;
	friend class LocPtr;
	void AddTotalBlocks(unsigned int fileIDValue, int diff); //TODO
	bool IsNewFile(unsigned int fileIDValue); // return if chosen file is new
	void SetNewFile(unsigned int fileIDValue, bool isNewValue); // set one file to be new file
	bool IsExist(unsigned int fileIDValue) const;
	bool IsExist(const string fileNameValue) const;
	unsigned long GetTotalBlocks(unsigned int fileIDValue) const; // return totalBlocks of a file
	unsigned int GetFileID(const string filename) ; // get the fileID of a filename
	MyCLOCK * GetCLOCK() const;
	File * NewFile(const string filename);
	File * operator [](unsigned int fileIDValue); // return a File obj in the DB by fileID
	File * operator[](const string filename) ;// return a File obj in the DB by fileName

	void PrintFileInfo(const string fileName) ;
	void PrintFileInfo(unsigned int fileID) ;
	void PrintFileInfo(File * ptr2File) const;
	void PrintCLOCKInfo() const;
	void PrintDBInfo() const;

	void Start();// the init function to start all DB services
	void End(); // the exit function to handle all space destruction and saving
private:
	unsigned int fileCount; // number of files in the DB
	unsigned int curFileID; // the id of the currently openned file.
	File * ptrFirstFile;
	File * ptrLastFile;
	File * ptrCurFile;
	map<string, unsigned int> fileName2fileID; // map fileName to fileID
	MyCLOCK * blockCLOCK; // the MyCLOCK obj to control subsitution and buffer
};

/**
*  function: write to a certain memory address
*  param TODO
*  param TODO
*  param TODO
*
*  @return LocPtr
*/
//LocPtr MemWrite(const void *, size_t, LocPtr *); // write to an addr in the memory

												 /*
												 * Gloabal Variable : MyDB
												 * Usage : the service provider of the whole buffer manager
												 * Related Function List:
												 *
												 * Notice : Only one DB obj to give all the service,
												 *		   remember to use MyDB.Start() and MyDB.End()
												 *		   in pair to gurantee no leakage.
												 */
extern DB MyDB;



#endif // BUFFER_H
