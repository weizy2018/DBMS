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
}
//select * from table1, table2 where table1.id = table2.id;
void SelectSql::handleConditions() {

}















