/*
 * FileNotFoundException.h
 *
 *  Created on: Nov 8, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_FILENOTFOUNDEXCEPTION_H_
#define EXCEPTION_HEAD_FILENOTFOUNDEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class FileNotFoundException :public exception {
public:
	FileNotFoundException();
	FileNotFoundException(string fileName);
	virtual ~FileNotFoundException();

	const char * what () const throw ();
private:
	string fileName;
};

#endif /* EXCEPTION_HEAD_FILENOTFOUNDEXCEPTION_H_ */
