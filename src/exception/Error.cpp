/*
 * Error.cpp
 *
 *  Created on: Feb 18, 2019
 *      Author: weizy
 */

#include "head/Error.h"

#include <iostream>

Error::Error() {
	// TODO Auto-generated constructor stub
	message = "";
}
Error::Error(string mesg) {
	message = mesg;
}

Error::~Error() {
	// TODO Auto-generated destructor stub
}
const char * Error::what() const throw() {
	string str = "ERROR: ";
	str += message;
	cout << str << endl;
	return str.c_str();
}

