/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dictionary.h
 * Author: weizy
 *
 * Created on November 1, 2018, 7:39 PM
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include<vector>

using namespace std;

#define NAME_LENGTH         20
#define MAX_RELATIONSHIP    50
#define MAX_PROPERTY        30  //每个表最多包含的属性个数

class Relation;

class Dictionary {

public:
    static Dictionary * getDictionary();
    void releaseDictionary();
private:
    static Dictionary * dic;

private:
    Dictionary();
    Dictionary(const Dictionary &);
    Dictionary & operator = (const Dictionary &);

public:
    virtual ~Dictionary();

public:
    void addRelation(Relation * rel);
    Relation * getaRelation(int index);
	
	void printDictionary(); //输出该字典到控制台上

protected:

private:
    vector<Relation*> relations;

};

class Relation{

public:
	Relation(int totalProperty, char * relName, char * relFileName);
	~Relation();
	void addType(int type, int value, int index);
	int getTotalProperty();
	int getTypeName(int index);
	int getTypeValue(int index);
	
	void setRelationName(char * relName);
	char * getRelationName();
	void setRelationFileName(char * relFileName);
	char * getRelationFileName();

	void printRelation();

private:
	char * relationName;
	char * relationFileName;
    int totalProperty;
    int type[MAX_PROPERTY][2];  //type[1][0] 属性的名称  type[1][1]: 属性的范围（对于char或varchar类型）

};

#endif /* DICTIONARY_H */

