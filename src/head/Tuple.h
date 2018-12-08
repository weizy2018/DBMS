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
#include "../basic/head/BasicType.h"

using namespace std;

class TupPosition;

class Tuple {
public:
	Tuple(const Relation * rel);
	Tuple(char * tupData, const Relation * rel);
//	Tuple(const char * tupData, int start, const Relation * rel);
	virtual ~Tuple();

private:
	char * rowId;							//该元组的id(暂时先随便生成一个不同的就得了）日期+随机数
	unsigned int tupTotalLength;			//该元组占用的字节数，包括保存位置信息的数据以及rowId
	unsigned int tupFixedLength;			//rowId + Positions + 定长记录
	char * tupleData;						//元组的数据（数组表示）
	vector<BasicType *> basicData;			//元组中不同类型的数据
	vector<TupPosition *> tupPosition;		//变长记录的表示方法
	const Relation * relationDic;			//该元组对应的关系表
public:
	void addInteger(int data);
	void addFload(float data);
	void addDouble(double data);
	void addChar(const char * data, int dataSize);
	void addVarchar(const char * data, int dataSize);

	void processData();						//将数据的基本类型转换成线性表
	char * getResult();						//返回转换结果
	unsigned int getTupLength();
	void printTuple();						//打印该元组
private:
	char * generateRowId();
	void calTupleLen();
private:
	void parsedTuple(int start);
	void prasedInteger(int & index);
	void prasedFload(int & index);
	void prasedDouble(int & index);
	void prasedChar(int & index, int len);
	void prasedVarchar(int & index);
};

class TupPosition{
private:
	unsigned short attrStart;
	unsigned short attrLength;
public:
	TupPosition (unsigned short start, unsigned short len);

	void setAttrStart(unsigned short start);
	unsigned short getAttrStart();

	void setAttrLength(unsigned short len);
	unsigned short getAttrLength();
};

#endif /* HEAD_TUPLE_H_ */
