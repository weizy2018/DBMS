/*
 * DatabaseCreateException.cpp
 *
 *  Created on: Dec 3, 2018
 *      Author: weizy
 */

#include "head/DatabaseCreateException.h"
#include <iostream>
using namespace std;

DatabaseCreateException::DatabaseCreateException() {
	// TODO Auto-generated constructor stub
	message = "";

}
DatabaseCreateException::DatabaseCreateException(string message) {
	this->message = message;
}

DatabaseCreateException::~DatabaseCreateException() {
	// TODO Auto-generated destructor stub
}

const char * DatabaseCreateException::what() const throw() {
	string str = "DatabaseCreateException: ";
	str += message;
	cout << str << endl;

	return str.c_str();
}
