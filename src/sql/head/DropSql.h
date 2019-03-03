/*
 * DropSql.h
 *
 *  Created on: Mar 3, 2019
 *      Author: weizy
 */

#ifndef SQL_HEAD_DROPSQL_H_
#define SQL_HEAD_DROPSQL_H_

#include "ExecuteStatus.h"

#include <string>
#include <vector>

using namespace std;

class DropSql: public ExecuteStatus {
public:
	DropSql(const vector<string> words);
	virtual ~DropSql();

public:
	void execute();

private:
	const vector<string> words;
};

#endif /* SQL_HEAD_DROPSQL_H_ */
