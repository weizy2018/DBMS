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

#include <vector>
#include <string>

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
    Relation * getRelation(int index);
    Relation * getRelation(char * relationname);
    void setCurDatabaseName(const char * curDBName);
	const char * getCurDatabaseName() const;
	void printDictionary(); //输出该字典到控制台上

	void writeBack();		//对字典更新后写回文件

private:
    vector<Relation*> relations;
    const char * curDatabaseName;

};

class Relation{

public:
	Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName);
	~Relation();
	void addType(int type, int value, int index);
	int getTotalProperty() const;
	int getTypeName(int index) const;
	int getTypeValue(int index) const;
	void addAttribute(char * attr);

	
	void setRelationName(char * relName);
	char * getRelationName() const;
	void setRelationFileName(char * relFileName);
	char * getRelationFileName() const;

	unsigned int getTotalBlock();
	void setTotalBlock(unsigned int totalBlock);

	void printRelation();

private:
	char * relationName;
	char * relationFileName;
	unsigned int totalBlock;
    int totalProperty;
    int type[MAX_PROPERTY][2];  //type[1][0] 属性的名称  type[1][1]: 属性的范围（对于char或varchar类型）
    vector<string> attribute;
};

#endif /* DICTIONARY_H */

