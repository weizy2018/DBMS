/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DBMS.cpp
 * Author: weizy
 * 
 * Created on November 1, 2018, 7:35 PM
 */


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "head/DBMS.h"
#include "head/Global.h"
#include "head/Tuple.h"
#include "tools/head/BPlusTree.h"

#include "exception/head/FileNotFoundException.h"
#include "exception/head/DatabaseCreateException.h"
#include "exception/head/DatabaseException.h"
#include "exception/head/TableCreateException.h"
#include "exception/head/InsertDataException.h"


using namespace std;

DBMS * DBMS::dbms = nullptr;

DBMS::DBMS() {
	currentDatabase = "";
	lru = new LruCache<string, Block *>(LRU_SIZE);
	loadDatabases();
}
DBMS::DBMS(int memorySize) {
	currentDatabase = "";
	int lru_size = memorySize/4;
	cout << "LruSize = " << lru_size << endl;
	lru = new LruCache<string, Block *>(lru_size);
	loadDatabases();
}
DBMS * DBMS::getDBMSInst() {
	if (dbms == nullptr) {
		dbms = new DBMS();
	}
	return dbms;
}


DBMS::~DBMS() {
	delete lru;

}
void DBMS::loadDatabases() {
	FILE * dbs;
	if ((dbs = fopen("data/databases.db", "r")) == NULL) {
		throw FileNotFoundException("data/databases.db");
	}
	char * p = (char*)malloc(Global::MAX_DATABASE_NAME);
	while (fscanf(dbs, "%s", p) != EOF) {
		databases.push_back(p);
		p = (char*)malloc(Global::MAX_DATABASE_NAME);
	}
	free(p);	//多申请了一个空间  删除以免内存泄露
	fclose(dbs);
}

void DBMS::initialDictionary(const char * dicName) {
	cout << "DBMS::inititalDictionary()" << endl;
	Dictionary::getDictionary()->setCurDatabaseName(dicName);
    FILE * dicFile;
    //eg: data/school/school.desc
    string dicDescName(dicName);	//school
    dicDescName = "data/" + dicDescName + "/" + dicDescName + ".desc";
    cout << "dicDescName : " << dicDescName << endl;

    if ((dicFile = fopen(dicDescName.c_str(), "r")) == NULL) {
        throw FileNotFoundException("FileNotFoundException: can't not open " + dicDescName);
    }
    int totalRelationship;
    int totalIndex;
    int blockSize;
    fscanf(dicFile, "%d", &totalRelationship);
    fscanf(dicFile, "%d", &totalIndex);
    fscanf(dicFile, "%d", &blockSize);

    Dictionary::getDictionary()->setBlockSize(blockSize);
    Dictionary::getDictionary()->setHeadspace((int)(blockSize*1024*0.2));
    
    for (int i = 0; i < totalRelationship; i++) {
    	unsigned int totalBlock;
        int totalProperty;
        char * relName = (char*)malloc(Global::MAX_RELATION_FILE_NAME);
        char * relFileName = (char*)malloc(Global::MAX_RELATION_FILE_NAME);

        fscanf(dicFile, "%u", &totalBlock);
        fscanf(dicFile, "%d", &totalProperty);
        fscanf(dicFile, "%s", relName);
        fscanf(dicFile, "%s", relFileName);

        Relation * rel = new Relation(totalBlock, totalProperty, relName, relFileName);
        for (int j = 0; j < totalProperty; j++) {
        	char  attrName[Global::MAX_ATTRIBUTE_NAME];
        	fscanf(dicFile, "%s", attrName);
        	rel->addAttribute(attrName);
        }
        for (int j = 0; j < totalProperty; j++) {
            char * type = (char*) malloc(Global::TYPE_LENGTH);
            int value;
            fscanf(dicFile, "%s", type);
            fscanf(dicFile, "%d", &value);
            int typeToInt;
            if (strcmp(type, "int")==0){
                typeToInt = Global::INTEGER;
            } else if (strcmp(type, "char") == 0){
                typeToInt = Global::CHAR;
            } else if (strcmp(type, "varchar") == 0){
                typeToInt = Global::VARCHAR;
            } else if (strcmp(type, "float") == 0){
                typeToInt = Global::FLOAT;
            } else if (strcmp(type, "double") == 0){
            	typeToInt = Global::DOUBLE;
            }
            
            rel->addType(typeToInt, value);
        }
        Dictionary::getDictionary()->addRelation(rel);
    }

    char tableName[Global::MAX_RELATION_NAME];
    char colName[Global::TYPE_LENGTH];
    char indexName[Global::MAX_IDNEX_NAME];
    for (int i = 0; i < totalIndex; i++) {
    	//表名  列名  索引名
    	fscanf(dicFile, "%s%s%s", tableName, colName, indexName);
    	string key(tableName);
    	key.append("$");
    	key.append(colName);

    	//向数据库字典中添加索引名称
    	Dictionary::getDictionary()->addIndex(key, indexName);

    	Relation * rel = Dictionary::getDictionary()->getRelation(tableName);
    	unsigned int attrIndex = rel->getAttributeIndex(colName);

    	//BPlusTree(const char * indexFileName, int keyLen, int valueLen, bool create);
    	string indexFileName(tableName);
    	indexFileName.append("_");
    	indexFileName.append(colName);
    	indexFileName.append(".ind");

    	int indexKeyLen = rel->getTypeValue(attrIndex);
    	int valueLen    = sizeof(unsigned long int);

    	//初始化相应的索引
    	if (rel->getTypeName(attrIndex) == Global::INTEGER) {
    		BPlusTree<int, unsigned long int>* tree;
    		tree = new BPlusTree<int, unsigned long int>(indexFileName.c_str(), indexKeyLen, valueLen, false);
    		Dictionary::getDictionary()->addIntIndex(key, tree);

    	} else if (rel->getTypeName(attrIndex) == Global::FLOAT) {
    		BPlusTree<float, unsigned long int> * tree;
    		tree = new BPlusTree<float, unsigned long int>(indexFileName.c_str(), indexKeyLen, valueLen, false);
    		Dictionary::getDictionary()->addFloatIndex(key, tree);

    	} else if (rel->getTypeName(attrIndex) == Global::DOUBLE) {
    		BPlusTree<double, unsigned long int> * tree;
    		tree = new BPlusTree<double, unsigned long int>(indexFileName.c_str(), indexKeyLen, valueLen, false);
    		Dictionary::getDictionary()->addDoubleIndex(key, tree);

    	} else if (rel->getTypeName(attrIndex) == Global::CHAR) {
    		BPlusTree<string, unsigned long int> * tree;
    		tree = new BPlusTree<string, unsigned long int>(indexFileName.c_str(), indexKeyLen, valueLen, false);
    		Dictionary::getDictionary()->addStringIndex(key, tree);

    	} else if (rel->getTypeName(attrIndex) == Global::VARCHAR) {
    		BPlusTree<string, unsigned long int> * tree;
    		tree = new BPlusTree<string, unsigned long int>(indexFileName.c_str(), indexKeyLen, valueLen, false);
    		Dictionary::getDictionary()->addStringIndex(key, tree);
    	}
    }
    fclose(dicFile);
    
    printf("initial success!!!\n");
}

/*
 * 创建数据库
 */
void DBMS::createDatabase(char * dbName, int blockSize) {
	//判断该数据库名称是否已经存在
	if (isExist(dbName)) {
		string str(dbName);
		string message = str + " already exist";
		throw DatabaseCreateException(message);
	}
	//创建该数据库的文件夹
	string folderName(dbName);
	folderName = "data/" + folderName;
	//先判断文件夹是否存在，存在返回0，不存在返回-1
	if (access(folderName.c_str(), F_OK) == -1) {
		//创建数据库对应的文件夹，成功返回0，不成功返回-1
		if (mkdir(folderName.c_str(), S_IRWXU) == -1) {
			throw DatabaseCreateException(dbName);
		}
	}

	//创建数据库字典文件
	FILE * dicFile;
	string dicFileName = folderName + "/" + dbName + ".desc";
	if ((dicFile = fopen(dicFileName.c_str(), "w")) == NULL) {
		throw FileNotFoundException(dicFileName);
	}
	fprintf(dicFile, "%d\n", 0);			//初始化数据库关系表个数为0
	fprintf(dicFile, "%d\n", 0);			//初始化索引个数为0
	fprintf(dicFile, "%d\n", blockSize);	//块的大小
	fclose(dicFile);

	databases.push_back(dbName);
	//写入数据库文件
	FILE * dbs;
	if ((dbs = fopen("data/databases.db", "a")) == NULL) {
		throw FileNotFoundException("data/databases.db");
	}
	fprintf(dbs, "%s\n", dbName);
	fclose(dbs);
}
/*
 * 创建关系表
 * vector<pair<attrName, pair<typeName, typeValue>>>
 */
void DBMS::createTable(char * relName, vector<pair<string, pair<string, int>>> attrs) {
	//以下两行为测试用
	currentDatabase = "abc";
	Dictionary::getDictionary()->setCurDatabaseName(currentDatabase.c_str());

	//未使用数据库 use databaseName;
	if (currentDatabase == "") {
		throw TableCreateException("no database selected");
	}
	//关系表重名
	const Relation * r = Dictionary::getDictionary()->getRelation(relName);
	if (r) {
		string error("the table \'");
		error.append(relName);
		error.append("\' already exist");
		throw TableCreateException(error);
	}

	//Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName);
	char * relFileName = (char*)malloc(Global::MAX_RELATION_FILE_NAME);
	strcpy(relFileName, relName);
	strcat(relFileName, ".rel");

	//Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName)
	//初始化totalBlock定义为0
	Relation * relation = new Relation(0, attrs.size(), relName, relFileName);
	for (auto it = attrs.begin(); it != attrs.end(); it++) {
		string attrName = it->first;
		pair<string, int> typeValue = it->second;
		cout << attrName << " " << typeValue.first << " " << typeValue.second << endl;
	}

	for (auto it = attrs.begin(); it != attrs.end(); it++) {
		string attrName = it->first;
		pair<string, int> typeValue = it->second;
		relation->addAttribute(attrName.c_str());
		int typeToInt;
		if (strcmp(typeValue.first.c_str(), "int")==0){
			typeToInt = Global::INTEGER;
		} else if (strcmp(typeValue.first.c_str(), "char") == 0){
			typeToInt = Global::CHAR;
		} else if (strcmp(typeValue.first.c_str(), "varchar") == 0){
			typeToInt = Global::VARCHAR;
		} else if (strcmp(typeValue.first.c_str(), "float") == 0){
			typeToInt = Global::FLOAT;
		} else if (strcmp(typeValue.first.c_str(), "double") == 0){
			typeToInt = Global::DOUBLE;
		}
		relation->addType(typeToInt, typeValue.second);
	}
	Dictionary::getDictionary()->addRelation(relation);

	//创建关系表文件,此表用于存放数据的
	// data/'databaseName'/'relFileName'
	string relFilePath("data/");
	relFilePath.append(currentDatabase);
	relFilePath.append("/");
	relFilePath.append(relFileName);

	FILE * relFile;
	if ((relFile = fopen(relFilePath.c_str(), "wb")) == NULL) {
		string error("can't create file \'");
		error.append(relFileName);
		error.append("\'");
		throw TableCreateException(error);
	}
	fclose(relFile);
	//跟新.rel数据库字典文件
	Dictionary::getDictionary()->writeBack();
}
/*
 * 数据的插入
 */
void DBMS::insert(const char * tableName, vector<string> values) {
	//未使用数据库 use databaseName;
	if (currentDatabase == "") {
		throw InsertDataException("no database selected");
	}

	Relation * rel = Dictionary::getDictionary()->getRelation(tableName);
	if (rel == nullptr) {
		string error("Database \'");
		error.append(currentDatabase);
		error.append("\' has no table \'");
		error.append(tableName);
		error.append("\'");
		throw InsertDataException(error);
	}
	//new a tuple
	Tuple * tup = new Tuple(rel);
	for (int i = 0; i < values.size(); i++) {
		int typeName = rel->getTypeName(i);
		if (typeName == Global::INTEGER) {
			try {
				int intValue = stoi(values.at(i));
				tup->addInteger(intValue);
			} catch (exception & e) {
				//Cannot convert "abc" to "int"
				string error("Cannot convert \'");
				error.append(values.at(i));
				error.append("\' to \'int\'");
				throw InsertDataException(error);
			}
		} else if (typeName == Global::FLOAT) {
			try {
				float floatValue = stof(values.at(i));
				tup->addFload(floatValue);
			} catch (exception & e) {
				string error("Cannot convert \'");
				error.append(values.at(i));
				error.append("\' to \'float\'");
				throw InsertDataException(error);
			}
		} else if (typeName == Global::DOUBLE) {
			try {
				double doubleValue = stod(values.at(i));
				tup->addDouble(doubleValue);
			} catch (exception & e) {
				string error("Cannot convert \'");
				error.append(values.at(i));
				error.append("\' to \'double\'");
				throw InsertDataException(error);
			}
		} else if (typeName == Global::CHAR) {
			if (values.at(i).length() > rel->getTypeValue(i)) {
				string error("\'");
				error.append(values.at(i));
				error.append("\' exceeds the defined length");
				throw InsertDataException(error);
			} else {
				tup->addChar(values.at(i).c_str(), rel->getTypeValue(i));
			}
		} else if (typeName == Global::VARCHAR) {
			if (values.at(i).length() > rel->getTypeValue(i)) {
				string error("\'");
				error.append(values.at(i));
				error.append("\' exceeds the defined length");
			} else {
				tup->addVarchar(values.at(i).c_str(), rel->getTypeValue(i));
			}
		}
	}
	tup->processData();		//一定要有

	//add to block
	//获取关系表中的最后一块，看该块是否满了，如果没满则在这个块中插入数据，否则新建一块
	unsigned int totalBlock = rel->getTotalBlock();
	string blockName(tableName);
//	string blockId = to_string(totalBlock);
//	blockName.append(blockId);

	Block * block = nullptr;
	if (totalBlock == 0) {
		blockName.append("0");

		block = new Block(totalBlock, rel);
		totalBlock += 1;
		rel->setTotalBlock(totalBlock);
	} else {
		string blockId = to_string(totalBlock - 1);
		blockName.append(blockId);
		try {
			block = lru->get(blockName);
		} catch (exception & e) {			//该块不在缓冲区中
			//Block * getBlock(const string databaseName, int blockId);
			block = rel->getBlock(currentDatabase, totalBlock);
		}
	}
	//block->addTuple(tup->getResult(), tup->getTupLength());
	if (block->getFreespace() > Dictionary::getDictionary()->getHeadspace()) {
		block->addTuple(tup->getResult(), tup->getTupLength());
		Block * b = lru->put(blockName, block);
		if (b) {
			delete b;
		}
	} else {
		blockName = "";
		blockName.append(tableName);
		string blockId = to_string(totalBlock);
		blockName.append(blockId);

		block = new Block(totalBlock, rel);

		totalBlock += 1;
		rel->setTotalBlock(totalBlock);

		block->addTuple(tup->getResult(), tup->getTupLength());
		Block * b = lru->put(blockName, block);
		if (b) {
			delete b;
		}
	}
	delete tup;
	cout << "insert OK" << endl;
	Dictionary::getDictionary()->setChange(true);
}
/*
 * 更新数据库文件
 */
void DBMS::writeBack() {
	FILE * dbs;
	if ((dbs = fopen("data/databases.db", "w")) == NULL) {
		throw FileNotFoundException("data/databases.db");
	}
	for (auto it = databases.begin(); it != databases.end(); it++) {
		fprintf(dbs, "%s\n", *it);
	}
	fclose(dbs);
}
/*
 * 判断数据库名称是否存在
 */
bool DBMS::isExist(char * dbName) {
	for (auto it = databases.begin(); it != databases.end(); it++) {
		if (strcmp(dbName, *it) == 0) {
			return true;
		}
	}
	return false;
}


void DBMS::test() {
	FILE * testFile;
	if ((testFile = fopen("testFile.ts", "r")) == NULL) {
		throw FileNotFoundException("testFile.ts");
	}
	unsigned int totalBlock = Dictionary::getDictionary()->getRelation(0)->getTotalBlock();
	Block * block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(0));
	string blockName(Dictionary::getDictionary()->getRelation(0)->getRelationName());
	string id = to_string(totalBlock);
	blockName.append(id);
	totalBlock += 1;
	Dictionary::getDictionary()->getRelation(0)->setTotalBlock(totalBlock);

	for (int i = 0; i < 500; i++) {
		char name1[20];
		char name2[20];
		int num;
		fscanf(testFile, "%s%s%d", name1, name2, &num);
		Tuple * tup = new Tuple(Dictionary::getDictionary()->getRelation(0));
		tup->addChar(name1, Dictionary::getDictionary()->getRelation(0)->getTypeValue(0));
		tup->addVarchar(name2, strlen(name2));
		tup->addInteger(num);
		tup->processData();
//		cout << "freespace : " << block->getFreespace() << endl;
		if (block->getFreespace() > 200) {
			block->addTuple(tup->getResult(), tup->getTupLength());
		} else {
//			block->writeBack();
			Block * b = lru->put(blockName, block);
			if (b) {
//				b->printBlock();
				delete b;
			}

			totalBlock = Dictionary::getDictionary()->getRelation(0)->getTotalBlock();
			block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(0));
			id = to_string(totalBlock);
			blockName = Dictionary::getDictionary()->getRelation(0)->getRelationName();
			blockName.append(id);
			block->addTuple(tup->getResult(), tup->getTupLength());
			totalBlock += 1;
			Dictionary::getDictionary()->getRelation(0)->setTotalBlock(totalBlock);
		}
		delete tup;
	}
//	block->writeBack();
	Block * b = lru->put(blockName, block);	//------------差点出错------------------最后一块也要放入lru中
	if (b) {
		delete b;
	}
	fclose(testFile);
	Dictionary::getDictionary()->writeBack();
	cout << "test() finish" << endl;

}
void DBMS::test2() {
	FILE * testFile;
	if ((testFile = fopen("testFile2.ts", "r")) == NULL) {
		throw FileNotFoundException("testFile2.ts");
	}
	unsigned int totalBlock = Dictionary::getDictionary()->getRelation(1)->getTotalBlock();
	Block * block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(1), 4);
	string blockName(Dictionary::getDictionary()->getRelation(1)->getRelationName());
	string id = to_string(totalBlock);
	blockName.append(id);
	totalBlock += 1;
	Dictionary::getDictionary()->getRelation(1)->setTotalBlock(totalBlock);
	for (int i = 0; i < 500; i++) {
		char str1[20];
		char str2[20];
		fscanf(testFile, "%s%s", str1,str2);
		Tuple * tup = new Tuple(Dictionary::getDictionary()->getRelation(1));
		tup->addChar(str1, Dictionary::getDictionary()->getRelation(1)->getTypeValue(0));
		tup->addVarchar(str2, strlen(str2));
		tup->processData();

		if (block->getFreespace() > 200) {
			block->addTuple(tup->getResult(), tup->getTupLength());
		} else {
//			block->writeBack();
			Block * b = lru->put(blockName, block);
			if (b) {
//				b->printBlock();
				delete b;
			}

			block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(1), 4);
			id = to_string(totalBlock);
			blockName = Dictionary::getDictionary()->getRelation(1)->getRelationName();
			blockName.append(id);
			block->addTuple(tup->getResult(), tup->getTupLength());
			totalBlock += 1;
			Dictionary::getDictionary()->getRelation(1)->setTotalBlock(totalBlock);
		}
		delete tup;
	}
//	block->writeBack();
	Block * b = lru->put(blockName, block);		//这一部很关键 不要忘了 不然会少一块没有被写回文件
	if (b) {
		delete b;
	}
	fclose(testFile);
	Dictionary::getDictionary()->writeBack();
	cout << "test2() finish" << endl;

}














