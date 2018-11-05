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
#include <stdlib.h>
#include <vector>
#include <stdio.h>

Dictionary * Dictionary::dic = nullptr;

Dictionary * Dictionary::getDictionary(){
    if (dic == nullptr){
        dic = new Dictionary;
    }
    return dic;
}

Dictionary::Dictionary(){

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
}



void Dictionary::addRelation(Relation * rel){
    relations.push_back(rel);
}
Relation * Dictionary::getaRelation(int index){
    return relations.at(index);
}


void Dictionary::printDictionary(){
	for (vector<Relation *>::iterator it = relations.begin(); it != relations.end(); it++){
		Relation * rel = *it;
		rel->printRelation();
	}
    
}


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



