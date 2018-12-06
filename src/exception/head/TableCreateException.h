/*
 * TableCreateException.h
 *
 *  Created on: Dec 6, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_TABLECREATEEXCEPTION_H_
#define EXCEPTION_HEAD_TABLECREATEEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class TableCreateException : public exception {
public:
	TableCreateException();
	TableCreateException(string message);
	virtual ~TableCreateException();
	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_TABLECREATEEXCEPTION_H_ */
