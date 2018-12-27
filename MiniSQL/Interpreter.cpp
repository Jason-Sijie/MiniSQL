#include "Interpreter.h"

Interpreter myInter;

using namespace std;
/********************
Input: the SQL command from user
OUTPUT: ERROR or information for API, information stored in class Interpreter
具体的read操作在main中完成，读到的一行作为string给到interpreter
********************/


bool Condition::FloatOJBK(float content)
{
	std::stringstream a;
	a << Value;
	float thisContent;
	a >> thisContent;
	switch (operation)
	{
	case EQUAL:
		return thisContent == content;
		break;
	case LESS:
		return thisContent < content;
		break;
	case MORE:
		return thisContent > content;
		break;
	case LESS_EQUAL:
		return thisContent <= content;
		break;
	case NOT_EQUAL:
		return thisContent != content;
		break;
	case MORE_EQUAL:
		return thisContent >= content;
		break;
	}

}
bool Condition::IntOJBK(int content)
{
	std::stringstream a;
	a << Value;
	int thisContent;
	a >> thisContent;
	switch (operation)
	{
	case EQUAL:
		return thisContent == content;
		break;
	case LESS:
		return thisContent < content;
		break;
	case MORE:
		return thisContent > content;
		break;
	case LESS_EQUAL:
		return thisContent <= content;
		break;
	case NOT_EQUAL:
		return thisContent != content;
		break;
	case MORE_EQUAL:
		return thisContent >= content;
		break;
	}
}
bool Condition::StringOJBK(string content)
{
	string thisContent = Value;
	switch (operation)
	{
	case EQUAL:
		return thisContent == content;
		break;
	case LESS:
		return thisContent < content;
		break;
	case MORE:
		return thisContent > content;
		break;
	case LESS_EQUAL:
		return thisContent <= content;
		break;
	case NOT_EQUAL:
		return thisContent != content;
		break;
	case MORE_EQUAL:
		return thisContent >= content;
		break;
	}
}



int Interpreter::Parse(string input)
{
	COMMAND result;
	int tmp = 0;
	string word;
	word = getWord(input, &tmp);
	if (strcmp(word.c_str(), "create") == 0)
	{
		word = getWord(input, &tmp);
		
		if (strcmp(word.c_str(), "table") == 0)
		{
			result = CREATETABLE;
		}
		else if (strcmp(word.c_str(), "index") == 0)
			result = CREATEINDEX;
		else
		{
			cout << "Syntax Error for " << word << endl;
		}
	}
	else if (strcmp(word.c_str(), "drop") == 0)
	{
		word = getWord(input, &tmp);
		if (strcmp(word.c_str(), "table") == 0)
			result = DROPTABLE;
		else if (strcmp(word.c_str(), "index") == 0)
			result = DROPINDEX;

		else
		{
			cout << "Syntax Error for " << word << endl;
			return 0;
		}
	}
	else if (strcmp(word.c_str(), "select") == 0)
		result = SELECT;
	else if (strcmp(word.c_str(), "delete") == 0)
		result = DELETE;
	else if (strcmp(word.c_str(), "insert") == 0)
		result = INSERT;
	else if (strcmp(word.c_str(), "quit") == 0)
		result = QUIT;
	else
	{
		cout << "Syntax Error for " << word << endl;
		return 0;
	}
	this->command = result;

	if (result == CREATETABLE)//create table 具体分析
	{
		//Read the name of table and store the information in interpreter
		word = getWord(input, &tmp);
		commandTableName = word;
		if (!word.empty())
			commandTable.tableName = word;
		else
		{
			cout << "Syntax Error for no table name" << endl;
			return 0;
		}
		//Read the attributes of table
		word = getWord(input, &tmp);
		if (word.empty() || strcmp(word.c_str(), "(") != 0)
		{
			cout << "Error in syntax!" << endl;
			return 0;
		}
		else
		{
			word = getWord(input, &tmp);
			
			int attriNum = 0;
			while (!word.empty() && strcmp(word.c_str(), "primary") != 0 && strcmp(word.c_str(), ")") != 0)
			{
				attriNum++;
				commandTable.attributeNum = attriNum;
				curTableAttribute.attributeName = word;
				word = getWord(input, &tmp);
				if (strcmp(word.c_str(), "int") == 0)
					curTableAttribute.attributeType = 0;
				else if (strcmp(word.c_str(), "float") == 0)
					curTableAttribute.attributeType = -1;
				else if (strcmp(word.c_str(), "char") == 0) {
					//string word1, word2;
					word = getWord(input, &tmp);
					if (strcmp(word.c_str(), "(") != 0) {
						cout << "Syntax Error: unknown data type" << endl;
						return 0;
					}
					word = getWord(input, &tmp);
					istringstream convert(word);
					convert >> curTableAttribute.attributeType;
					//curTableAttribute.attributeType *= -1;
					word = getWord(input, &tmp);
					if (strcmp(word.c_str(), ")"))
					{
						cout << "Syntax Error: unknown data type" << endl;
						return 0;
					}

				}
				word = getWord(input, &tmp);
				if (strcmp(word.c_str(), "unique") == 0)
				{
					curTableAttribute.ifUnique = true;			//ifUnique初始化为not true
					word = getWord(input, &tmp);
				}
				commandTable.tableAttribute.push_back(curTableAttribute);
				if (strcmp(word.c_str(), ",") != 0)
				{
					if (strcmp(word.c_str(), ")") != 0) {
						cout << "Syntax Error for ,!" << endl;
						return 0;
					}
					else
						break;
				}
				word = getWord(input, &tmp);
			}
			//Read the only primarykey of the table, record the location of it in commandTable.primarykeyLocation.
			if (strcmp(word.c_str(), "primary") == 0)
			{
				word = getWord(input, &tmp);
				if (strcmp(word.c_str(), "key") != 0)
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
				else
				{
					word = getWord(input, &tmp);
					if (strcmp(word.c_str(), "(") == 0)
					{
						word = getWord(input, &tmp);
						string primaryKey = word;
						int i;
						for (i = 0; i < commandTable.tableAttribute.size(); i++)
						{
							if (primaryKey == commandTable.tableAttribute[i].attributeName)
							{
								commandTable.tableAttribute[i].ifUnique = true;
								commandTable.tableAttribute[i].ifPrimaryKey = true;
								break;
							}
							cout << i << endl;
						}
						if (i >= commandTable.tableAttribute.size())
						{
							cout << "Syntax Error: primaryKey does not exist in attributes " << endl;
							return 0;
						}
						commandTable.primarykeyLocation = i;
						word = getWord(input, &tmp);
						if (strcmp(word.c_str(), ")") != 0)
						{
							cout << "Error in syntax!" << endl;
							return 0;
						}
					}
					else
					{
						cout << "Error in syntax!" << endl;
						return 0;
					}
					word = getWord(input, &tmp);
					if (strcmp(word.c_str(), ")") != 0)
					{
						cout << "Error in syntax!" << endl;
						return 0;
					}
				}
			}
			else if (word.empty())
			{
				cout << "Syntax Error: ')' absent!" << endl;
				return 0;
			}
		}
	}
	else if (result == CREATEINDEX)// create index 具体分析
	{
		word = getWord(input, &tmp);
		if (!word.empty())			//create index indexname
			commandIndexName = word;
		else {
			cout << "Error in syntax!" << endl;
			return 0;
		}
		commandIndex.indexName = word;
		word = getWord(input, &tmp);
		if (strcmp(word.c_str(), "on") != 0)
		{
			cout << "Error in index!" << endl;
			return 0;
		}
		else {
			word = getWord(input, &tmp);

			if (word.empty())
			{
				cout << "Error in index!" << endl;
				return 0;
			}
			else {
				commandIndex.indexTableName = word;
				commandTableName = word;
			}
			word = getWord(input, &tmp);
			if (strcmp(word.c_str(), "(") != 0)
			{
				cout << "Syntax Error!" << endl;
				return 0;
			}
			else {
				word = getWord(input, &tmp);
				if (strcmp(word.c_str(), ")") == 0 || word.empty())
				{
					cout << "Error in index for no attribute name" << endl;
					return 0;
				}
				commandIndex.indexAttributeName = word;
				commandAtttributeName = word;
				word = getWord(input, &tmp);
			}
			if (strcmp(word.c_str(), ")") != 0)
			{
				cout << "Error in index" << endl;
				return 0;
			}
		}
	}
	else if (result == DROPTABLE)//Drop table 具体操作
	{
		word = getWord(input, &tmp);
		if (!word.empty())
		{
			auto it = word.end();
			if (*(--it) == ';')
				word.erase(it);
			commandTableName = word;
		}
		else
		{
			cout << "Syntax Error for no table name" << endl;
			return 0;
		}
	}
	else if (result == DROPINDEX)// Drop index 具体分析
	{
		word = getWord(input, &tmp);
		if (!word.empty())
		{
			auto it = word.end();
			if (*(--it) == ';')
				word.erase(it);
			commandIndexName = word;
		}
		else
		{
			cout << "Syntax Error for no table name" << endl;
			return 0;
		}
	}
	else if (result == SELECT)// Select 具体分析
	{
		word = getWord(input, &tmp);
		if (strcmp(word.c_str(), "*") != 0)// 如果不是select*形式
		{
			while (strcmp(word.c_str(), "from") != 0)
			{
				curSelectAttribute.attributeName = word;
				commandAttribute.push_back(curSelectAttribute);
				word = getWord(input, &tmp);
				if (strcmp(word.c_str(), ",") != 0 && strcmp(word.c_str(), "from") != 0)
				{
					cout << "Syntax Error" << endl;
					return 0;
				}
				if (strcmp(word.c_str(), "from") != 0)
					word = getWord(input, &tmp);
			}
		}
		else
		{
			word = getWord(input, &tmp);
			if (strcmp(word.c_str(), "from") != 0)
			{
				cout << "Error in syntax for no from after *!" << endl;
				return 0;
			}
		}
		//Read the tablename selected from
		word = getWord(input, &tmp);
		if (!word.empty())
			commandTableName = word;
		else
		{
			cout << "Error in syntax for no table name!" << endl;
			return 0;
		}
		//Read the condition
		word = getWord(input, &tmp);
		if (word.empty())	//如果没有condition
			conditionNum = 0;
		else if (strcmp(word.c_str(), "where") == 0)
		{
			do
			{
				conditionNum++;
				word = getWord(input, &tmp);		//准备接收左值
				curCondition.attributeName = word;
				if (word.empty())
				{
					cout << "Error in syntax for condition!" << endl;
					return 0;
				}
				word = getWord(input, &tmp);		//准备接收右值
													//cout <<"operation是"<< word << endl;
				if (strcmp(word.c_str(), "<=") == 0)
					curCondition.operation = LESS_EQUAL;
				else if (strcmp(word.c_str(), ">=") == 0)
					curCondition.operation = MORE_EQUAL;
				else if (strcmp(word.c_str(), "<") == 0)
					curCondition.operation = LESS;
				else if (strcmp(word.c_str(), ">") == 0)
					curCondition.operation = MORE;
				else if (strcmp(word.c_str(), "=") == 0)
					curCondition.operation = EQUAL;
				else if (strcmp(word.c_str(), "<>") == 0)
					curCondition.operation = NOT_EQUAL;
				else
				{
					cout << "Error in syntax for wrong operation!" << endl;
					return 0;
				}
				word = getWord(input, &tmp);		//准备接收右值
				if (word.empty() || strcmp(word.c_str(), "and") == 0)
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
				else
				{
					auto it = word.end();
					if (*(--it) == ';')
						word.erase(it);
					curCondition.Value = word;
				}
				word = getWord(input, &tmp);		//准备接收and
				commandCondition.push_back(curCondition);
			} while (strcmp(word.c_str(), "and") == 0);
		}
	}
	else if (result == DELETE)
	{
		word = getWord(input, &tmp);		//准备记录delete
		if (strcmp(word.c_str(), "from") == 0)
		{
			word = getWord(input, &tmp);
			if (word.empty())
			{
				cout << "Error in syntax!" << endl;
				return 0;
			}
			else
				commandTableName = word;
		}
		else
		{
			cout << "Error in syntax!" << endl;
			return 0;
		}
		word = getWord(input, &tmp);		//准备记录condition
		if (word.empty())					//如果没有condition
			conditionNum = 0;
		else if (strcmp(word.c_str(), "where") == 0)
		{
			do
			{
				conditionNum++;
				word = getWord(input, &tmp);		//准备接收左值
				curCondition.attributeName = word;
				if (word.empty())
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
				word = getWord(input, &tmp);		//准备接收operation
				if (strcmp(word.c_str(), "<=") == 0)
					curCondition.operation = LESS_EQUAL;
				else if (strcmp(word.c_str(), ">=") == 0)
					curCondition.operation = MORE_EQUAL;
				else if (strcmp(word.c_str(), "<") == 0)
					curCondition.operation = LESS;
				else if (strcmp(word.c_str(), ">") == 0)
					curCondition.operation = MORE;
				else if (strcmp(word.c_str(), "=") == 0)
					curCondition.operation = EQUAL;
				else if (strcmp(word.c_str(), "<>") == 0)
					curCondition.operation = NOT_EQUAL;
				else
				{
					cout << "Error in syntax for wrong operation!" << endl;
					return 0;
				}
				word = getWord(input, &tmp);		//准备接收右值
				if (word.empty() || strcmp(word.c_str(), "and") == 0)
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
				else
				{
					auto it = word.end();
					if (*(--it) == ';')
						word.erase(it);
					curCondition.Value = word;
				}
				word = getWord(input, &tmp);		//准备接收and
				commandCondition.push_back(curCondition);
			} while (strcmp(word.c_str(), "and") == 0);
		}
	}
	else if (result == INSERT)
	{
		word = getWord(input, &tmp);		//准备接收into
		if (strcmp(word.c_str(), "into") == 0)
		{
			word = getWord(input, &tmp);	//准备接收表名
			if (word.empty())
			{
				cout << "Error in syntax for no table!" << endl;
				return 0;
			}
			else
				commandTableName = word;
		}
		else
		{
			cout << "Error in syntax for 'into'!" << endl;
			return 0;
		}
		word = getWord(input, &tmp);			//准备接收values
		if (strcmp(word.c_str(), "values") == 0)
		{
			word = getWord(input, &tmp);
			if (strcmp(word.c_str(), "(") == 0) {
				word = getWord(input, &tmp);	//准备接收columns
				while (strcmp(word.c_str(), ")") != 0)
				{
					commandItem.columeSize++;			//别忘了初始化
					commandItem.columns.push_back(word);
					word = getWord(input, &tmp);		//准备接收“，”
					if (strcmp(word.c_str(), ",") != 0 && strcmp(word.c_str(), ")") != 0)
					{
						cout << "Error in syntax ','!" << endl;
						return 0;
					}
					if (strcmp(word.c_str(), ")") != 0)
						word = getWord(input, &tmp);
				}
			}
			else
			{
				cout << "Error in syntax for '('!" << endl;
				return 0;
			}
		}
		else
		{
			cout << "Error in syntax for 'values'!" << endl;
			return 0;
		}
	}
	else if (result == QUIT)
	{
		cout << "Are you sure to quit miniSQL?" << endl;
		return 1;
	}
	return 1;
}


string Interpreter::getWord(string s, int *tmp)
{
	string word;
	int idx1, idx2;

	while ((s[*tmp] == ' ' || s[*tmp] == 10 || s[*tmp] == '\t') && s[*tmp] != 0)
	{
		(*tmp)++;
	}
	idx1 = *tmp;
	if (s[*tmp] == '(' || s[*tmp] == ',' || s[*tmp] == ')')
	{
		(*tmp)++;
		idx2 = *tmp;
		word = s.substr(idx1, idx2 - idx1);
		return word;
	}
	else if (s[*tmp] == 39)
	{
		(*tmp)++;
		while (s[*tmp] != 39 && s[*tmp] != 0)
			(*tmp)++;
		if (s[*tmp] == 39)
		{
			idx1++;
			idx2 = *tmp;
			(*tmp)++;
			word = s.substr(idx1, idx2 - idx1);
			return word;
		}
		else
		{
			word = "";
			return word;
		}
	}
	else
	{
		while (s[*tmp] != ' ' &&s[*tmp] != '(' && s[*tmp] != 10 && s[*tmp] != 0 && s[*tmp] != ')' && s[*tmp] != ',')
			(*tmp)++;
		idx2 = *tmp;
		if (idx1 != idx2)
			word = s.substr(idx1, idx2 - idx1);
		else
			word = "";

		return word;
	}
}

unsigned int Table::GetPrimaryKeyPosition()
{
	for (unsigned int i = 0; i < attributeNum; i++)
	{
		if (tableAttribute[i].ifPrimaryKey == true)
		{
			return i;
		}
	}
}
