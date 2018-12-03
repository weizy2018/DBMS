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

#define SYMBOL_SIZE 10

using namespace std;

class SQL {
private:
	SQL();
	static SQL * sqlInst;

public:
	static SQL * getsqlInst();
	static void releaseInst();

public:
	virtual ~SQL();

public:
	void inputSql();
	void parse();


private:
	string sql;
	vector<string> words;
	char symbol[SYMBOL_SIZE];
private:
	bool isSymbol(char c);
};

#endif /* HEAD_SQL_H_ */
