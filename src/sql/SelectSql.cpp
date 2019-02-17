/*
 * SelectSql.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#include "head/SelectSql.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../head/Global.h"
#include "../head/DBMS.h"
#include "../head/Dictionary.h"

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
	if (words[1] != "*" || words[2] != "from") {
		throw SqlSyntaxException("sql syntax error");
	}
	handleTables();
	handleConditions();

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

//	checkTable();
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
//Table 'sampdb.abc' doesn't exist
//检查from的table是否在currentDatabase中
void SelectSql::checkTable() {
	int totalRelation = Dictionary::getDictionary()->getTotalRelation();
	int j;
	const string currentDatabase = DBMS::getDBMSInst()->getCurrentDatabase();
	for (unsigned int i = 0; i < tableNames.size(); i++) {
		for (j = 0; j < totalRelation; j++) {
			Relation * rel = Dictionary::getDictionary()->getRelation(j);
			string relName(rel->getRelationName());
			if (relName == tableNames[i]) {
				break;
			}
		}
		if (j == totalRelation) {
			string error("Table \'");
			error.append(currentDatabase);
			error.append(".");
			error.append(tableNames[i]);
			error.append("\' doesn't exist");
			throw SqlSyntaxException(error);
		}
	}
}















