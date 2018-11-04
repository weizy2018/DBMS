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
	totalRelation = 0;
}

void Dictionary::releaseDictionary(){
    delete dic;
    dic = nullptr;
}
Dictionary::~Dictionary(){
    for (int i=0; i < totalRelation; ++i){
        free(relationName[i]);
        free(relationFileName[i]);

    }
    while (!relations.empty()){
    	Relation * rel = relations.back();
    	delete rel;
    	relations.pop_back();
    }
}

void Dictionary::addRelationName(char * reName, int cnt){
    relationName[cnt] = reName;
}
char * Dictionary::getRelationName(int index){
    return relationName[index];
}

void Dictionary::addRelationFileName(char * reFileName, int cnt){
    relationFileName[cnt] = reFileName;
}
char * Dictionary::getRelationFileName(int index){
    return relationFileName[index];
}

void Dictionary::addRelation(Relation * rel){
    relations.push_back(rel);
}
Relation * Dictionary::getaRelation(int index){
    return relations.at(index);
}
void Dictionary::setTotalRelation(int totalRelation) {
    this->totalRelation = totalRelation;
}

int Dictionary::getTotalRelation() {
    return this->totalRelation;
}

void Dictionary::printDictionary(){
    printf("relationship name :\n");
    for (int i = 0; i < this->totalRelation; i++){
        printf("%s\t", this->relationName[i]);
    }
    printf("\n relation file name :\n");
    for (int i = 0; i < this->totalRelation; i++){
        printf("%s\t", this->relationFileName[i]);
    }
    printf("\n");
    for (Relation *rel : this->relations){
        rel->printRelation();
        printf("\n");
    }
    printf("\n");
    
}


Relation::Relation(int totalProperty){
    this->totalProperty = totalProperty;
}
void Relation::addType(int type, int value, int index){
    this->type[index][0] = type;
    this->type[index][1] = value;
}
int Relation::getTotalProperty(){
    return totalProperty;
}
int Relation::getTypeName(int index){
    return type[index][0];
}
int Relation::getTypeValue(int index){
    return type[index][1];
}
void Relation::printRelation(){
    for (int i = 0; i < totalProperty; i++){
        printf("%d\t%d\n", type[i][0], type[i][1]);
    }
}



