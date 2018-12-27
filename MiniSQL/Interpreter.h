#ifndef INTERPRETER_H
#define INTERPRETER_H 1

#include <string>
#include <iostream>
#include <vector>
#include <sstream>


#define MAXTABLENUM 100
#define MAXINDEXNUM 100
#define	MAXCOLUMNLENGTH 100

using namespace std;

class Item				//Information for columns when insert
{
public:
	int columeSize;		//insert的时候，输入的元组总共有多少个元素
	vector<string> columns;	//用string存储这些元素
	void InitItem() { columeSize = 0; columns.clear(); }
};
class attribute			//Information for attribute
{
public:

	string attributeName;
	int attributeType;          //数据类型-1 float, 0 represents int, other positive integer represents char and the value is the number of char)
	bool ifUnique;				//初始化为not
	bool ifPrimaryKey;			//初始化为not
	bool ifIndex;				//初始化为not

	void InitAttribute() { ifUnique = ifPrimaryKey = ifIndex = 0; attributeName = ""; attributeType = -2; }
	attribute() {};
};
class Index
{
public:
	string indexName;				//the name of the index
	string indexTableName;			//the name of the table on which the index is create
	string indexAttributeName;		//the name of the attribute on which index is create
	void InitIndex() { indexName = ""; indexTableName = ""; indexAttributeName = ""; }
};

enum OPERATION { EQUAL, NOT_EQUAL, LESS, MORE, LESS_EQUAL, MORE_EQUAL };
class Condition
{

public:
	OPERATION operation;
	string attributeName;			//操作符左值
	string Value;					//操作符右值

	void InitCondition() { attributeName = ""; Value = ""; };
private:
	bool IntOJBK(int content);
	bool FloatOJBK(float content);
	bool StringOJBK(string content);
};

class Table
{
public:

	string tableName;
	int attributeNum;						//the number of attributes in the tables
	int primarykeyLocation;
	vector<int> totalLength;				//total length of one record, should be equal to sum(attributes[i].length)

	vector<attribute> tableAttribute;
	string tableInputValue[32];				//for input value
	void InitTable() {
		tableName = ""; attributeNum = 0; primarykeyLocation = -1;
		totalLength.clear(); tableAttribute.clear(); tableInputValue[32] = { "" };
	}
	unsigned int GetPrimaryKeyPosition();

};
enum COMMAND { CREATETABLE, CREATEINDEX, DROPTABLE, DROPINDEX, SELECT, DELETE, INSERT, QUIT };
class Interpreter
{
public:
	COMMAND command;					//操作名,用COMMAND宏表示
	string commandTableName;			//输入语句中，需要被操作的表格
	string commandIndexName;			//输入语句中，需要被操作的索引名
	string commandAtttributeName;		//需要被操作的属性名

	Table commandTable;					//create时新建的表格信息
	Index commandIndex;					//create时新建的index信息
	Item commandItem;					//存储insert时输入的元组

	attribute curTableAttribute;		//临时的，用于接在table中vector属性后，记录了每个attribute的信息
	attribute curSelectAttribute;		//临时的，用于select语句中，记录attribute的名字
	vector<attribute> commandAttribute;	//只存select时要选择的attribute，createtable时的attri存在table中

	Condition curCondition;				//临时的，用于连在vectorcondition的后面，记录了每个condition的信息
	int conditionNum;					//存语句中condition的数量
	vector<Condition> commandCondition;	//store the information before where



	Interpreter() {};
	string getWord(string s, int *tmp);

	int Parse(string input);


	void initInterpreter()
	{
		commandTableName = "";
		commandIndexName = "";
		commandAtttributeName.clear();
		commandTable.InitTable();
		commandIndex.InitIndex();
		commandItem.InitItem();
		curTableAttribute.InitAttribute();
		curSelectAttribute.InitAttribute();
		commandAttribute.clear();
		curCondition.InitCondition();
		conditionNum = 0;
		commandCondition.clear();
	}
};

extern Interpreter myInter;

#endif