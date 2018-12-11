/*
 * IndexCreateException.cpp
 *
 *  Created on: Dec 11, 2018
 *      Author: weizy
 */

#include "head/IndexCreateException.h"
#include <iostream>

IndexCreateException::IndexCreateException() {
	// TODO Auto-generated constructor stub
	message = "";

}
IndexCreateException::IndexCreateException(string message) {
	this->message = message;
}

IndexCreateException::~IndexCreateException() {
	// TODO Auto-generated destructor stub
}

const char * IndexCreateException::what () const throw () {
	string error("IndexCreateException: ");
	error.append(message);
	cout << error << endl;
	return error.c_str();
}

