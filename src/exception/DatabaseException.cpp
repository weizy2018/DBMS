/*
 * DatabaseException.cpp
 *
 *  Created on: Dec 6, 2018
 *      Author: weizy
 */

#include "head/DatabaseException.h"
#include <iostream>

using namespace std;

DatabaseException::DatabaseException() {
	// TODO Auto-generated constructor stub
	message = "";
}
DatabaseException::DatabaseException(string message) {
	// TODO Auto-generated constructor stub
	this->message = message;
}

DatabaseException::~DatabaseException() {
	// TODO Auto-generated destructor stub
}

const char * DatabaseException::what () const throw () {
	cout << "DatabaseException: " << message << endl;

	return message.c_str();
}

