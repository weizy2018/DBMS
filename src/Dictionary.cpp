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
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "head/Global.h"
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
	curDatabaseName = nullptr;
}

void Dictionary::releaseDictionary(){
    delete dic;
    dic = nullptr;
}
Dictionary::~Dictionary(){

    while (!relations.empty()){
    	Relation * rel = relations.back();
    	delete rel;
    	relations.pop_back();
    }
    for (auto it = stringIndex.begin(); it != stringIndex.end(); it++) {
    	delete(it->second);
    }
    stringIndex.clear();

    for (auto it = intIndex.begin(); it != intIndex.end(); it++) {
    	delete(it->second);
    }
    intIndex.clear();

    for (auto it = floatIndex.begin(); it != floatIndex.end(); it++) {
    	delete(it->second);
    }
    floatIndex.clear();

    for (auto it = doubleIndex.begin(); it != doubleIndex.end(); it++) {
    	delete(it->second);
    }
    doubleIndex.clear();
}
void Dictionary::setCurDatabaseName(const char * curDBName) {
	curDatabaseName = curDBName;
}
const char * Dictionary::getCurDatabaseName() const {
	return curDatabaseName;
}

void Dictionary::setBlockSize(int size) {
	this->BlockSize = size;
}
int Dictionary::getBlockSize() {
	return BlockSize;
}


void Dictionary::addRelation(Relation * rel){
    relations.push_back(rel);
}
Relation * Dictionary::getRelation(int index){
    return relations.at(index);
}
Relation * Dictionary::getRelation(char * relationName) {
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


void Dictionary::printDictionary(){
	for (vector<Relation *>::iterator it = relations.begin(); it != relations.end(); it++){
		Relation * rel = *it;
		rel->printRelation();
	}
    
}
void Dictionary::writeBack() {
	FILE * dic;
	if ((dic = fopen("data/dictionary2.dic", "w")) == NULL) {
		throw FileNotFoundException();
	}
	fprintf(dic, "%ld\n", relations.size());
	fprintf(dic, "%ld\n\n", indexs.size());

	for (auto it = relations.begin(); it != relations.end(); it++) {
		fprintf(dic, "%u\n", (*it)->getTotalBlock());
		fprintf(dic, "%d\n", (*it)->getTotalProperty());
		fprintf(dic, "%s\n", (*it)->getRelationName());
		fprintf(dic, "%s\n", (*it)->getRelationFileName());
		int total = (*it)->getTotalProperty();
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
void Dictionary::addIntIndex(string key, BPlusTree<int, unsigned long int> * value) {
	intIndex.insert(pair<string, BPlusTree<int, unsigned long int> *>(key, value));
}
void Dictionary::addFloatIndex(string key, BPlusTree<float, unsigned long int> * value) {
	floatIndex.insert(pair<string, BPlusTree<float, unsigned long int> *>(key, value));
}
void Dictionary::addDoubleIndex(string key, BPlusTree<double, unsigned long int> * value) {
	doubleIndex.insert(pair<string, BPlusTree<double, unsigned long int> *>(key, value));
}
void Dictionary::addIndex(string key, string indexName) {
	indexs.insert(pair<string, string>(key, indexName));
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
void Relation::addType(int type, int value, int index){
    this->type[index][0] = type;
    this->type[index][1] = value;
}
int Relation::getTotalProperty() const{
    return totalProperty;
}
int Relation::getTypeName(int index) const{
    return type[index][0];
}
int Relation::getTypeValue(int index) const{
    return type[index][1];
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
        printf("%d\t%d\n", type[i][0], type[i][1]);
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
void Relation::addAttribute(char * attrName) {
	string str(attrName);
	attribute.push_back(str);
}
unsigned int Relation::getAttributeIndex(char * attr) {
	string temp(attr);
	for (unsigned int i = 0; i < attribute.size(); i++) {
		if (temp == attribute.at(i)){
			return i;
		}
	}
	return -1;
}



