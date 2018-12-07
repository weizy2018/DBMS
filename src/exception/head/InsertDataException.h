/*
 * InsertDataException.h
 *
 *  Created on: Dec 7, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_INSERTDATAEXCEPTION_H_
#define EXCEPTION_HEAD_INSERTDATAEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class InsertDataException :public exception {
public:
	InsertDataException();
	InsertDataException(string message);
	virtual ~InsertDataException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_INSERTDATAEXCEPTION_H_ */
