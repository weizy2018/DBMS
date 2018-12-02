//============================================================================
// Name        : DBMS01.cpp
// Author      : weizy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstdlib>
#include <iostream>
#include <string.h>

#include "head/DBMS.h"
#include "head/Block.h"
#include "head/Tuple.h"
#include "tools/head/tools.h"

using namespace std;

int main(int argc, char** argv) {
	int memorySize;
	cout << "输入可用的主存空间： ";
	cin >> memorySize;

    DBMS * dbms = new DBMS(memorySize);
    char dicName[MAX_DATABASE_NAME];
    strcpy(dicName, "school");
    dbms->initialDictionary(dicName);
    Dictionary::getDictionary()->printDictionary();

    dbms->test();
    dbms->test2();


    delete dbms;	//一定要先释放dbms后再释放Dictionary 不然后面将块写回文件的时候会找不到Dictionary而报错
    Dictionary::getDictionary()->releaseDictionary();
    Tools::getToolsInst()->releaseInst();

    return 0;
}
