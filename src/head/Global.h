/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Global.h
 * Author: weizy
 *
 * Created on November 1, 2018, 7:40 PM
 */

#ifndef GLOBAL_H
#define GLOBAL_H
class Global {
public:
    Global();
    virtual ~Global();

public:
    const static int INTEGER = 1;
    const static int FLOAT   = 2;
    const static int DOUBLE	 = 3;
    const static int CHAR    = 4;
    const static int VARCHAR = 5;
	
	const static int MAX_RELATION_NAME 		= 20;
	const static int MAX_IDNEX_NAME 		= 20;
	const static int MAX_RELATION_FILE_NAME = 25;
	
	const static int TYPE_LENGTH = 10;	 //属性的名称长度

protected:

private:
};


#endif /* GLOBAL_H */

