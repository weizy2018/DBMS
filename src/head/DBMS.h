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
#define LRU_SIZE				20		//定义lru能容纳的块数

class DBMS {
private:
	DBMS();
	DBMS(int memorySize);
	DBMS(const DBMS& orig);
	DBMS & operator = (const DBMS &);
private:
	static DBMS * dbms;

public:
	static DBMS * getDBMSInst();
	static DBMS * releaseDBMSInst();
public:
	virtual ~DBMS();

public:
	void loadDatabases();			//将/data/databases.db加载到内存（所有数据库的名称）
	void initialDictionary(const char * dicName);		//发布“use school”命令后调用，将对应的数据库字典调入内存
public:
	void test();
	void test2();
public:
	void createDatabase(char * dbName);
	void writeBack();				//将数据库名称写回文件databases
private:
	bool isExist(char * dbName);					//判断数据库是否存在
private:
	LruCache<string, Block *> * lru;
	vector<char *> databases;		//保存所有数据库的名称
};

#endif /* DBMS_H */

