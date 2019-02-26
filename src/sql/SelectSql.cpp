/*
 * SelectSql.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#include "head/SelectSql.h"

#include "../head/Global.h"
#include "../head/DBMS.h"
#include "../head/Dictionary.h"
#include "../head/Block.h"
#include "../head/Tuple.h"

#include "../exception/head/SqlSyntaxException.h"
#include "../exception/head/Error.h"

#include "../tools/head/BPlusTree.h"
#include "../tools/head/Bplustree.h"

#include <string.h>


SelectSql::SelectSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

SelectSql::~SelectSql() {
	// TODO Auto-generated destructor stub
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		delete (*it);
	}
}
void SelectSql::execute() {
	if (DBMS::getDBMSInst()->getCurrentDatabase() == "") {
		throw Error("no database selected");
	}
	if (words[1] != "*" || words[2] != "from") {
		throw SqlSyntaxException("sql syntax error");
	}
	handleTables();
	checkTable();

	handleConditions();
	checkCondition();

	if (conditions.size() == 0) {
		if (tableNames.size() == 1) {
			selectAll();
		} else {
			selectAll2();
		}
	} else {
		if (tableNames.size() == 1) {
			int index = checkIndex();
			if (index != -1) {
				selectInIndex1(index);
			} else {
				select1();
			}
		} else {
			select2();
		}
	}

//	cout << "after check condition" << endl;
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		Condition * con = conditions[i];
//		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
//	}

}

void SelectSql::handleTables() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "from") {
		index++;
	}
	//没有from关键字
	if (index == words.size()) {
		throw SqlSyntaxException("sql syntax error");
	}
	index++;
	while (words[index] != ";" && words[index] != "where") {
		tableNames.push_back(words[index]);
		index++;
		if (words[index] == ",") {
			index++;
		} else if (words[index] == ";" || words[index] == "where") {
			break;
		}
	}
	if (tableNames.size() == 0) {
		throw SqlSyntaxException("sql syntax error");
	}

//	for (auto it = tableNames.begin(); it != tableNames.end(); it++) {
//		cout << *it << "  ";
//	}
//	cout << endl;
}
void SelectSql::handleConditions() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "where") {
		index++;
	}
	index++;
	if (index >= words.size()) {
		return;
	}

	//向join中加入一个"and"
	join.push_back("and");

	while (words[index] != ";") {
		string table1 = "";
		string column1 = "";
		string table2 = "";
		string column2 = "";
		string sym;

		//table.id = ... 类型
		if (index+1 < words.size() && words[index+1] == ".") {
			table1 = words[index];
			if (index+2 < words.size()) {
				column1 = words[index+2];
			} else {
				throw SqlSyntaxException("sql syntax exception");
			}
			index += 3;
		} else {				//id = ... 类型
			column1 = words[index];
			index += 1;
		}
		//符号 = 、> 、< 、>= 、<= 、由于之前的设置，这里没有!=
		if (words[index] == "=" || words[index] == ">" || words[index] == "<" || words[index] == ">=" || words[index] == "<=") {
			sym = words[index];
			index++;
		} else {
			throw SqlSyntaxException("sql syntax exception");
		}
		//... = table2.id类型
		if (index+1 < words.size() && words[index+1] == ".") {
			table2 = words[index];
			if (index+2 < words.size()) {
				column2 = words[index+2];
			} else {
				throw SqlSyntaxException("sql syntax exception");
			}
			index += 3;
		} else {			//... = id 或者 ... = "id" 或者 ... = 'id'  类型
			if (words[index] == "\"" || words[index] == "\'") {
				index++;
				column2 = words[index];
				index += 2;
			} else {
				column2 = words[index];
				index++;
			}
		}
		Condition * con = new Condition(table1, column1, table2, column2, sym);
		conditions.push_back(con);
		// and   or
		if (words[index] == "and" || words[index] == "or") {
			join.push_back(words[index]);
			index++;
		} else if (words[index] == ";") {
			break;
		} else {
			throw SqlSyntaxException("sql syntax exception");
		}
	}
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		Condition * con = conditions[i];
//		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
//	}
}
//检查from的table是否在currentDatabase中
void SelectSql::checkTable() {
	const string currentDatabase = DBMS::getDBMSInst()->getCurrentDatabase();
	for (unsigned int i = 0; i < tableNames.size(); i++) {
		Relation * r = Dictionary::getDictionary()->getRelation(tableNames[i].c_str());
		if (r == nullptr) {
			string error("Table \'");
			error.append(currentDatabase);
			error.append(".");
			error.append(tableNames[i]);
			error.append("\' doesn't exist");
			throw Error(error);
		}
	}
}
//检查where中关系表以及对应的列是否合法
void SelectSql::checkCondition() {
	for (unsigned int i = 0; i < conditions.size(); i++) {
		Condition * con = conditions[i];

		//table1.column1
		if (con->table1 == "") {
			//从from的关系表中找到该列所在的关系表
			bool flag = false;
			Relation * r;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				r = Dictionary::getDictionary()->getRelation(tableNames[j].c_str());
				if (r->hasAttribute(con->column1)) {
					flag = true;
					break;
				}
			}
			//Unknown column 'student_name' in 'where clause'
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			} else {
				con->table1 = r->getRelationName();
			}
		} else {
			//需判断table1是否在tableNames中
			bool flag = false;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				if (con->table1 == tableNames.at(j)) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->table1);
				error.append(".");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
			//检查是否有对应的column
			Relation * rel = Dictionary::getDictionary()->getRelation(con->table1.c_str());
			if (!(rel->hasAttribute(con->column1))) {
				string error("Unknown column \'");
				error.append(con->table1);
				error.append(".");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
		}

		//table2.column2 / value
		if (con->table2 == "") {
			//do nothing
		} else {
			bool flag = false;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				if (con->table2 == tableNames.at(j)) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->table2);
				error.append(".");
				error.append(con->column2);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
		}
	}
	for (unsigned int i = 0; i < conditions.size(); i++) {
		if (conditions[i]->table1 != "") {
			Relation * rel1 = Dictionary::getDictionary()->getRelation(conditions[i]->table1.c_str());
			unsigned int column1Index = rel1->getAttributeIndex(conditions[i]->column1);
			conditions[i]->column1Index = column1Index;
		}
		if (conditions[i]->table2 != "") {
			Relation * rel2 = Dictionary::getDictionary()->getRelation(conditions[i]->table2.c_str());
			unsigned int column2Index = rel2->getAttributeIndex(conditions[i]->column2);
			conditions[i]->column2Index = column2Index;
		}
	}
}
//tableNames.size() == 1
void SelectSql::selectAll() {
	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames.at(0).c_str());
	unsigned int totalBlock = rel->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), i);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), i, block);
		}
		vector<Tuple *> tuples = block->getBlockTupls();
		for (unsigned int i = 0; i < tuples.size(); i++) {
			Tuple * tup = tuples[i];
			tup->printTuple();
			cout << endl;
		}
		//释放内存
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			delete (*it);
		}
	}
}
//tableNames.size() == 2	最多只能支持两个表联立了
void SelectSql::selectAll2() {
	Relation * rel1 = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	Relation * rel2 = Dictionary::getDictionary()->getRelation(tableNames[1].c_str());
	unsigned int totalBlock1 = rel1->getTotalBlock();
	unsigned int totalBlock2 = rel2->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock1; i++) {
		Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], i);
		if (block1 == nullptr) {
			block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames[0], i, block1);
		}
		vector<Tuple *> tuples1 = block1->getBlockTupls();
		for (unsigned int j = 0; j < totalBlock2; j++) {
			Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], j);
			if (block2 == nullptr) {
				block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
				DBMS::getDBMSInst()->putBlock(tableNames[1], j, block2);
			}
			vector<Tuple *> tuples2 = block2->getBlockTupls();
			for (auto it1 = tuples1.begin(); it1 != tuples1.end(); it1++) {
				for (auto it2 = tuples2.begin(); it2 != tuples2.end(); it2++) {
					(*it1)->printTuple();
					(*it2)->printTuple();
					cout << endl;
				}
			}
			for (auto it = tuples2.begin(); it != tuples2.end(); it++) {
				delete (*it);
			}
		}

		for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
			delete (*it);
		}
	}
}
//条件语句中没有优先级之分(没有括号),同意从左到右依次运算
//单表查询
void SelectSql::select1() {
	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	unsigned int totalBlock = rel->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock; i++)  {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), i);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), i, block);
		}
		vector<Tuple *> tuples = block->getBlockTupls();
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			bool flag = true;
			for (unsigned int k = 0; k < conditions.size(); k++) {
				BasicType * left = (*it)->getTupleBasicType(conditions[k]->column1Index);
				int type = rel->getTypeName(conditions[k]->column1Index);
				bool result = check(left, type, conditions[k]->symbol, conditions[k]->column2);
				if (join[k] == "or") {
					flag |= result;
				} else {
					flag &= result;
				}
			}
			if (flag) {
				(*it)->printTuple();
				cout << endl;
			}

		}
		//释放内存
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			delete (*it);
		}
	}
}
void SelectSql::select2() {

}
//含有索引的单表查询
//select * from student where id = '123' and name = 'zhangsan';
void SelectSql::selectInIndex1(int index) {
//	cout << "search in index" << endl;
	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	int type = rel->getTypeName(conditions[index]->column1Index);

	set<unsigned long int> blocksId;

	if (type == Global::CHAR || type == Global::VARCHAR) {
		BPlusTree<string, unsigned long int> * stringTree =
				Dictionary::getDictionary()->getStringIndex(conditions[index]->table1, conditions[index]->column1);
		blocksId = stringTree->get(conditions[index]->column2);
//		cout << "b tree" << endl;
//		stringTree->printTree();
	} else if (type == Global::INTEGER) {
		Bplustree<int, unsigned long int> * intTree =
				Dictionary::getDictionary()->getIntIndex(conditions[index]->table1, conditions[index]->column1);
		int key;
		try {
			key = stoi(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'int\'");
			throw Error(error);
		}
		blocksId = intTree->get(key);
	} else if (type == Global::FLOAT) {
		Bplustree<float, unsigned long int> * floatTree =
				Dictionary::getDictionary()->getFloatIndex(conditions[index]->table1, conditions[index]->column1);
		float key;
		try {
			key = stof(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'float\'");
			throw Error(error);
		}
		blocksId = floatTree->get(key);
	} else if (type == Global::DOUBLE) {
		Bplustree<double, unsigned long int> * doubleTree =
				Dictionary::getDictionary()->getDoubleIndex(conditions[index]->table1, conditions[index]->column1);
		double key;
		try {
			key = stod(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'double\'");
			throw Error(error);
		}
		blocksId = doubleTree->get(key);
	}

//	cout << "blocksId.size() = " << blocksId.size() << endl;

	for (auto it = blocksId.begin(); it != blocksId.end(); it++) {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), *it);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *it);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), *it, block);
		}

		vector<Tuple *> tuples = block->getBlockTupls();
		for (auto tup = tuples.begin(); tup != tuples.end(); tup++) {
			bool flag = true;
			for (unsigned int i = 0; i < conditions.size(); i++) {
				BasicType * left = (*tup)->getTupleBasicType(conditions[i]->column1Index);
				int type = rel->getTypeName(conditions[i]->column1Index);
				bool result = check(left, type, conditions[i]->symbol, conditions[i]->column2);
				if (join[i] == "or") {
					flag |= result;
				} else {
					flag &= result;
				}
			}
			if (flag) {
				(*tup)->printTuple();
				cout << endl;
			}
		}
		//释放内存
		for (auto tup = tuples.begin(); tup != tuples.end(); tup++) {
			delete (*tup);
		}
	}

}

bool SelectSql::check(BasicType * left, int type, string symbol, string right) {
	bool flag = false;
	if (type == Global::INTEGER) {
		int * leftData = (int*)left->getData();
		int rightData;
		try {
			rightData = stoi(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'int\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::FLOAT) {
		float * leftData = (float*)left->getData();
		float rightData;
		try {
			rightData = stof(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'float\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::DOUBLE) {
		double * leftData = (double*)left->getData();
		double rightData;
		try {
			rightData = stod(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'double\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::CHAR) {
		char * data = left->getData();
		string leftData(data);
		if (symbol == "=") {
			return leftData == right;
		} else if (symbol == ">") {
			return leftData > right;
		} else if (symbol == "<") {
			return leftData < right;
		} else if (symbol == ">=") {
			return leftData >= right;
		} else if (symbol == "<=") {
			return leftData <= right;
		}
	} else if (type == Global::VARCHAR) {
		char * data = left->getData();
		string leftData(data);
		if (symbol == "=") {
			return leftData == right;
		} else if (symbol == ">") {
			return leftData > right;
		} else if (symbol == "<") {
			return leftData < right;
		} else if (symbol == ">=") {
			return leftData >= right;
		} else if (symbol == "<=") {
			return leftData <= right;
		}
	}
	return flag;
}

//检查是否可以使用索引搜索（where 字句只有一个条件或者所有条件都是使用 and 连接起来的，并且所在列已经创建了索引)
//返回where字句中第几个条件下标，如果没有返回-1
int SelectSql::checkIndex() {
	//先检查是否都是用and连接起来
	for (auto it = join.begin(); it != join.end(); it++) {
		if (*it != "and") {
			return -1;
		}
	}
	//检查condition中的table1.column1是否创建了索引 (有一个就得了)
	int index = -1;
	for (unsigned int i = 0 ; i < conditions.size(); i++) {
		if (Dictionary::getDictionary()->isIndex(conditions[i]->table1, conditions[i]->column1)) {
			index = i;
			break;
		}
	}
	return index;
}

















