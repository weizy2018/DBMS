//============================================================================
// Name        : DBMS01.cpp
// Author      : weizy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstdlib>
#include<iostream>

#include "head/DBMS.h"
#include "head/Block.h"
#include "head/Tuple.h"
#include "tools/head/tools.h"

using namespace std;

/*
 * s
 */
int main(int argc, char** argv) {
    cout<<"Hello World"<<endl;
    cout << "Hello DBMS" << endl;

    DBMS * dbms = new DBMS;
    dbms->initialDictionary();
    Dictionary::getDictionary()->printDictionary();

//    Tuple * tup1 = new Tuple(Dictionary::getDictionary()->getaRelation(0));
//    char cch[] = "abc";
//    char vvch[] = "def";
//    tup1->addChar(cch, 20);
//    tup1->addVarchar(vvch, 3);
//    tup1->addInteger(66);
//    tup1->processData();
//
//    Tuple * tup2 = new Tuple(Dictionary::getDictionary()->getaRelation(0));
//    char cch2[] = "abcd";
//    char vvch2[] = "defg";
//    tup2->addChar(cch2, 20);
//    tup2->addVarchar(vvch2, 4);
//    tup2->addInteger(88);
//    tup2->processData();
//
//    unsigned int a = 0;
//    Block * block = new Block(a, Dictionary::getDictionary()->getaRelation(0));
//    block->addTuple(tup1->getResult(), tup1->getTupLength());
//    block->addTuple(tup2->getResult(), tup2->getTupLength());
//    cout << "----------------------------------------" << endl;
//    block->printBlock();
//
//    delete block;
//    delete tup2;
//    delete tup1;

    dbms->test();

    Dictionary::getDictionary()->releaseDictionary();
    Tools::getToolsInst()->releaseInst();
    delete dbms;

    return 0;
}
