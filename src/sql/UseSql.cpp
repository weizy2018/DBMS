/*
 * UseSql.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#include "head/UseSql.h"
#include "../head/Global.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../head/DBMS.h"

#include <string.h>

UseSql::UseSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

UseSql::~UseSql() {
	// TODO Auto-generated destructor stub
}

void UseSql::execute() {
	char databaseName[Global::MAX_DATABASE_NAME];
	if (words.size() < 3 || words[2] != ";") {
		throw SqlSyntaxException("sql syntax error");
	}
	strcpy(databaseName, words[1].c_str());
	DBMS::getDBMSInst()->changeDatabase(databaseName);
}









