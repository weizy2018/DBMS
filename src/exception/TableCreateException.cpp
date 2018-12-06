/*
 * TableCreateException.cpp
 *
 *  Created on: Dec 6, 2018
 *      Author: weizy
 */

#include "head/TableCreateException.h"

#include <iostream>
using namespace std;

TableCreateException::TableCreateException() {
	// TODO Auto-generated constructor stub
	message = "";

}
TableCreateException::TableCreateException(string message) {
		this->message = message;
}

TableCreateException::~TableCreateException() {
	// TODO Auto-generated destructor stub
}
const char * TableCreateException::what() const throw() {
	string str = "TableCreateException: ";
	str += message;
	cout << str << endl;

	return str.c_str();
}

