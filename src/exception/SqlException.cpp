/*
 * SqlException.cpp
 *
 *  Created on: Dec 3, 2018
 *      Author: weizy
 */

#include "head/SqlException.h"

SqlException::SqlException() {
	// TODO Auto-generated constructor stub
	message = "";

}
SqlException::SqlException(string message) {
	this->message = message;
}

SqlException::~SqlException() {
	// TODO Auto-generated destructor stub
}
const char * SqlException::what() const throw() {
	string str = "SqlException: ";
	str += message;
	return str.c_str();
}

