/*
 * Error.h
 *
 *  Created on: Feb 18, 2019
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_ERROR_H_
#define EXCEPTION_HEAD_ERROR_H_

#include <exception>
#include <string>

using namespace std;

class Error : public exception {
public:
	Error();
	Error(string message);
	virtual ~Error();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_ERROR_H_ */
