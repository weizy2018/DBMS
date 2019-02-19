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
#include "../exception/head/DatabaseException.h"
#include "../exception/head/TableCreateException.h"
#include "../exception/head/QueryException.h"
#include "../exception/head/IndexCreateException.h"
#include "../exception/head/InsertDataException.h"
#include "../exception/head/FileNotFoundException.h"
#include "../exception/head/Error.h"

#include "head/CreateSql.h"
#include "head/ExecuteStatus.h"
#include "head/InsertSql.h"
#include "head/UseSql.h"
#include "head/SelectSql.h"
#include "head/ShowSql.h"
#include "head/DescSql.h"

SQL * SQL::sqlInst = nullptr;

SQL::SQL() {
	// TODO Auto-generated constructor stub
	char sym[] = {'*', '=', ',', '\'', '\"', '(', ')', ';', '>', '<', '.'};
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
	sqlInst = nullptr;
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
		throw SqlSyntaxException("sql syntax error1");
	}
	for (unsigned int i = 0; i < sql.size(); i++) {
		string word = "";
		while (sql.at(i) == ' ') {
			i++;
		}
		if (isSymbol(sql.at(i))) {
			word += sql.at(i);
			if (sql.at(i) == '\'' || sql.at(i) == '\"') {
				words.push_back(word);
				word = "";
				char temp = sql.at(i);
				i++;
				while (i < sql.size() && sql.at(i) != temp) {		//temp 为\'或者\"
					word += sql.at(i);
					i++;
				}
				if (i == sql.size()) {
					throw SqlSyntaxException("sql syntax error2");
				}
				words.push_back(word);
				word = "";
				word += sql.at(i);				//sql.at(i) == temp
				words.push_back(word);

			} else {
				i++;
				//sym[] = {'*', '=', ',', '\'', '\"', '(', ')', ';', '>', '<', '.'};
				if (i < sql.size() && sql.at(i) == '=') {	//处理>=、<=的情况
					word += '=';
					i++;
				}
				words.push_back(word);
				i--;
			}

			continue;
		}

		while (i < sql.size() && sql.at(i) != ' ') {
			if (isSymbol(sql.at(i))) {
				if (sql.at(i) == '.') {
					bool flag = false;
					for (unsigned int k = 0; k < word.size(); k++) {
						if (word[k] < '0' || word[k] > '9') {
							flag = true;
							break;
						}
					}
					if (flag) {
						break;
					} else {
						word += sql.at(i);
						i++;
					}
				} else {
					break;
				}
			} else {
				word += sql.at(i);
				i++;
			}

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
		executeStatus = new CreateSql(words);

	} else if (words[0] == SELECT) {
		executeStatus = new SelectSql(words);

	} else if (words[0] == INSERT) {
		cout << "insert" << endl;
		executeStatus = new InsertSql(words);

	} else if (words[0] == UPDATE) {
		cout << "update" << endl;

	} else if (words[0] == DELETE) {
		cout << "delete" << endl;

	} else if (words[0] == USE) {
		executeStatus = new UseSql(words);

	} else if (words[0] == SHOW) {
		executeStatus = new ShowSql(words);

	} else if (words[0] == DESC) {
		executeStatus = new DescSql(words);

	} else if (words[0] == EXIT) {
		DBMS::releaseDBMSInst();
		finish = true;
		cout << "Bye" << endl;
	} else if (words[0] == DROP) {
		cout << "drop" << endl;
	} else if (words[0] == STATUS) {		//用select database()代替
		cout << "status" << endl;
	} else {
		throw Error("the word \'" + words[0] + "\' is undefined");
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
		} catch (QueryException & e) {
			e.what();
		} catch (IndexCreateException & e) {
			e.what();
		} catch (InsertDataException & e) {
			e.what();
		} catch (FileNotFoundException & e) {
			e.what();
		} catch (Error & e) {
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
	stack<char> s;		//括号匹配
	stack<char> s2;		//引号匹配
	bool left = true;
	for (unsigned int i = 0; i < sql.size(); i++) {
		if (sql.at(i) == '(') {
			s.push(sql.at(i));
		} else if (sql.at(i) == ')') {
			if (s.empty() || s.top() != '(') {
				return false;
			} else {
				s.pop();
			}
		} else if (sql.at(i) == '\'' || sql.at(i) == '\"') {
			if (left) {
				s2.push(sql.at(i));
				left = false;
			} else {
				if (s2.empty() || s2.top() != sql.at(i)) {
					return false;
				} else {
					s2.pop();
					left = true;
				}
			}

		}
	}
	if (!s.empty() || !s2.empty()) {
		return false;
	} else {
		return true;
	}

}
bool SQL::isFinish() {
	return finish;
}












