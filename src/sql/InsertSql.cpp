/*
 * InsertSql.cpp
 *
 *  Created on: Dec 7, 2018
 *      Author: weizy
 */

#include "head/InsertSql.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../head/Global.h"

#include <string.h>
#include <iostream>




InsertSql::InsertSql(const vector<string> ws) : words(ws){
	// TODO Auto-generated constructor stub

}

InsertSql::~InsertSql() {
	// TODO Auto-generated destructor stub
}
//insert into tb values("aa bb cc", 34, 54.8, 'abd def', "sdf", 'aaa');

void InsertSql::execute() {
	cout << "InsertSql::execute()" << endl;

	if (words[1] != "into") {
		throw SqlSyntaxException("SQL syntax error");
	}
	if (words[3] != "values" || words[4] != "(") {
		throw SqlSyntaxException("SQL syntax error");
	}

	char tableName[Global::MAX_RELATION_NAME];
	vector<string> values;
	strcpy(tableName, words[2].c_str());


	for (unsigned int i = 5; i < words.size() - 2; i++) {
		if (words[i] == "\"" || words[i] == "\'" || words[i] == ",") {
			continue;
		}
		values.push_back(words[i]);
	}

	for (auto it = values.begin(); it != values.end(); it++) {
		cout << *it << endl;
	}

}













