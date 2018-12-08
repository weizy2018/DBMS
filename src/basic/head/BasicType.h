/*
 * BasicType.h
 *
 *  Created on: Nov 3, 2018
 *      Author: weizy
 */

#ifndef BASIC_HEAD_BASICTYPE_H_
#define BASIC_HEAD_BASICTYPE_H_

class BasicType {
public:
	BasicType() {}
	virtual ~BasicType(){}
	virtual char * getData() = 0;
	virtual unsigned int getDataLength() = 0;
};

//----------------------------------------
class Integer : public BasicType {
public:
	Integer(int adata);
	virtual ~Integer();
	virtual char * getData();
	virtual unsigned int getDataLength();
private:
	int data;
	char * convert;
};

//------------------------------------------
class Float : public BasicType {
public:
	Float(float adata);
	virtual ~Float();
	virtual char * getData();
	virtual unsigned int getDataLength();
private:
	float data;
	char * convert;
};

//-----------------------------------------
class Double : public BasicType {
public:
	Double(double adata);
	virtual ~Double();
	virtual char * getData();
	virtual unsigned int getDataLength();
private:
	double data;
	char * convert;
};

//--------------------------------------------
class Char : public BasicType {
public:
	Char(const char * adata, unsigned int dataLength);
	virtual ~Char();
	virtual char * getData();
	virtual unsigned int getDataLength();
private:
	char * data;
	unsigned int dataLength;
};

//--------------------------------------------
class Varchar : public BasicType {
public:
	Varchar(const char * adata, unsigned int dataLength);
	virtual ~Varchar();
	virtual char * getData();
	virtual unsigned int getDataLength();
private:
	char * data;
	unsigned int dataLength;
};

#endif /* BASIC_HEAD_BASICTYPE_H_ */
