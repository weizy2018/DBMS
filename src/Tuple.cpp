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
#include "tools/head/tools.h"

Tuple::Tuple(const Relation * rel):relationDic(rel) {
	tupleData = nullptr;
	tupTotalLength = 0;
	tupFixedLength = 0;
	rowId = Tools::getToolsInst()->generateRowId();
}
Tuple::Tuple(char * tupData, const Relation * rel):relationDic(rel){
	this->tupleData = tupData;
	parsedTuple(0);
}
//Tuple::Tuple(const char * tupData, int start, const Relation * rel) {
//	this->tupleData = tupData;
//	this->relationDic = rel;
//	parsedTuple(start);
//}

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

	cout << "time_t : " << t << "  ran : " << ran << endl;

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
	const int totalProperty = relationDic->getTotalProperty();
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
//	cout << "tupTotalLength = " << len;
//	cout << "  tupFixedLength = " << fixed << endl;
}
void Tuple::parsedTuple(int start) {
	int index = start;
	//rowId
	int rowIdLength = sizeof(time_t) + sizeof(int);
	rowId = (char*)malloc(rowIdLength);
	for (int i = 0; i < rowIdLength; i++){
		rowId[i] = tupleData[index++];
	}
	for (int i = 0; i < relationDic->getTotalProperty(); i++){
		switch(relationDic->getTypeName(i)){
		case Global::INTEGER:
			prasedInteger(index);
			break;
		case Global::FLOAT:
			prasedFload(index);
			break;
		case Global::DOUBLE:
			prasedDouble(index);
			break;
		case Global::CHAR:
			prasedChar(index, relationDic->getTypeValue(i));
			break;
		case Global::VARCHAR:
			prasedVarchar(index);
			break;
		}
	}
}
void Tuple::prasedInteger(int & index){
	int data;
	char * d = (char*)&data;
	for (unsigned int i = 0; i < sizeof(data); i++){
		d[i] = tupleData[index++];
	}
	BasicType * in = new Integer(data);
	basicData.push_back(in);
}
void Tuple::prasedFload(int & index){
	float data;
	char * f = (char*)&data;
	for (unsigned int i = 0; i < sizeof(data); i++){
		f[i] = tupleData[index++];
	}
	BasicType * fl = new Float(data);
	basicData.push_back(fl);
}
void Tuple::prasedDouble(int & index){
	double data;
	char * d = (char*)&data;
	for (unsigned int i = 0; i < sizeof(data); i++){
		d[i] = tupleData[index++];
	}
	BasicType * db = new Double(data);
	basicData.push_back(db);
}
void Tuple::prasedChar(int & index, int len){
	char * data = (char*)malloc(len);
	for (int i = 0; i < len; i++){
		data[i] = tupleData[index++];
	}
	BasicType * ch = new Char(data, len);
	basicData.push_back(ch);
}
void Tuple::prasedVarchar(int & index){
	unsigned short start;
	unsigned short len;
	char * s = (char*)&start;
	for (unsigned int i = 0; i < sizeof(start); i++){
		s[i] = tupleData[index++];
	}
	char * le = (char*)&len;
	for (unsigned int i = 0; i < sizeof(len); i++){
		le[i] = tupleData[index++];
	}
	TupPosition * pos = new TupPosition(start, len);
	tupPosition.push_back(pos);
	char * data = (char*)malloc(len);
	for (unsigned int i = 0; i < len; i++){
		data[i] = tupleData[start++];
	}
	Varchar * vc = new Varchar(data, len);
	basicData.push_back(vc);
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
		tupleData[index++] = rowId[i];
	}
	for (unsigned int i = 0; i < basicData.size(); i++) {
		BasicType * b = basicData.at(i);
		if (relationDic->getTypeName(i) == Global::VARCHAR){	//存入变长记录
			unsigned short start = varIndex;
			unsigned short len = b->getDataLength();
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
unsigned int Tuple::getTupLength() {
	return tupTotalLength;
}
void Tuple::printTuple() {
	time_t ti;
	int index = 0;
	char * p = (char*)&ti;
	for (unsigned int i = 0; i < sizeof(ti); i++) {
		p[i] = tupleData[index++];
	}
	int ran;
	p = (char*)&ran;
	for (unsigned int i = 0; i < sizeof(ran); i++) {
		p[i] = tupleData[index++];
	}
	printf("rowId : %ld %d\n", ti, ran);
	for (unsigned int i = 0; i < basicData.size(); i++) {
		switch(relationDic->getTypeName(i)) {
		case Global::INTEGER:{
			int * data1 = (int*)basicData.at(i)->getData();
			printf("  %d", *data1);
			break;
		}
		case Global::FLOAT:{
			float * data2 = (float*)basicData.at(i)->getData();
			printf("  %f", *data2);
			break;
		}
		case Global::DOUBLE:{
			double * data3 = (double*)basicData.at(i)->getData();
			printf("  %f", *data3);
			break;
		}
		case Global::CHAR:
			printf("  %s", basicData.at(i)->getData());
			break;
		case Global::VARCHAR:
			printf("  %s", basicData.at(i)->getData());
			break;
		}
	}
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














