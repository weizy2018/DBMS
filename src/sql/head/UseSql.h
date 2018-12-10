/*
 * UseSql.h
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_USESQL_H_
#define SQL_HEAD_USESQL_H_

#include "ExecuteStatus.h"
#include <vector>
#include <string>

using namespace std;

class UseSql : public ExecuteStatus {
public:
	UseSql(const vector<string> ws);
	virtual ~UseSql();

public:
	void execute();
private:
	const vector<string> words;
};

#endif /* SQL_HEAD_USESQL_H_ */
