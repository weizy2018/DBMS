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
#include "../../head/DBMS.h"

#define SYMBOL_SIZE 10

#define CREATE 		"create"
#define SELECT 		"select"
#define INSERT		"insert"
#define DELETE 		"delete"
#define DROP		"drop"
#define SHOW		"show"
#define DESC		"desc"
#define EXIT		"exit"
#define USE			"use"
#define STATUS		"status"





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
	void execute();
	bool isFinish();


private:
	string sql;
	vector<string> words;
	char symbol[SYMBOL_SIZE];


private:
	bool isSymbol(char c);		//检查是否是标点符号等
	bool check();				//检查括号匹配
	bool finish;
};

#endif /* HEAD_SQL_H_ */
