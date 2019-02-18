/*
 * DescSql.h
 *
 *  Created on: Feb 18, 2019
 *      Author: weizy
 */

#ifndef SQL_HEAD_DESCSQL_H_
#define SQL_HEAD_DESCSQL_H_

#include "ExecuteStatus.h"

#include <vector>
#include <string>

using namespace std;

class DescSql: public ExecuteStatus {
public:
	DescSql(const vector<string> ws);
	virtual ~DescSql();

public:
	void execute();

private:
	const vector<string> words;
};

#endif /* SQL_HEAD_DESCSQL_H_ */
