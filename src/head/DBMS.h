/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DBMS.h
 * Author: weizy
 *
 * Created on November 1, 2018, 7:35 PM
 */

#ifndef DBMS_H
#define DBMS_H

#include "Dictionary.h"
#include "../tools/head/lru.h"
#include "Block.h"

class DBMS {
public:
	DBMS();
	DBMS(const DBMS& orig);
	virtual ~DBMS();

public:
	void initialDictionary();
	void test();
private:
	LruCache<string, Block *> * lru;

};

#endif /* DBMS_H */

