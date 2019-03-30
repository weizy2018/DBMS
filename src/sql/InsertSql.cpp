/*
 * InsertSql.cpp
 *
 *  Created on: Dec 7, 2018
 *      Author: weizy
 */

#include "head/InsertSql.h"
#include "../exception/head/SqlSyntaxException.h"
#include "../head/Global.h"
#include "../head/DBMS.h"

#include <string.h>
#include <iostream>

#include <fstream>




InsertSql::InsertSql(const vector<string> ws) : words(ws){
	// TODO Auto-generated constructor stub

}

InsertSql::~InsertSql() {
	// TODO Auto-generated destructor stub
}
//insert into tb values("aa bb cc", 34, 54.8, 'abd def', "sdf", 'aaa');

void InsertSql::execute() {

	if (words[1] == "test") {
		insertData();
		return;
	}

	if (words[1] != "into") {
		throw SqlSyntaxException("SQL syntax error");
	}
	if (words[3] != "values" || words[4] != "(") {
		throw SqlSyntaxException("SQL syntax error");
	}

	char tableName[Global::MAX_RELATION_NAME];
	vector<string> values;
	strcpy(tableName, words[2].c_str());


	for (unsigned int i = 5; i < words.size() - 2; i++) {
		if (words[i] == "\"" || words[i] == "\'" || words[i] == ",") {
			continue;
		}
		values.push_back(words[i]);
	}

//	for (auto it = values.begin(); it != values.end(); it++) {
//		cout << *it << endl;
//	}
	DBMS::getDBMSInst()->insert(tableName, values);

}

void InsertSql::insertData() {

	ifstream input("mydata");
	if (input.fail()) {
		cout << "file open fail" << endl;
		return;
	}

	vector<string> values;
	string tableName("data");

//	string name;
//	string sex;
//	int id;
//	for (int i = 1; i <= 31; i++) {
//		input >> name;
//		input >> sex;
//		id = i;
//		values.push_back(name);
//		values.push_back(sex);
//		values.push_back(to_string(id));
//		cout << name << " " << sex << " " << id << endl;
//
//		DBMS::getDBMSInst()->insert(tableName.c_str(), values);
//		values.clear();
//	}


//	int studentId, eventId, score;
//	for (int i = 0; i < 173; i++) {
//		input >> studentId;
//		input >> eventId;
//		input >> score;
//		cout << studentId << " " << eventId << " " << score << endl;
//		values.push_back(to_string(studentId));
//		values.push_back((to_string(eventId)));
//		values.push_back(to_string(score));
//		DBMS::getDBMSInst()->insert(tableName.c_str(), values);
//		values.clear();
//	}

//	string date;
//	string category;
//	string eventId;
//	for (int i = 0; i < 6; i++) {
//		input >> date;
//		input >> category;
//		input >> eventId;
//		values.push_back(date);
//		values.push_back(category);
//		values.push_back(eventId);
//		DBMS::getDBMSInst()->insert(tableName.c_str(), values);
//		values.clear();
//
//	}

//	string studentId;
//	string date;
//	for (int i = 0; i < 6; i++) {
//		input >> studentId;
//		input >> date;
//		values.push_back(studentId);
//		values.push_back(date);
//		DBMS::getDBMSInst()->insert(tableName.c_str(), values);
//		values.clear();
//	}

	string id;
	string year;
	string state;
	string county;
	string drug;
	for (int i = 0; i < 24062; i++) {
		cout << "i = " << i << endl;
		input >> id;
		input >> year;
		input >> state;
		input >> county;
		input >> drug;
		values.push_back(id);
		values.push_back(year);
		values.push_back(state);
		values.push_back(county);
		values.push_back(drug);
		DBMS::getDBMSInst()->insert(tableName.c_str(), values);
		values.clear();
	}

	input.close();
}













