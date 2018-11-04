/*
 * Tuple.h
 *
 *  Created on: Nov 2, 2018
 *      Author: weizy
 */

#ifndef HEAD_TUPLE_H_
#define HEAD_TUPLE_H_

#include <vector>

#include "Dictionary.h"

using namespace std;

class TupPosition;

class Tuple {
public:
	Tuple();
	Tuple(Relation * relationDic);
	virtual ~Tuple();

private:
	int rowId;
	int tupMaxLength;			//该元组占用的字节数，包括保存位置信息的数据以及rowId
	char * tupleDate;
	vector<TupPosition> tupPosition;
	Relation * relationDic;
public:
	void addInteger(int data);
	void addFload(float data);
	void addDouble(double data);
	void addChar(char * data, int dataSize);
	void addVarchar(char * data, int dataSize);

	void processData();
	char * getResult();
};

class TupPosition{
private:
	unsigned short attrStart;
	unsigned short attrLength;
};

#endif /* HEAD_TUPLE_H_ */
