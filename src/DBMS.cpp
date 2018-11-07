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
#include <string.h>
#include <string>

#include "head/DBMS.h"
#include "head/Global.h"
#include "head/Tuple.h"

using namespace std;

DBMS::DBMS() {
	lru = new LruCache<string, Block *>(3);
}


DBMS::~DBMS() {
	delete lru;

}

void DBMS::initialDictionary() {
	cout << "DBMS::inititalDictionary" << endl;
    FILE * dicFile;

    if ((dicFile = fopen("school.dic", "r")) == NULL) {
        cout << "can't open the file dictionary.dic" << endl;
        exit(0);
    }
    int totalRelationship;
    fscanf(dicFile, "%d", &totalRelationship);
    
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
            
            rel->addType(typeToInt, value, j);
        }
        Dictionary::getDictionary()->addRelation(rel);
    }

    fclose(dicFile);
    
    printf("initial success!!!\n");
}
void DBMS::test() {
	FILE * testFile;
	if ((testFile = fopen("testFile.ts", "r")) == NULL) {
		printf ("can't open file testFile.ts\n");
		return;
	}
	unsigned int totalBlock = Dictionary::getDictionary()->getRelation(0)->getTotalBlock();
	Block * block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(0));
	string blockName(Dictionary::getDictionary()->getRelation(0)->getRelationName());
	string id = to_string(totalBlock);
	blockName.append(id);

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
			Block * b = lru->put(blockName, block);
			if (b) {
				b->printBlock();
				delete b;
			}
			totalBlock += 1;
			Relation * r = Dictionary::getDictionary()->getRelation(0);
			r->setTotalBlock(totalBlock);
			totalBlock = Dictionary::getDictionary()->getRelation(0)->getTotalBlock();
			block = new Block(totalBlock, Dictionary::getDictionary()->getRelation(0));
			id = to_string(totalBlock);
			blockName = Dictionary::getDictionary()->getRelation(0)->getRelationName();
			blockName.append(id);

			block->addTuple(tup->getResult(), tup->getTupLength());
		}
		delete tup;
	}
	fclose(testFile);
	cout << "test() finish" << endl;

}














