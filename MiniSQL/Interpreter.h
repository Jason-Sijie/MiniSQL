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
	int columeSize;		//insert��ʱ�������Ԫ���ܹ��ж��ٸ�Ԫ��
	vector<string> columns;	//��string�洢��ЩԪ��
	void InitItem() { columeSize = 0; columns.clear(); }
};
class attribute			//Information for attribute
{
public:

	string attributeName;
	int attributeType;          //��������-1 float, 0 represents int, other positive integer represents char and the value is the number of char)
	bool ifUnique;				//��ʼ��Ϊnot
	bool ifPrimaryKey;			//��ʼ��Ϊnot
	bool ifIndex;				//��ʼ��Ϊnot

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
	string attributeName;			//��������ֵ
	string Value;					//��������ֵ

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
	COMMAND command;					//������,��COMMAND���ʾ
	string commandTableName;			//��������У���Ҫ�������ı��
	string commandIndexName;			//��������У���Ҫ��������������
	string commandAtttributeName;		//��Ҫ��������������

	Table commandTable;					//createʱ�½��ı����Ϣ
	Index commandIndex;					//createʱ�½���index��Ϣ
	Item commandItem;					//�洢insertʱ�����Ԫ��

	attribute curTableAttribute;		//��ʱ�ģ����ڽ���table��vector���Ժ󣬼�¼��ÿ��attribute����Ϣ
	attribute curSelectAttribute;		//��ʱ�ģ�����select����У���¼attribute������
	vector<attribute> commandAttribute;	//ֻ��selectʱҪѡ���attribute��createtableʱ��attri����table��

	Condition curCondition;				//��ʱ�ģ���������vectorcondition�ĺ��棬��¼��ÿ��condition����Ϣ
	int conditionNum;					//�������condition������
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