/*
 * DatabaseException.h
 *
 *  Created on: Dec 6, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_DATABASEEXCEPTION_H_
#define EXCEPTION_HEAD_DATABASEEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class DatabaseException : public exception {
public:
	DatabaseException();
	DatabaseException(string message);
	virtual ~DatabaseException();
	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_DATABASEEXCEPTION_H_ */
