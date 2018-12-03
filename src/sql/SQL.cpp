/*
 * SQL.cpp
 *
 *  Created on: Dec 2, 2018
 *      Author: weizy
 */

#include "head/SQL.h"
#include <iostream>
#include <string.h>

SQL * SQL::sqlInst = nullptr;

SQL::SQL() {
	// TODO Auto-generated constructor stub
	char sym[] = {'*', '=', ',', '\'', '\"', '(', ')', ';', '>', '<'};
	for (int i = 0; i < SYMBOL_SIZE; i++) {
		symbol[i] = sym[i];
	}
}

SQL::~SQL() {
	// TODO Auto-generated destructor stub
}

SQL * SQL::getsqlInst() {
	if (sqlInst == nullptr) {
		sqlInst = new SQL;
	}
	return sqlInst;
}
void SQL::releaseInst() {
	delete sqlInst;
}

void SQL::inputSql() {
	cout << "SQL::inoutSql()" << endl;
	sql = "";
	cout << "sql> ";
	getline(cin, sql);
	string str = sql;
	if (str.size() == 0) {
		str = " ";
	}

	while (str.find(";") == string::npos) {
		cout << "  -> ";
		getline(cin, str);

		if (str.size() == 0) {
			str = " ";
		} else {
			sql += " ";
			sql += str;
		}

	}
	int index = sql.find(';');
	sql.erase(index + 1);			//清空‘；’后面的空格或者其他字符也一起清了

	cout << "input finish :" << sql << endl;
}

void SQL::parse() {
	for (unsigned int i = 0; i < sql.size(); i++) {
//		cout << "i = " << i << "sql.at(i) = " << sql.at(i) << endl;
		string word = "";
		while (sql.at(i) == ' ') {
			i++;
		}
//		cout << "i = " << i << endl;
		if (isSymbol(sql.at(i))) {
			word += sql.at(i);
			i++;
			if (i < sql.size() && sql.at(i) == '=') {
				word += '=';
				i++;
			}
			words.push_back(word);
			i--;
			continue;
		}

		while (i < sql.size() && sql.at(i) != ' ' && !isSymbol(sql.at(i))) {
			word += sql.at(i);
			i++;
		}
		i--;
		words.push_back(word);
	}
	cout << "parse finish !" << endl;
	for (unsigned int i = 0; i < words.size(); i++) {
		cout << words[i] << endl;
	}
}
bool SQL::isSymbol(char c) {
	int len = strlen(symbol);
	for (int i = 0; i < len; i++) {
		if (symbol[i] == c) {
			return true;
		}
	}
	return false;
}












