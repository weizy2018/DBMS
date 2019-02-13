/*
 * ShowSql.h
 *
 *  Created on: Feb 13, 2019
 *      Author: weizy
 */

#ifndef SQL_HEAD_SHOWSQL_H_
#define SQL_HEAD_SHOWSQL_H_

#include "ExecuteStatus.h"

#include <vector>
#include <string>

using namespace std;

class ShowSql: public ExecuteStatus {
public:
	ShowSql(vector<string> w);
	virtual ~ShowSql();

public:
	void execute();

private:
	const vector<string> words;
};

#endif /* SQL_HEAD_SHOWSQL_H_ */
