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
#include <map>
#include <string>
#include "../tools/head/BPlusTree.h"
#include "../tools/head/Bplustree.h"
#include "Block.h"

using namespace std;

#define NAME_LENGTH         20
#define MAX_RELATIONSHIP    50
#define MAX_PROPERTY        30  //每个表最多包含的属性个数

class Relation;
class Block;

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
    Relation * getRelation(const char * relationname);
    void setCurDatabaseName(const char * curDBName);
	const char * getCurDatabaseName() const;

	void setBlockSize(int size);
	int getBlockSize();
	void setHeadspace(int headspace);
	int getHeadspace();

	void setChange(bool change);
	bool getChange();
public:
	void printDictionary(); //输出该字典到控制台上

	void writeBack();		//对字典更新后写回文件
public:
	void addStringIndex(string key, BPlusTree<string, unsigned long int> * value);

	void addIntIndex(string key, Bplustree<int, unsigned long int> * value);
	void addFloatIndex(string key, Bplustree<float, unsigned long int> * value);
	void addDoubleIndex(string key, Bplustree<double, unsigned long int> * value);

	void addIndex(string key, string indexName);
public:
	BPlusTree<string, unsigned long int> * getStringIndex(const string tableName, const string colName);

	Bplustree<int, unsigned long int> * getIntIndex(const string tableName, const string colName);
	Bplustree<float, unsigned long int> * getFloatIndex(const string tableName, const string colName);
	Bplustree<double, unsigned long int> * getDoubleIndex(const string tableName, const string colName);

private:
    vector<Relation*> relations;
    const char * curDatabaseName;

    //索引  string = tableName + "$" + colName
    map<string, BPlusTree<string, unsigned long int> *> stringIndex;

    map<string, Bplustree<int, unsigned long int> *> intIndex;
    map<string, Bplustree<float, unsigned long int> *> floatIndex;
    map<string, Bplustree<double, unsigned long int> *> doubleIndex;

    //index <tableName + "$" + colName, indexName>
    map<string, string> indexs;
private:
    int blockSize;			//块的大小
    int headspace;			//每个块的预留空间 = blockSize*1024*0.2
private:
    bool change;			//字典更改标志
};

class Relation{

public:
	Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName);
	~Relation();
	void addType(int typeName, int value);
	int getTotalProperty() const;
	int getTypeName(int index) const;
	int getTypeValue(int index) const;
	void addAttribute(const char * attr);
	string getAttribute(int index);
	unsigned int getAttributeIndex(char * attr);

	
	void setRelationName(char * relName);
	char * getRelationName() const;
	void setRelationFileName(char * relFileName);
	char * getRelationFileName() const;

	unsigned int getTotalBlock();
	void setTotalBlock(unsigned int totalBlock);
public:
	void printRelationData();
	void printRelation();
public:
	Block * getBlock(const string databaseName, unsigned int blockId);

private:
	char * relationName;
	char * relationFileName;
	unsigned int totalBlock;
    int totalProperty;
    //int type[MAX_PROPERTY][2];  //type[1][0] 属性的名称  type[1][1]: 属性的范围（对于char或varchar类型）
    vector<pair<int, int>> type;
    vector<string> attribute;

    //<colName, fileName>
//    map<string, string> indexFileName;
};

#endif /* DICTIONARY_H */

