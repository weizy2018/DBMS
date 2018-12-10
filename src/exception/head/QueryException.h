/*
 * QueryException.h
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_QUERYEXCEPTION_H_
#define EXCEPTION_HEAD_QUERYEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class QueryException :public exception {
public:
	QueryException();
	QueryException(string message);
	virtual ~QueryException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_QUERYEXCEPTION_H_ */
