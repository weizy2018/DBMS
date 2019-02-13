/*
 * ShowSql.cpp
 *
 *  Created on: Feb 13, 2019
 *      Author: weizy
 */

#include "head/ShowSql.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../head/DBMS.h"

ShowSql::ShowSql(vector<string> w): words(w) {
	// TODO Auto-generated constructor stub

}

ShowSql::~ShowSql() {
	// TODO Auto-generated destructor stub
}

void ShowSql::execute() {
	if (words[1] == "databases" && words[2] == ";") {	//show databases;
		DBMS::getDBMSInst()->showDatabases();

	} else if (words[1] == "tables" && words[2] == ";") {	//show tables;
		DBMS::getDBMSInst()->showTables();
	} else {
		throw SqlSyntaxException("sql syntax error");
	}
}
