/*
 * tools.h
 *
 *  Created on: Nov 6, 2018
 *      Author: weizy
 */

#ifndef TOOLS_HEAD_TOOLS_H_
#define TOOLS_HEAD_TOOLS_H_

class Tools {
public:
	static Tools * getToolsInst();
	static void releaseInst();
private:
	Tools();
	Tools(const Tools & );
	Tools & operator = (const Tools &);

	static Tools * tools;
public:
	virtual ~Tools();
public:
	char * generateRowId();
};

#endif /* TOOLS_HEAD_TOOLS_H_ */
