/*
 * SelectSql.h
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_SELECTSQL_H_
#define SQL_HEAD_SELECTSQL_H_

#include "ExecuteStatus.h"
#include <vector>
#include <string>

using namespace std;

class Condition;

class SelectSql : public ExecuteStatus {
public:
	SelectSql(const vector<string> ws);
	virtual ~SelectSql();
public:
	void execute();
private:
	const vector<string> words;

	vector<string> tableNames;
	vector<Condition *> conditions;
private:
	void handleTables();
	void handleConditions();
};

class Condition {
public:
	string table1;
	string column1;
	string table2;
	string column2;
	string cond;
public:
	Condition(string tab1, string col1, string tab2, string col2, string cond) {
		this->table1 = tab1;
		this->column1 = col1;
		this->table2 = tab2;
		this->column2 = col2;
		this->cond = cond;
	}
};

#endif /* SQL_HEAD_SELECTSQL_H_ */
