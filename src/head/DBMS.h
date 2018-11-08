/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DBMS.h
 * Author: weizy
 *
 * Created on November 1, 2018, 7:35 PM
 */

#ifndef DBMS_H
#define DBMS_H

#include "Dictionary.h"
#include "../tools/head/lru.h"
#include "Block.h"
#include "../exception/head/FileNotFoundException.h"
#include <string>

#define MAX_DATABASE_NAME 		25		//数据库名称的长度
#define MAX_ATTRIBUTE_NAME 		25		//关系表属性名称的长度

class DBMS {
public:
	DBMS(int memorySize);
	DBMS(const DBMS& orig);
	virtual ~DBMS();

public:
	void loadDatabases();			//将/data/databases.db加载到内存（所有数据库的名称）
	void initialDictionary(const char * dicName);		//发布“use school”命令后调用，将对应的数据库字典调入内存
	void test();
	void test2();
private:

private:
	LruCache<string, Block *> * lru;
	vector<char *> databases;		//保存所有数据库的名称
};

#endif /* DBMS_H */

