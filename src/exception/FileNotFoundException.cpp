/*
 * FileNotFoundException.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: weizy
 */

#include "head/FileNotFoundException.h"
#include <iostream>
using namespace std;

FileNotFoundException::FileNotFoundException() {
	fileName = "";

}
FileNotFoundException::FileNotFoundException(string fileName) {
	this->fileName = fileName;
}

FileNotFoundException::~FileNotFoundException() {
	// TODO Auto-generated destructor stub
}

const char * FileNotFoundException::what() const throw() {
	string error = "File \'" + fileName + "\' not found";
	cout << "FileNotFoundException: " << error << endl;

	return error.c_str();
}

