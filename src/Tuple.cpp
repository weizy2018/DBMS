/*
 * Tuple.cpp
 *
 *  Created on: Nov 2, 2018
 *      Author: weizy
 */
#include <ctime>
#include <stdlib.h>
#include <iostream>

using namespace std;

#include "head/Tuple.h"
#include "head/Global.h"


Tuple::Tuple(Relation * rel) {
	rowId = generateRowId();
	relationDic = rel;
}

Tuple::~Tuple() {
	free(rowId);
	free(tupleData);
	while (!basicData.empty()){
		BasicType * b = basicData.back();
		basicData.pop_back();
		delete b;
	}
	while (!tupPosition.empty()){
		TupPosition * t = tupPosition.back();
		tupPosition.pop_back();
		delete t;
	}
}

//rowId由时间和一个随机数组合而成
char * Tuple::generateRowId(){
	char * id = (char*)malloc(sizeof(time_t) + sizeof(int));
	time_t t = time(0);
	srand(0);
	int ran = rand();

	int index = 0;
	char * p = (char*)&t;
	for (unsigned int i = 0; i < sizeof(t); i++,index++){
		id[index] = p[i];
	}
	p = (char*)&ran;
	for (unsigned int i = 0; i < sizeof(ran); i++,index++){
		id[index] = p[i];
	}
	return id;
}
//计算元组的长度
void Tuple::calTupleLen() {
	unsigned int len = 0;
	unsigned fixed = 0;
	//统计边长类型的个数
	int varCount = 0;
	int totalProperty = relationDic->getTotalProperty();
	for (int i = 0; i < totalProperty; i++) {
		if (relationDic->getTypeName(i) == Global::VARCHAR) {
			varCount++;
		}
	}
	int rowIdLength = sizeof(time_t) + sizeof(int); //用于存放RowId的长度
	len += rowIdLength;
	fixed += rowIdLength;

	int positionLength = varCount*sizeof(TupPosition);
	len += positionLength;		//记录变长记录所用到的长度
	fixed += positionLength;

	//所有数据的长度
	for (unsigned int i = 0; i < basicData.size(); i++){
		BasicType * b = basicData.at(i);
		len += b->getDataLength();
		if (relationDic->getTypeName(i) != Global::VARCHAR){
			fixed += b->getDataLength();
		}
	}
	this->tupTotalLength = len;
	this->tupFixedLength = fixed;
	cout << "tupTotalLength = " << len;
	cout << "tupFixedLength = " << fixed;
}

void Tuple::addInteger(int data){
	BasicType * integer = new Integer(data);
	basicData.push_back(integer);
}
void Tuple::addFload(float data){
	BasicType * f = new Float(data);
	basicData.push_back(f);
}
void Tuple::addDouble(double data){
	BasicType * d = new Double(data);
	basicData.push_back(d);
}
void Tuple::addChar(char * data, int dataSize){
	BasicType * c = new Char(data, dataSize);
	basicData.push_back(c);
}
void Tuple::addVarchar(char * data, int dataSize){
	BasicType * v = new Varchar(data, dataSize);
	basicData.push_back(v);
}
void Tuple::processData(){
	this->calTupleLen();
	tupleData = (char*)malloc(tupTotalLength);

	int index = 0;
	int varIndex = tupFixedLength;					//变长记录开始存放的位置

	int rowIdLength = sizeof(time_t) + sizeof(int); //用于存放RowId的长度
	for (int i = 0; i < rowIdLength; i++){
		tupleData[index] = rowId[i];
	}
	for (unsigned int i = 0; i < basicData.size(); i++) {
		BasicType * b = basicData.at(i);
		if (relationDic->getTypeName(i) == Global::VARCHAR){	//存入变长记录
			unsigned int start = index;
			unsigned int len = b->getDataLength();
			char * d = b->getData();
			//这个暂时好像还没什么用， 先存起来吧
			TupPosition * pos = new TupPosition(start, len);
			tupPosition.push_back(pos);
			//写入位置
			char * s = (char*)&start;
			for (unsigned int j = 0; j < sizeof(start); j++){
				tupleData[index++] = s[j];
			}
			char *l = (char*)&len;
			for (unsigned int j = 0; j < sizeof(len); j++){
				tupleData[index++] = l[j];
			}
			//写入数据
			for (unsigned int j = 0; j < len; j++) {
				tupleData[varIndex++] = d[j];
			}
		} else {
			char * d = b->getData();
			unsigned int len = b->getDataLength();
			for (unsigned int j = 0; j < len; j++) {
				tupleData[index++] = d[j];
			}
		}
	}
}
char * Tuple::getResult() {
	return tupleData;
}

//-------------------------------
//TupPosition
//-------------------------------
TupPosition::TupPosition (unsigned short start, unsigned short len){
	attrStart = start;
	attrLength = len;
}
void TupPosition::setAttrStart(unsigned short start){
	attrStart = start;
}
unsigned short TupPosition::getAttrStart(){
	return attrStart;
}

void TupPosition::setAttrLength(unsigned short len){
	attrLength = len;
}
unsigned short TupPosition::getAttrLength(){
	return attrLength;
}














