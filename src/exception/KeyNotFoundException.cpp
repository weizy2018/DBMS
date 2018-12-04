/*
 * KeyNotFoundException.cpp
 *
 *  Created on: Dec 4, 2018
 *      Author: weizy
 */

#include "head/KeyNotFoundException.h"

KeyNotFoundException::KeyNotFoundException() {
	// TODO Auto-generated constructor stub
	message = "";
}

KeyNotFoundException::KeyNotFoundException(string message) {
	this->message = message;
}
KeyNotFoundException::~KeyNotFoundException() {
	// TODO Auto-generated destructor stub
}
const char * KeyNotFoundException::what() const throw() {
	string error = "KeyNotFoundException: " + message + " not found";
	return error.c_str();
}

