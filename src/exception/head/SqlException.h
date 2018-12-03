/*
 * SqlException.h
 *
 *  Created on: Dec 3, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_SQLEXCEPTION_H_
#define EXCEPTION_HEAD_SQLEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class SqlException {
public:
	SqlException();
	SqlException(string messsage);
	virtual ~SqlException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_SQLEXCEPTION_H_ */
