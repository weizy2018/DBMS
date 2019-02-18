/*
 * SqlSyntaxException.h
 *
 *  Created on: Dec 4, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_SQLSYNTAXEXCEPTION_H_
#define EXCEPTION_HEAD_SQLSYNTAXEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class SqlSyntaxException : public exception {
public:
	SqlSyntaxException();
	SqlSyntaxException(string messsage);
	virtual ~SqlSyntaxException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_SQLSYNTAXEXCEPTION_H_ */
