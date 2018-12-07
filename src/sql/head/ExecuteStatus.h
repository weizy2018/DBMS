/*
 * ExecuteStatus.h
 *
 *  Created on: Dec 5, 2018
 *      Author: weizy
 */

#ifndef SQL_HEAD_EXECUTESTATUS_H_
#define SQL_HEAD_EXECUTESTATUS_H_


class ExecuteStatus {
public:
	ExecuteStatus();
	virtual ~ExecuteStatus();
protected:

public:
	virtual void execute();
};

#endif /* SQL_HEAD_EXECUTESTATUS_H_ */
