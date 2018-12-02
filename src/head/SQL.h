/*
 * SQL.h
 *
 *  Created on: Dec 2, 2018
 *      Author: weizy
 */

#ifndef HEAD_SQL_H_
#define HEAD_SQL_H_

#include <string>
#include <vector>

using namespace std;

class SQL {
private:
	SQL();
	static SQL * sqlInst;

public:
	static SQL * getsqlInst();

public:
	virtual ~SQL();

public:
	void inputSql();
	void parse();


private:
	string sql;
	vector<string> word;
};

#endif /* HEAD_SQL_H_ */
