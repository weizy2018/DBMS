/*
 * IndexCreateException.h
 *
 *  Created on: Dec 11, 2018
 *      Author: weizy
 */

#ifndef EXCEPTION_HEAD_INDEXCREATEEXCEPTION_H_
#define EXCEPTION_HEAD_INDEXCREATEEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class IndexCreateException : public exception {
public:
	IndexCreateException();
	IndexCreateException(string message);
	virtual ~IndexCreateException();

	const char * what () const throw ();
private:
	string message;
};

#endif /* EXCEPTION_HEAD_INDEXCREATEEXCEPTION_H_ */
