/*
 * CreateSql.cpp
 *
 *  Created on: Dec 5, 2018
 *      Author: weizy
 */

#include "head/CreateSql.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string.h>

#include "../exception/head/SqlSyntaxException.h"
#include "../exception/head/DatabaseCreateException.h"
#include "../exception/head/TableCreateException.h"
#include "../exception/head/IndexCreateException.h"

#include "../head/Global.h"
#include "../head/DBMS.h"


#define INT			"int"
#define FLOAT		"float"
#define DOUBLE		"double"
#define CHAR		"char"
#define VARCHAR		"varchar"


CreateSql::CreateSql(const vector<string> ws) :words(ws) {
	// TODO Auto-generated constructor stub

}

CreateSql::~CreateSql() {
	// TODO Auto-generated destructor stub
}

void CreateSql::create() {

}
void CreateSql::execute() {
	if (words[2] == ";") {
		throw SqlSyntaxException("sql syntax error");
	}
	if (words[1] == DATABASE) {
		cout << "create database" << endl;
		char * dbName = (char*)malloc(words[2].size() + 1);
		strcpy(dbName, words[2].c_str());
		int blockSize = inputBlockSize();
		try {
			DBMS::getDBMSInst()->createDatabase(dbName, blockSize);
		} catch (exception & e) {
			e.what();
		}

	} else if (words[1] == TABLE) {
		cout << "create table" << endl;
		set<string> s;
		vector<pair<string, pair<string, int>>> attrs;
		//必须在use database之后才能创建表
		char * relName = (char*)malloc(Global::MAX_RELATION_NAME);
		if (words[2].size() > Global::MAX_RELATION_NAME) {
			throw TableCreateException("the length of table name to long");
		}
		strcpy(relName, words[2].c_str());
		if (words[3] != "(") {
			throw SqlSyntaxException("sql syntax error");
		}
		for (unsigned int i = 4; i < words.size() - 2; i++) {
			if (words[i] == ",") {
				continue;
			}
			char attrName[Global::MAX_ATTRIBUTE_NAME];
			char type[Global::TYPE_LENGTH];

			//属性的名称
			strcpy(attrName, words[i].c_str());
			i++;
			if (i < words.size() - 2) {
				if (words[i] == CHAR || words[i] == VARCHAR) {
					strcpy(type, words[i].c_str());
					if (i + 3 > words.size() - 2 || words[i+1] != "(" || words[i+3] != ")") {
						throw SqlSyntaxException("sql syntax error");
					}
					int value = stoi(words[i+2]);	//转换错误抛出invalid_argument异常
					pair<set<string>::iterator, bool> ret;
					ret = s.insert(attrName);
					if (ret.second) {
						attrs.push_back(pair<string, pair<string, int>>(attrName, pair<string, int>(type, value)));
					} else {
						string error("the attribute \'");
						error.append(attrName);
						error.append("\' already exist");
						throw SqlSyntaxException(error);
					}
					i += 3;

				} else if (words[i] == INT || words[i] == FLOAT || words[i] == DOUBLE) {
					int value;
					if (words[i] == INT) {
						value = sizeof(int);
					} else if (words[i] == FLOAT) {
						value = sizeof(float);
					} else if (words[i] == DOUBLE) {
						value = sizeof(double);
					}
					strcpy(type, words[i].c_str());
					pair<set<string>::iterator, bool> ret;
					ret = s.insert(attrName);
					if (ret.second) {
						attrs.push_back(pair<string, pair<string, int>>(attrName, pair<string, int>(type, value)));
					} else {
						string error("the attribute \'");
						error.append(attrName);
						error.append("\' already exist");
						throw SqlSyntaxException(error);
					}

				} else {
					throw SqlSyntaxException("the word \'" + words[i] + "\' is undefined");
				}
			}
		}
		DBMS::getDBMSInst()->createTable(relName, attrs);
	} else if (words[1] == INDEX) {
//		cout << "create index" << endl;
		//create index student_name on student(name);
//		char indexName[Global::MAX_IDNEX_NAME];
//		char tableName[Global::MAX_RELATION_NAME];
//		char arrtName[Global::MAX_ATTRIBUTE_NAME];

		if (words.size() != 9) {
			throw SqlSyntaxException("sql syntax error");
		}
		if (words[3] != "on" || words[5] != "(") {
			throw SqlSyntaxException("sql syntax error");
		}
		if (words[2].length() > Global::MAX_IDNEX_NAME) {
			throw IndexCreateException("the index name is to long");
		}

//		cout << "create index" << endl;
//		cout << words[2] << " " << words[4] << " " << words[6] << endl;
		//DBMS::createIndex(const string indexName, const string tableName, const string attrName)

		DBMS::getDBMSInst()->createIndex(words[2], words[4], words[6]);
	} else {
		throw SqlSyntaxException("sql syntax error");
	}
}

int CreateSql::inputBlockSize() {
	int success = false;
	string str;
	int size;
	while (!success) {
		cout << "input the block size : ";
		getline(cin, str);
		try {
			size = stoi(str);
			if (size < 1 || size > 10) {
				cout << "Error : The size of the block must be between 1 and 10" << endl;
			} else {
				success = true;
			}

		} catch (invalid_argument & e) {
			cout << "Format error" << endl;
		}
	}
	return size;
}

