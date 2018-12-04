/*
 * KeyNotFoundException.h
 *
 *  Created on: Dec 4, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_KEYNOTFOUNDEXCEPTION_H_
#define EXCEPTION_HEAD_KEYNOTFOUNDEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class KeyNotFoundException {
public:
	KeyNotFoundException();
	KeyNotFoundException(string message);
	virtual ~KeyNotFoundException();
public:
	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_KEYNOTFOUNDEXCEPTION_H_ */
