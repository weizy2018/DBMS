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

#include "../../basic/head/BasicType.h"

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
	vector<string> join;			//连接conditions的连接词(and, or)
private:
	void handleTables();
	void handleConditions();
	void checkTable();			//检查from中的table是否在当前数据库中
	void checkCondition();		//检查where中的column是否在对应的关系表中
private:
	bool check(BasicType * basic, int type, Condition * cond);
	bool check(BasicType * left, int type, string symbol, string right);
private:
	void selectAll();
	void selectAll2();

	void select1();
	void select2();

	void select();
};

class Condition {
public:
	string table1;
	string column1;
	unsigned int column1Index;
	string table2;
	string column2;
	unsigned int column2Index;
	string symbol;
public:
	Condition(string tab1, string col1, string tab2, string col2, string sym) {
		this->table1 = tab1;
		this->column1 = col1;
		this->table2 = tab2;
		this->column2 = col2;
		this->symbol = sym;
		column1Index = -1;
		column2Index = -1;
	}
};

#endif /* SQL_HEAD_SELECTSQL_H_ */
