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
#include "sql/head/SQL.h"
#include "exception/head/SqlSyntaxException.h"
#include "exception/head/Error.h"

using namespace std;

void test();

int main(int argc, char** argv) {

	test();

//	int memorySize;
//	cout << "输入可用的主存空间： ";
//	cin >> memorySize;
//
//    DBMS * dbms = new DBMS(memorySize);
//    char dicName[MAX_DATABASE_NAME];
//    strcpy(dicName, "school");
//    dbms->initialDictionary(dicName);
//    Dictionary::getDictionary()->printDictionary();
//
//    dbms->test();
//    dbms->test2();
//
//
//    delete dbms;	//一定要先释放dbms后再释放Dictionary 不然后面将块写回文件的时候会找不到Dictionary而报错
//    Dictionary::getDictionary()->releaseDictionary();
//    Tools::getToolsInst()->releaseInst();

    return 0;
}
void test() {
	SQL * sql = SQL::getsqlInst();
	while (!sql->isFinish()) {
		sql->inputSql();
		try {
			sql->parse();
			sql->execute();
		} catch (Error & e) {
			e.what();
		} catch (SqlSyntaxException & e) {
			e.what();
		} catch (exception & e) {
			cout << e.what() << endl;
		}
	}

	SQL::releaseInst();
//	DBMS::releaseDBMSInst();		//在SQL.cpp中执行exit后已经执行该操作，这里不再需要
}
