/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dictionary.cpp
 * Author: weizy
 * 
 * Created on November 1, 2018, 7:39 PM
 */

#include "head/Dictionary.h"
#include "exception/head/FileNotFoundException.h"
#include "head/Global.h"
#include "head/DBMS.h"
#include "basic/head/BasicType.h"

#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iostream>


using namespace std;

Dictionary * Dictionary::dic = nullptr;

Dictionary * Dictionary::getDictionary(){
    if (dic == nullptr){
        dic = new Dictionary;
    }
    return dic;
}

Dictionary::Dictionary(){
	curDatabaseName = "";
	blockSize = 4;
	change = false;
	headspace = (int)(blockSize*1024*0.2);
}

void Dictionary::releaseDictionary(){
    delete dic;
    dic = nullptr;
}
Dictionary::~Dictionary(){
	if (change) {
		writeBack();
	}
    while (!relations.empty()){
    	Relation * rel = relations.back();
    	delete rel;
    	relations.pop_back();
    }
    for (auto it = stringIndex.begin(); it != stringIndex.end(); it++) {
    	delete(it->second);
    }
    stringIndex.clear();

//    for (auto it = intIndex.begin(); it != intIndex.end(); it++) {
//    	delete(it->second);
//    }
//    intIndex.clear();
//
//    for (auto it = floatIndex.begin(); it != floatIndex.end(); it++) {
//    	delete(it->second);
//    }
//    floatIndex.clear();
//
//    for (auto it = doubleIndex.begin(); it != doubleIndex.end(); it++) {
//    	delete(it->second);
//    }
//    doubleIndex.clear();
}
void Dictionary::setCurDatabaseName(const char * curDBName) {
	curDatabaseName = curDBName;
}
const char * Dictionary::getCurDatabaseName() const {
	return curDatabaseName;
}

//设置块的大小
void Dictionary::setBlockSize(int size) {
	this->blockSize = size;
}
int Dictionary::getBlockSize() {
	return blockSize;
}

//设置预留空间大小 = blockSize*0.2
void Dictionary::setHeadspace(int headspace) {
	this->headspace = headspace;
}
int Dictionary::getHeadspace() {
	return headspace;
}
//字典更改标志
void Dictionary::setChange(bool change) {
	this->change = change;
}
bool Dictionary::getChange() {
	return change;
}


void Dictionary::addRelation(Relation * rel){
    relations.push_back(rel);
}
Relation * Dictionary::getRelation(int index){
    return relations.at(index);
}
Relation * Dictionary::getRelation(const char * relationName) {
	Relation * r = nullptr;
	for (vector<Relation *>::iterator it = relations.begin(); it != relations.end(); it++) {
		const char * rName = (*it)->getRelationName();
		if (strcmp(relationName, rName) == 0) {
			r =  *it;
			break;
		}
	}
	return r;
}
int Dictionary::getTotalRelation() {
	return relations.size();
}

void Dictionary::printDictionary(){
	for (vector<Relation *>::iterator it = relations.begin(); it != relations.end(); it++){
		Relation * rel = *it;
		rel->printRelation();
	}
    
}
void Dictionary::writeBack() {
	string dicPath("data/");
	dicPath.append(curDatabaseName);
	dicPath.append("/");
	dicPath.append(curDatabaseName);
	dicPath.append(".desc");

	FILE * dic;
	if ((dic = fopen(dicPath.c_str(), "w")) == NULL) {
		throw FileNotFoundException();
	}
	fprintf(dic, "%ld\n", relations.size());
	fprintf(dic, "%ld\n", indexs.size());
	fprintf(dic, "%d\n\n", blockSize);

	for (auto it = relations.begin(); it != relations.end(); it++) {
		fprintf(dic, "%u\n", (*it)->getTotalBlock());
		fprintf(dic, "%d\n", (*it)->getTotalProperty());
		fprintf(dic, "%s\n", (*it)->getRelationName());
		fprintf(dic, "%s\n", (*it)->getRelationFileName());
		int total = (*it)->getTotalProperty();
		//属性名称
		for (int i = 0; i < total; i++) {
			fprintf(dic, "%s ", (*it)->getAttribute(i).c_str());
		}
		fprintf(dic, "\n");
		//属性类型以及取值范围
		for (int i = 0; i < total; i++) {
			char attrName[10];
			switch((*it)->getTypeName(i)) {
			case Global::INTEGER:
				strcpy(attrName, "int");
				break;
			case Global::FLOAT:
				strcpy(attrName, "float");
				break;
			case Global::DOUBLE:
				strcpy(attrName, "double");
				break;
			case Global::CHAR:
				strcpy(attrName, "char");
				break;
			case Global::VARCHAR:
				strcpy(attrName, "varchar");
				break;
			}
			fprintf(dic, "%s %d\n", attrName, (*it)->getTypeValue(i));
		}
		fprintf(dic, "\n");
	}
	fprintf(dic, "\n");

	for (auto it = indexs.begin(); it != indexs.end(); it++) {
		string key = it->first;
		string value = it->second;
		unsigned int index = key.find('$');
		string tableName = key.substr(0, index);
		string colName   = key.substr(index + 1);
		fprintf(dic, "%s %s %s\n", tableName.c_str(), colName.c_str(), value.c_str());
	}

	fclose(dic);
}

void Dictionary::addStringIndex(string key, BPlusTree<string, unsigned long int> * value) {
	stringIndex.insert(pair<string, BPlusTree<string, unsigned long int> *>(key, value));
}
void Dictionary::addIntIndex(string key, Bplustree<int, unsigned long int> * value) {
	intIndex.insert(pair<string, Bplustree<int, unsigned long int> *>(key, value));
}
void Dictionary::addFloatIndex(string key, Bplustree<float, unsigned long int> * value) {
	floatIndex.insert(pair<string, Bplustree<float, unsigned long int> *>(key, value));
}
void Dictionary::addDoubleIndex(string key, Bplustree<double, unsigned long int> * value) {
	doubleIndex.insert(pair<string, Bplustree<double, unsigned long int> *>(key, value));
}

bool Dictionary::addIndex(string key, string indexName) {
	pair<map<string, string>::iterator, bool> ret;
	ret = indexs.insert(pair<string, string>(key, indexName));
	return ret.second;
}

BPlusTree<string, unsigned long int> * Dictionary::getStringIndex(const string tableName, const string colName) {
	BPlusTree<string, unsigned long int> * tree = nullptr;
	string key(tableName);
	key.append("$");
	key.append(colName);
	for (auto it = stringIndex.begin(); it != stringIndex.end(); it++) {
		if (it->first == key) {
			tree = it->second;
			break;
		}
	}
	return tree;
}

Bplustree<int, unsigned long int> * Dictionary::getIntIndex(const string tableName, const string colName) {
	Bplustree<int, unsigned long int> * tree = nullptr;
	string key(tableName);
	key.append("$");
	key.append(colName);
	for (auto it = intIndex.begin(); it != intIndex.end(); it++) {
		if (it->first == key) {
			tree = it->second;
			break;
		}
	}
	return tree;
}
Bplustree<float, unsigned long int> * Dictionary::getFloatIndex(const string tableName, const string colName) {
	Bplustree<float, unsigned long int> * tree = nullptr;
	string key(tableName);
	key.append("$");
	key.append(colName);
	for (auto it = floatIndex.begin(); it != floatIndex.end(); it++) {
		if (it->first == key) {
			tree = it->second;
			break;
		}
	}
	return tree;
}
Bplustree<double, unsigned long int> * Dictionary::getDoubleIndex(const string tableName, const string colName) {
	Bplustree<double, unsigned long int> * tree = nullptr;
	string key(tableName);
	key.append("$");
	key.append(colName);
	for (auto it = doubleIndex.begin(); it != doubleIndex.end(); it++) {
		if (it->first == key) {
			tree = it->second;
			break;
		}
	}
	return tree;
}


//==============================================

Relation::Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName){
    this->totalBlock = totalBlock;
	this->totalProperty = totalProperty;
    this->relationName = relName;
    this->relationFileName = relFileName;
}
Relation::~Relation(){
	free(relationName);
	free(relationFileName);
}
void Relation::addType(int typeName, int value){
	type.push_back(pair<int, int>(typeName, value));
}
int Relation::getTotalProperty() const{
    return totalProperty;
}
int Relation::getTypeName(int index) const{
    return type.at(index).first;
}
int Relation::getTypeValue(int index) const{
	return type.at(index).second;
}

void Relation::printRelation(){
	printf("relation name : %s\n", relationName);
	printf("relation file name : %s\n", relationFileName);
	printf("total block : %u\n", totalBlock);
	for (unsigned int i = 0; i < attribute.size(); i++) {
		cout << attribute[i] << " " ;
	}

	cout << endl;
    for (int i = 0; i < totalProperty; i++){
        printf("%d\t%d\n", type.at(i).first, type.at(i).second);
    }
}
void Relation::printRelationData() {
	for (unsigned int i = 0; i < totalBlock; i++) {
		//Block * DBMS::getBlock(const string relationName, unsigned int blockId)
		Block * block = DBMS::getDBMSInst()->getBlock(relationName, i);
		if (block == nullptr) {
			block = this->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			//void putBlock(string relationName, unsigned int blockId, Block * value);
			DBMS::getDBMSInst()->putBlock(relationName, i, block);
		}
		block->printBlock();
	}
}
/*
 * attrIndex: 元组中的第几个元素, 这里对应的该元素一定是int类型的
 */
void Relation::initIntBplustree(unsigned int attrIndex, Bplustree<int, unsigned long int> * tree) {
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(relationName, i);
		if (block == nullptr) {
			block = this->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(relationName, i, block);
		}
		unsigned int blockId = block->getBlockId();
		vector<Tuple *> tuples = block->getBlockTupls();
		while (!tuples.empty()) {
			Tuple * tup = tuples.back();
			tuples.pop_back();
			BasicType * basic = tup->getTupleBasicType(attrIndex);
			int * data = (int*)basic->getData();
			tree->put(*data, blockId);
			delete tup;
		}

	}
}
/**
 * attrIndex: 元组对应的第几个元素，这里对应的元素一定是float类型的
 */
void Relation::initFloatBplustree(unsigned int attrIndex, Bplustree<float, unsigned long int>  * tree) {
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(relationName, i);
		if (block == nullptr) {
			block = this->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(relationName, i, block);
		}
		unsigned int blockId = block->getBlockId();
		vector<Tuple *> tuples = block->getBlockTupls();
		while (!tuples.empty()) {
			Tuple * tup = tuples.back();
			tuples.pop_back();
			BasicType * basic = tup->getTupleBasicType(attrIndex);
			float * data = (float*)basic->getData();
			tree->put(*data, blockId);
		}
	}
}
/**
 * attrIndex: 元组对应的第几个元素，这里对应的元素一定是double类型的
 */
void Relation::initDoubleBplustree(unsigned int attrIndex, Bplustree<double, unsigned long int> * tree) {
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(relationName, i);
		if (block == nullptr) {
			block = this->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(relationName, i, block);
		}
		unsigned int blockId = block->getBlockId();
		vector<Tuple *> tuples = block->getBlockTupls();
		while (!tuples.empty()) {
			Tuple * tup = tuples.back();
			tuples.pop_back();
			BasicType * basic = tup->getTupleBasicType(attrIndex);
			double * data = (double*)basic->getData();
			tree->put(*data, blockId);
		}
	}
}
/**
 * attrIndex: 元组对应的第几个元素，这里对应的元素一定是char *类型的
 */
void Relation::initStringBplustree(unsigned int attrIndex, BPlusTree<string, unsigned long int> * tree) {
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(relationName, i);
		if (block == nullptr) {
			block = this->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(relationName, i, block);
		}
		unsigned int blockId = block->getBlockId();
		vector<Tuple *> tuples = block->getBlockTupls();
		while (!tuples.empty()) {
			Tuple * tup = tuples.back();
			tuples.pop_back();
			BasicType * basic = tup->getTupleBasicType(attrIndex);
			char * data = basic->getData();
			string key(data);
			tree->put(key, blockId);
		}
	}
}


void Relation::setRelationName(char * relName){
	relationName = relName;
}
char * Relation::getRelationName() const{
	return relationName;
}
void Relation::setRelationFileName(char * relFileName){
	relationFileName = relFileName;
}
char * Relation::getRelationFileName() const{
	return relationFileName;
}

unsigned int Relation::getTotalBlock() {
	return totalBlock;
}
void Relation::setTotalBlock(unsigned int totalBlock) {
	this->totalBlock = totalBlock;
}
void Relation::addAttribute(const char * attrName) {
	string str(attrName);
	attribute.push_back(str);
}
string Relation::getAttribute(int index) {
	return attribute.at(index);
}

bool Relation::hasAttribute(const string attr) {
	for (unsigned int i = 0; i < attribute.size(); i++) {
		if (attr == attribute[i]) {
			return true;
		}
	}
	return false;
}
unsigned int Relation::getAttributeIndex(const char * attr) {
	string temp(attr);
	for (unsigned int i = 0; i < attribute.size(); i++) {
		if (temp == attribute.at(i)){
			return i;
		}
	}
	return -1;
}
//获取关系表中对应的块
Block * Relation::getBlock(const string databaseName, unsigned int blockId) {
	FILE * relFile;
	string relFileName("data/");
	relFileName.append(databaseName);
	relFileName.append("/");
	relFileName.append(relationFileName);
	if ((relFile = fopen(relFileName.c_str(), "rb")) == NULL) {
		throw FileNotFoundException(relFileName);
	}
	int blockSize = Dictionary::getDictionary()->getBlockSize();
	fseek(relFile, blockSize*1024*blockId, SEEK_SET);
	char * blockData = (char*)malloc(blockSize*1024);
	fread(blockData, blockSize*1024, 1, relFile);
	//Block(char * block, const Relation * rel);
	Block * block = new Block(blockData, this);
	//这里不用释放blockData
	return block;
}



