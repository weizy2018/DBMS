/*
 * CalTime.h
 *
 *  Created on: Mar 2, 2019
 *      Author: weizy
 */

#ifndef TOOLS_HEAD_CALTIME_H_
#define TOOLS_HEAD_CALTIME_H_

#include <ctime>

class CalTime {
private:
	CalTime();
	CalTime(const CalTime & tm);
	CalTime & operator = (const CalTime &);
public:
	virtual ~CalTime();
private:
	static CalTime * tm;

	clock_t startTime;
	clock_t endTime;

	int row;

public:
	static CalTime * getCalTimeInst();
	static void releaseCalTime();

	void setStartTime();
	void setEndTime();
	double getTime();

	void resetRow();
	void addRow();
	int getRow();

};

#endif /* TOOLS_HEAD_CALTIME_H_ */
