/*
 * BasicType.cpp
 *
 *  Created on: Nov 3, 2018
 *      Author: weizy
 */

#include "head/BasicType.h"

#include <stdlib.h>
#include <iostream>
using namespace std;

Integer::Integer(int data){
	this->data = data;
	convert = (char*)malloc(sizeof(int));
	char * d = (char*)&data;
	for (unsigned int i = 0; i < sizeof(int); i++){
		convert[i] = d[i];
	}
}
Integer::~Integer(){
	free(convert);
}
char * Integer::getData(){
	return convert;
}
unsigned int Integer::getDataLength(){
	return sizeof(data);
}

///------------------------------------------
Float::Float(float adata){
	this->data = adata;
	convert = (char*)malloc(sizeof(float));
	char * d = (char*)&adata;
	for (unsigned int i = 0; i < sizeof(float); i++){
		convert[i] = d[i];
	}
}
Float::~Float(){
	free(convert);
}
char * Float::getData(){
	return convert;
}
unsigned int Float::getDataLength(){
	return sizeof(data);
}

///----------------------------------------------
Double::Double(double adata){
	this->data = adata;
	convert = (char*)malloc(sizeof(double));
	char * d = (char*)&adata;
	for (unsigned int i = 0; i < sizeof(double); i++){
		convert[i] = d[i];
	}
}
Double::~Double(){
	free(convert);
}
char * Double::getData(){
	return convert;
}
unsigned int Double::getDataLength(){
	return sizeof(data);
}

///---------------------------------------------------
Char::Char(const char *adata, unsigned int dataLength){
	this->dataLength = dataLength;		//该类型的数据长度应该是创建的时候定义好的长度

	data = (char*)malloc(dataLength+1);
	for (unsigned int i = 0; i < dataLength; i++){
		data[i] = adata[i];
	}
	data[dataLength] = '\0';
}
Char::~Char(){
	free(data);
}
char * Char::getData(){
	return data;
}
unsigned int Char::getDataLength(){
	return this->dataLength;
}

///-----------------------------------------------
Varchar::Varchar(char * adata, unsigned int dataLength){
	this->dataLength = dataLength;
	data = (char*)malloc(dataLength+1);
	for (unsigned int i = 0; i < dataLength; i++){
		data[i] = adata[i];
	}
	data[dataLength] = '\0';
}
Varchar::~Varchar(){
	free(data);
}
char * Varchar::getData(){
	return data;
}
unsigned int Varchar::getDataLength(){
	return dataLength;
}








