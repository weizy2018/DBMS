/*
 * ExecuteStatus.h
 *
 *  Created on: Dec 5, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_EXECUTESTATUS_H_
#define SQL_HEAD_EXECUTESTATUS_H_

#include <string>
#include <vector>

#include "../../head/DBMS.h"

using namespace std;

class ExecuteStatus {
public:
	ExecuteStatus(const vector<string> ws);
	virtual ~ExecuteStatus();
protected:
	const vector<string> words;
public:
	virtual void execute();
};

#endif /* SQL_HEAD_EXECUTESTATUS_H_ */
