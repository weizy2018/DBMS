/*
 * DatabaseCreateException.h
 *
 *  Created on: Dec 3, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_DATABASECREATEEXCEPTION_H_
#define EXCEPTION_HEAD_DATABASECREATEEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class DatabaseCreateException :public exception {
public:
	DatabaseCreateException();
	DatabaseCreateException(string message);
	virtual ~DatabaseCreateException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_DATABASECREATEEXCEPTION_H_ */
