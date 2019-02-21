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

#include <string.h>


SelectSql::SelectSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

SelectSql::~SelectSql() {
	// TODO Auto-generated destructor stub
}

//select * from table1, table2 where name = 'weizy' and year >= 20
//select * from table1, table2 where table1.id = table2.id;

//mysql> select * from student where student.id = '123';
//ERROR 1054 (42S22): Unknown column 'student.id' in 'where clause'

//mysql> select * from abc, def;
//ERROR 1146 (42S02): Table 'sampdb.abc' doesn't exist
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
		selectAll();
	}

//	cout << "after check condition" << endl;
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		Condition * con = conditions[i];
//		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
//	}


//	char tableName[Global::MAX_RELATION_NAME];
//	strcpy(tableName, words[3].c_str());
//
//	string name(words[3]);


//	tableNames.push_back(name);

//	DBMS::getDBMSInst()->select(tableNames, condition);

//	for (auto it = words.begin(); it != words.end(); it++) {
//		cout << *it << endl;
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

	for (auto it = tableNames.begin(); it != tableNames.end(); it++) {
		cout << *it << "  ";
	}
	cout << endl;
}
//select * from table1, table2 where table1.id = table2.id and age > 35;
void SelectSql::handleConditions() {
	cout << "handleContions()" << endl;
	unsigned int index = 0;
	while (index < words.size() && words[index] != "where") {
		index++;
	}
	index++;
	if (index >= words.size()) {
		return;
	}
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
	for (unsigned int i = 0; i < conditions.size(); i++) {
		Condition * con = conditions[i];
		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
	}
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
}

void SelectSql::selectAll() {
	cout << "select all" << endl;
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
		}
		//释放内存
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			delete (*it);
		}
	}
}















