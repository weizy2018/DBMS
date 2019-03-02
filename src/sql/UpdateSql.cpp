/*
 * UpdateSql.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: weizy
 */

#include "head/UpdateSql.h"
#include "../head/Dictionary.h"
#include "../head/DBMS.h"
#include "../exception/head/Error.h"

#include <iostream>

UpdateSql::UpdateSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

UpdateSql::~UpdateSql() {
	// TODO Auto-generated destructor stub
	for (auto it = exprs.begin(); it != exprs.end(); it++) {
		delete (*it);
	}
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		delete (*it);
	}
}
//update Person set Address = 'Zhongshan', City = 'Nanjing', age = 18 where LastName = 'Wilson' and firstName = "wei"
void UpdateSql::execute() {
	if (DBMS::getDBMSInst()->getCurrentDatabase() == "") {
		throw Error("no database selected");
	}
	if (words.size() < 7) {				//至少也得有7个单词吧
		throw Error("sql syntax error");
	}
	Relation * rel = Dictionary::getDictionary()->getRelation(words[1].c_str());
	if (rel == nullptr) {
		string error("Table \'");
		error.append(DBMS::getDBMSInst()->getCurrentDatabase());
		error.append(".");
		error.append(words[1]);
		error.append("\' doesn't exist");
		throw Error(error);
	}
	handleSet();
	checkSet(rel);

	handleWhere();
	checkWhere(rel);

//	cout << "expression:" << endl;
//	for (unsigned int i = 0; i < exprs.size(); i++) {
//		cout << exprs[i]->left << " " << exprs[i]->symbol << " " << exprs[i]->right << " index = " << exprs[i]->leftIndex << endl;
//	}
//	cout << "conditions:" << endl;
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		cout <<conditions[i]->table << "." << conditions[i]->left << " " << conditions[i]->symbol << " " << conditions[i]->right;
//		cout << " index = " << conditions[i]->leftIndex << endl;
//	}

}

void UpdateSql::handleSet() {
	unsigned int index = 3;
	while (index < words.size()) {
		string left = words[index];
		string symbol;
		string right;
		if (index + 1 < words.size()) {
			if (words[index+1] != "=") {
				throw Error("sql syntax error");
			}
			symbol = words[index+1];
		} else {
			throw Error("sql syntax error");
		}
		if (index + 2 < words.size()) {
			if (words[index+2] == "\"" || words[index+2] == "\'") {
				right = words[index+3];
				index += 5;
			} else {
				right = words[index+2];
				index += 3;
			}
		} else {
			throw Error("sql syntax error");
		}
		Expression * exp = new Expression(left, right, symbol, words[1]);
		exprs.push_back(exp);

		if (words[index] == ",") {
			index++;
		} else if (words[index] == "where" || words[index] == ";") {
			break;
		} else {
			throw Error("sql syntax error");
		}
	}
}

void UpdateSql::handleWhere() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "where") {
		index++;
	}
	index++;
	if (index >= words.size()) {
		return;
	}
	joins.push_back("and");
	while (words[index] != ";") {
		string left;
		string right;
		string symbol;
		string table;
		if (index+1 < words.size() && words[index+1] == ".") {
			table = words[index];
			if (index+2 < words.size()) {
				left = words[index+2];
			} else {
				throw Error("sql syntax error");
			}
			index += 3;
		} else {
			table = words[1];
			left = words[index];
			index += 1;
		}
		//由于之前进行sql解析的是否没考虑!=，所以这里没有!=
		if (words[index] == "=" || words[index] == ">" || words[index] == "<" || words[index] == ">=" || words[index] == "<=") {
			symbol = words[index];
			index++;
		} else {
			throw Error("sql syntax exception");
		}

		if (words[index] == "\"" || words[index] == "\'") {
			right = words[index + 1];
			index += 3;
		} else {
			right = words[index];
			index += 1;
		}
		Expression * con = new Expression(left, right, symbol, table);
		conditions.push_back(con);

		if (words[index] == "and" || words[index] == "or") {
			joins.push_back(words[index]);
			index += 1;
		} else if (words[index] == ";") {
			break;
		} else {
			throw Error("sql syntax error");
		}
	}
}

void UpdateSql::checkSet(Relation * rel) {
	if (exprs.size() == 0) {
		throw Error("sql syntax error");
	} else {
		for (auto it = exprs.begin(); it != exprs.end(); it++) {
			int index = rel->getAttributeIndex((*it)->left);
			if (index == -1) {
				string error("Unknown column \'");
				error.append(words[1]);
				error.append(".");
				error.append((*it)->left);
				error.append("\' in \'field list\'");
				throw Error(error);
			} else {
				(*it)->leftIndex = index;
			}
		}
	}
}
void UpdateSql::checkWhere(Relation * rel) {
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		if ((*it)->table != words[1]) {
			string error("Unknown column \'");
			error.append((*it)->table);
			error.append(".");
			error.append((*it)->left);
			error.append("\' in \'where clause\'");
			throw Error(error);
		}
		int index = rel->getAttributeIndex((*it)->left);
		if (index == -1) {
			string error("Unknown column \'");
			error.append(words[1]);
			error.append(".");
			error.append((*it)->left);
			error.append("\' in \'where clause\'");
			throw Error(error);
		} else {
			(*it)->leftIndex = index;
		}
	}
}





















