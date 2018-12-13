/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Block.h
 * Author: weizy
 *
 * Created on November 1, 2018, 7:06 PM
 */

#ifndef BLOCK_H
#define BLOCK_H
#include<vector>
#include "Dictionary.h"
#include "Tuple.h"


using namespace std;

//一些block头部信息的开始位置
#define BLOCK_ID_START 		0
#define BLOCK_SIZE_START 	4
#define FREE_SPACE_START 	6
#define TUPLES_START 		14
#define POSITION_START		18
#define POSITION_SIZE 		8

typedef unsigned int position_start;
typedef int offset_length;

typedef unsigned int block_id;
typedef short block_size;

#define TUPLES_COUNT 20             //规定每行的属性个数

class Position;
class Relation;

class Block {
public:
    Block(block_id blockId, const Relation * rel);
    Block(block_id blockId, const Relation * rel, block_size blockSize);
    Block(char * block, const Relation * rel);
    virtual ~Block();
    
//这两没有使用过，并没有给出实现
private:
    void insertRecord(char *record, int recordLength);
    void removeRecord();

private:
//    char * fileName;                //块对应的文件名称(好像也没啥用，先留着吧)

    //------块头包含属性-------
    block_id blockId;           	//每个块的id,表示该块对应文件中的第几块，从中可以计算该块在文件中的首地址 addr = blockId*1024;
    block_size blockSize;           //块的大小，默认为4K  4
    int tups;                       //块中包含的元组个数
//    int attr;                       //关系表中的属性个数
    Position * freeSpace;           //空闲空间

    bool change;		            //修改标志   如果没有修改，写回的时候就不需要真的写了
    vector<Position *> pos;         //记录每个元组在块中的初始位置以及结束位置

    //-------块的定义-------
    char * block;                   //块
    const Relation * relation;

    int dictionary[TUPLES_COUNT];   //块中每个元组的各个属性的类型(暂时还没用到)

    int maxTupLength;				//每个元组最大长度
//    int maxTups;                    //最多的元组数  maxTups = (blockSize-else)/tupsMaxSize
private:
    void initBlock();
    void updateTups(int value);		//更新块头中记录元组个数的数据
    void updateFreeSpace(const position_start start, const offset_length length);
    void addPosition(Position * position);				//向块头中添加一组元组的信息

    void parsedBlock();									//解析char * block
public:
    block_id getBlockId();
    vector<Tuple *> getBlockTupls();
public:
    void printBlock();              					//用于输出元组的内容
    int getFreespace();									//获取该块中剩余的空闲空间
    void addTuple(const char * p, int tupSize);			//向该块中添加一个元组
    void writeBack();
};

class Position {
private:
    position_start start;
    offset_length length;

public:
	Position(position_start start, offset_length length){
		this->start = start;
		this->length = length;
	}
	position_start getStart() {
        return start;
    }
    void setStart(position_start start) {
        this->start = start;
    }
    offset_length getLength() {
        return length;
    }
    void setLength(offset_length length) {
        this->length = length;
    }
};

#endif // BLOCK_H


