/*
 * InsertSql.h
 *
 *  Created on: Dec 7, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_INSERTSQL_H_
#define SQL_HEAD_INSERTSQL_H_

#include "ExecuteStatus.h"
#include <vector>
#include <string>

using namespace std;

class InsertSql : public ExecuteStatus {
public:
	InsertSql(const vector<string> ws);
	virtual ~InsertSql();
public:
	void execute();
private:
	const vector<string> words;
};

#endif /* SQL_HEAD_INSERTSQL_H_ */
