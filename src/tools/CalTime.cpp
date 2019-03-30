/*
 * CalTime.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: weizy
 */

#include "head/CalTime.h"
#include <iostream>
using namespace std;


CalTime * CalTime::tm = nullptr;

CalTime::CalTime() {
	// TODO Auto-generated constructor stub
	startTime = time(0);
	endTime = time(0);
	row = 0;
}

CalTime::~CalTime() {
	// TODO Auto-generated destructor stub
}

CalTime * CalTime::getCalTimeInst() {
	if (tm == nullptr) {
		tm = new CalTime();
	}
	return tm;
}
void CalTime::releaseCalTime() {
	delete tm;
}

void CalTime::setStartTime() {
	startTime = clock();
}
void CalTime::setEndTime() {
	endTime = clock();
}
double CalTime::getTime() {
	return (double)(endTime - startTime)/CLOCKS_PER_SEC;
}

void CalTime::resetRow() {
	row = 0;
}
void CalTime::addRow() {
	row++;
}
int CalTime::getRow() {
	return row;
}

