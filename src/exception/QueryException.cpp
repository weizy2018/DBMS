/*
 * QueryException.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#include "head/QueryException.h"
#include <iostream>
#include <string>

using namespace std;

QueryException::QueryException() {
	// TODO Auto-generated constructor stub
	message = "";
}
QueryException::QueryException(string message) {
	this->message = message;
}

QueryException::~QueryException() {
	// TODO Auto-generated destructor stub
}

const char * QueryException::what () const throw () {
	cout << "QueryException: " << message << endl;
	string error("QueryException: ");
	error.append(message);
	return error.c_str();
}




