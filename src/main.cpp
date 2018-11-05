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

//    Block * block1 = new Block(0, 3, 44);
//    Block * block2 = new Block(1, 3, 44, 8);

//    cout << "Block1 : " << block1->getMaxTups() << endl;
//    cout << "Block2 : " << block2->getMaxTups() << endl;

//    delete block1;
//    delete block2;

    Dictionary::getDictionary()->releaseDictionary();

    return 0;
}
