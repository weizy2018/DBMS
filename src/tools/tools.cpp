/*
 * tools.cpp
 *
 *  Created on: Nov 6, 2018
 *      Author: weizy
 */

#include "head/tools.h"
#include <stdlib.h>
#include <ctime>
#include <iostream>
using namespace std;

Tools * Tools::tools = nullptr;

Tools::Tools() {
	srand(0);
}

Tools::~Tools() {
	// TODO Auto-generated destructor stub
}

Tools * Tools::getToolsInst() {
	if (tools == nullptr) {
		tools = new Tools;
	}
	return tools;
}

void Tools::releaseInst() {
	delete tools;
}


char * Tools::generateRowId() {
	char * id = (char*) malloc(sizeof(time_t) + sizeof(int));
	time_t t = time(0);
	int ran = rand();

//	cout << "time_t : " << t << "  ran : " << ran << endl;

	int index = 0;
	char * p = (char*) &t;
	for (unsigned int i = 0; i < sizeof(t); i++, index++) {
		id[index] = p[i];
	}
	p = (char*) &ran;
	for (unsigned int i = 0; i < sizeof(ran); i++, index++) {
		id[index] = p[i];
	}
	return id;
}

