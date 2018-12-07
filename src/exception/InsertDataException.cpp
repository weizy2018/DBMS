/*
 * InsertDataException.cpp
 *
 *  Created on: Dec 7, 2018
 *      Author: weizy
 */

#include "head/InsertDataException.h"

#include <iostream>

InsertDataException::InsertDataException() {
	// TODO Auto-generated constructor stub
	message = "";

}
InsertDataException::InsertDataException(string message) {
	this->message = message;
}

InsertDataException::~InsertDataException() {
	// TODO Auto-generated destructor stub
}
const char * InsertDataException::what () const throw () {
	string error("InsertDataException: ");
	error.append(message);
	cout << error << endl;
	return error.c_str();
}





