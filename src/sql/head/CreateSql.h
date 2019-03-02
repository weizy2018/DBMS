/*
 * CreateSql.h
 *
 *  Created on: Dec 5, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_CREATESQL_H_
#define SQL_HEAD_CREATESQL_H_


#include "ExecuteStatus.h"
#include <string>
#include <vector>

using namespace std;

#define DATABASE	"database"
#define INDEX		"index"
#define TABLE		"table"

class CreateSql : public ExecuteStatus {
public:
	CreateSql(const vector<string> words);
	virtual ~CreateSql();
public:
	void execute();
private:
	int inputBlockSize();
private:
	vector<string> words;

};

#endif /* SQL_HEAD_CREATESQL_H_ */
