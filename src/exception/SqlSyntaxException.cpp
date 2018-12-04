/*
 * SqlSyntaxException.cpp
 *
 *  Created on: Dec 4, 2018
 *      Author: weizy
 */

#include "head/SqlSyntaxException.h"

SqlSyntaxException::SqlSyntaxException() {
	// TODO Auto-generated constructor stub
	message = "";
}
SqlSyntaxException::SqlSyntaxException(string message) {
	this->message = message;
}

SqlSyntaxException::~SqlSyntaxException() {
	// TODO Auto-generated destructor stub
}

const char * SqlSyntaxException::what() const throw() {
	string str = "SqlException: ";
	str += message;
	return str.c_str();
}
