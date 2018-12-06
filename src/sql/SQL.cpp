/*
 * SQL.cpp
 *
 *  Created on: Dec 2, 2018
 *      Author: weizy
 */

#include "head/SQL.h"
#include <iostream>
#include <string.h>
#include <stack>

#include "../exception/head/SqlSyntaxException.h"
#include "head/CreateSql.h"
#include "head/ExecuteStatus.h"
#include "../exception/head/DatabaseException.h"
#include "../exception/head/TableCreateException.h"

SQL * SQL::sqlInst = nullptr;

SQL::SQL() {
	// TODO Auto-generated constructor stub
	char sym[] = {'*', '=', ',', '\'', '\"', '(', ')', ';', '>', '<'};
	for (int i = 0; i < SYMBOL_SIZE; i++) {
		symbol[i] = sym[i];
	}
	finish = false;
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
//	cout << "SQL::inputSql()" << endl;
	sql = "";
	words.clear();
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

//	cout << "input finish :" << sql << endl;
}

void SQL::parse() {
	if (!check()) {
		throw SqlSyntaxException("sql syntax error");
	}
	for (unsigned int i = 0; i < sql.size(); i++) {
		string word = "";
		while (sql.at(i) == ' ') {
			i++;
		}
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
//	cout << "parse finish !" << endl;
//	for (unsigned int i = 0; i < words.size(); i++) {
//		cout << words[i] << endl;
//	}
}
void SQL::execute() {
	ExecuteStatus * executeStatus = nullptr;
	if (words[0] == CREATE) {
		cout << "create" << endl;
		executeStatus = new CreateSql(words);

	} else if (words[0] == SELECT) {
		cout << "select" << endl;

	} else if (words[0] == INSERT) {
		cout << "insert" << endl;

	} else if (words[0] == DELETE) {
		cout << "delete" << endl;

	} else if (words[0] == USE) {
		cout << "use" << endl;

	} else if (words[0] == SHOW) {
		cout << "show" << endl;

	} else if (words[0] == DESC) {
		cout << "desc" << endl;

	} else if (words[0] == EXIT) {
		cout << "exit" << endl;
		finish = true;
	} else if (words[0] == DROP) {
		cout << "drop" << endl;
	} else if (words[0] == STATUS) {
		cout << "status" << endl;
	} else {
		throw SqlSyntaxException("the word \'" + words[0] + "\' is undefined");
	}
	if (executeStatus) {
		try {
			executeStatus->execute();
		} catch (SqlSyntaxException & e) {
			e.what();
		} catch (invalid_argument & e) {
			cout << e.what() << endl;
		} catch (DatabaseException & e) {
			e.what();
		} catch (TableCreateException & e) {
			e.what();
		}

		delete executeStatus;
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
bool SQL::check() {
	stack<char> s;
	for (unsigned int i = 0; i < sql.size(); i++) {
		if (sql.at(i) == '(') {
			s.push(sql.at(i));
		} else if (sql.at(i) == ')') {
			if (s.empty() || s.top() != '(') {
				return false;
			} else {
				s.pop();
			}
		}
	}
	if (!s.empty()) {
		return false;
	} else {
		return true;
	}

}
bool SQL::isFinish() {
	return finish;
}












