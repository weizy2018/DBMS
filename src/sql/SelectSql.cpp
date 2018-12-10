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

void SelectSql::execute() {
	if (words[1] != "*" || words[2] != "from") {
		throw SqlSyntaxException("sql syntax error");
	}
	char tableName[Global::MAX_RELATION_NAME];
	strcpy(tableName, words[3].c_str());

	string name(words[3]);
	vector<string> tableNames;
	vector<string> condition;

	tableNames.push_back(name);

	DBMS::getDBMSInst()->select(tableNames, condition);
}















