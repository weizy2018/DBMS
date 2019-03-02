/*
 * UpdateSql.h
 *
 *  Created on: Mar 2, 2019
 *      Author: weizy
 */

#ifndef SQL_HEAD_UPDATESQL_H_
#define SQL_HEAD_UPDATESQL_H_

#include "ExecuteStatus.h"
#include "../../head/Dictionary.h"

#include <string>
#include <vector>

using namespace std;

class Expression;

class UpdateSql : public ExecuteStatus {
public:
	UpdateSql(const vector<string> words);
	virtual ~UpdateSql();
public:
	void execute();

private:
	void handleSet();
	void handleWhere();

	void checkSet(Relation * rel);
	void checkWhere(Relation * rel);

private:
	void update(Relation * rel);
	void updateBlock(Block * block, Relation * rel, bool lastBlock);

	int checkIndex();
	int findIndex();
	bool check(BasicType * left, int type, string symbol, string right);
	int isInExpres(int index);

private:
	vector<string> words;

	vector<Expression *> exprs;
	vector<Expression *> conditions;
	vector<string> joins;
};

class Expression {
public:
	Expression(string left, string right, string symbol, string table) {
		this->left = left;
		this->right = right;
		this->symbol = symbol;
		leftIndex = -1;
		this->table = table;
	}
public:
	string table;
	string left;
	string right;
	string symbol;

	int leftIndex;
};

#endif /* SQL_HEAD_UPDATESQL_H_ */
