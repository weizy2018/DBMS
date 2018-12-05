/*
 * CreateSql.cpp
 *
 *  Created on: Dec 5, 2018
 *      Author: weizy
 */

#include "head/CreateSql.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../exception/head/DatabaseCreateException.h"
#include <iostream>

CreateSql::CreateSql(const vector<string> words) : ExecuteStatus(words) {
	// TODO Auto-generated constructor stub

}

CreateSql::~CreateSql() {
	// TODO Auto-generated destructor stub
}

void CreateSql::create() {

}
void CreateSql::execute() {
	if (words[2] == ";") {
		throw SqlSyntaxException("sql syntax error");
	}
	if (words[1] == DATABASE) {
		cout << "create database" << endl;
		char * dbName = (char*)malloc(words[2].size() + 1);
		memcpy(dbName, words[2].c_str(), words[2].size());
		dbName[words[2].size()] = '\0';
		try {
			DBMS::getDBMSInst()->createDatabase(dbName);
		} catch (exception & e) {
			e.what();
		}

	} else if (words[1] == TABLE) {
		cout << "create table" << endl;
		//必须在use database之后才能创建表


	} else if (words[1] == INDEX) {
		cout << "create index" << endl;

	} else {
		throw SqlSyntaxException("sql syntax error");
	}
}

