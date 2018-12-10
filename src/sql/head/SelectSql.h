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

class SelectSql : public ExecuteStatus {
public:
	SelectSql(const vector<string> ws);
	virtual ~SelectSql();
public:
	void execute();
private:
	const vector<string> words;
};

#endif /* SQL_HEAD_SELECTSQL_H_ */
