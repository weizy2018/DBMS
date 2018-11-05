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

#include "head/DBMS.h"
#include "head/Global.h"

using namespace std;

DBMS::DBMS() {
}

DBMS::DBMS(const DBMS& orig) {
}

DBMS::~DBMS() {
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

