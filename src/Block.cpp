/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Block.cpp
 * Author: weizy
 * 
 * Created on November 1, 2018, 7:06 PM
 */

#include "head/Block.h"
#include "head/Tuple.h"
#include "exception/head/FileNotFoundException.h"
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

Block::Block(block_id blockId, const Relation * rel) {
	relation = rel;
    this->blockId = blockId;
    blockSize = 4;
    tups = 0;							//初始化块中包含的元组数为0
//    this->attr = tr;

    position_start start = sizeof(blockId) + sizeof(blockSize) + sizeof(freeSpace) + sizeof(tups);
    freeSpace = new Position(start, 1024*blockSize - start);

    change = true;
    block = (char*)malloc(1024*4);		//默认大小 4K

//    this->maxTupLength = maxTupLen;
//    maxTups = (1024*blockSize - sizeof(blockId) - sizeof(blockSize)
//    		- sizeof(Position) - sizeof(tups))/(sizeof(Position) + maxTupLength);//可容纳的最多的元组数

    initBlock();
}
Block::Block(block_id blockId, const Relation * rel, block_size blockSize){
	relation = rel;
	this->blockId = blockId;
	this->blockSize = blockSize;
	tups = 0;
//	this->attr = tr;

	position_start start = sizeof(blockId) + sizeof(blockSize) + sizeof(freeSpace) + sizeof(tups);
	freeSpace = new Position(start, 1024*blockSize - start);

	change = true;
	block = (char*)malloc(1024*blockSize);
//	this->maxTupLength = maxTupLen;
//	maxTups = (1024*blockSize - sizeof(blockId) - sizeof(blockSize)
//    		- sizeof(Position) - sizeof(tups))/(sizeof(Position) + maxTupLength);

	initBlock();
}

Block::Block(char * block, const Relation * rel){
	this->block = block;
	this->relation = rel;
	change = false;
	parsedBlock();
}

Block::~Block() {
	writeBack();
	free(block);
	while (!pos.empty()){
		Position * p = pos.back();
		pos.pop_back();
		delete p;
	}
	delete freeSpace;
//	cout << "~Block()" << endl;
}

void Block::clearBlock() {
	delete freeSpace;
	position_start start = sizeof(blockId) + sizeof(blockSize) + sizeof(freeSpace) + sizeof(tups);
	freeSpace = new Position(start, 1024*blockSize - start);
	tups = 0;
	for (auto it = pos.begin(); it != pos.end(); it++) {
		delete (*it);
	}
	pos.clear();
	change = true;
	initBlock();
}

void Block::initBlock(){
	char * b = block;
	int index = 0;

	//blockId
	const char * bId = (char*)&blockId;
	for (unsigned int i = 0; i < sizeof(blockId); i++,bId++,index++){
		b[index] = *bId;
	}

	//blockSize
	const char * bs = (char*)&blockSize;
	for (unsigned int i = 0; i < sizeof(blockSize); i++,bs++,index++){
		b[index] = *bs;
	}

	//freeSpace
	position_start positionStart = freeSpace->getStart();
	const char * fs = (char*)&positionStart;
	for (unsigned int i = 0; i < sizeof(freeSpace->getStart()); i++,fs++,index++){
		b[index] = *fs;
	}
	offset_length offset = freeSpace->getLength();
	fs = (char*)&offset;
	for (unsigned int i = 0; i < sizeof(freeSpace->getLength()); i++,fs++,index++){
		b[index] = *fs;
	}

	//tups
	const char * tup = (char*)&tups;
	for (unsigned int i = 0; i < sizeof(tups); i++,tup++,index++){
		b[index] = *tup;
	}
}

int Block::getFreespace(){
	return freeSpace->getLength();
}
void Block::updateTups(int value){
	char * b = block;
	this->tups = value;
	const char * tup = (char*)&tups;
	int index = TUPLES_START;
	for (unsigned int i = 0; i < sizeof(tups); i++,tup++,index++){
		b[index] = *tup;
	}
}
void Block::updateFreeSpace(const position_start start, const offset_length length){
	this->freeSpace->setStart(start);
	this->freeSpace->setLength(length);
	char * b = block;

	int index = FREE_SPACE_START;
	char * fs = (char*)&start;
	for (unsigned int i = 0; i < sizeof(start); i++,fs++,index++){
		b[index] = *fs;
	}
	char * fl = (char*)&length;
	for(unsigned int i = 0; i < sizeof(length); i++,fl++,index++){
		b[index] = *fl;
	}
}
void Block::addPosition(Position * position){
	pos.push_back(position);
	char * b = block;
	int index = freeSpace->getStart();

	position_start positionStart = position->getStart();
	offset_length positionOffset = position->getLength();
	char * ps = (char*)&positionStart;
	for (unsigned int i = 0; i < sizeof(positionStart); i++){
		b[index] = *ps;
		ps++;
		index++;
	}
	char *pl = (char*)&positionOffset;
	for (unsigned int i = 0; i < sizeof(positionOffset); i++){
		b[index] = *pl;
		pl++;
		index++;
	}
}

void Block::addTuple(const char *p, int tupSize){
//	Position * ps = new Position(freeSpace->getStart(), tupSize);
	change = true;

	int blockIndex = freeSpace->getStart() + freeSpace->getLength() - tupSize;
	Position * ps = new Position(blockIndex, tupSize);
	char * b = block;
	for (int i = 0; i < tupSize; i++, blockIndex++){
		b[blockIndex] = p[i];
	}

	updateTups(tups+1);
	addPosition(ps);
	updateFreeSpace(freeSpace->getStart() + POSITION_SIZE, freeSpace->getLength() - POSITION_SIZE - tupSize);
}

void Block::parsedBlock(){
	int index = 0;
	//blockId
	char * bId = (char*)&blockId;
	for (unsigned int i = 0; i < sizeof(blockId); i++){
		bId[i] = block[index++];
	}
	//blockSize
	char * bs = (char*)&blockSize;
	for (unsigned int i = 0; i < sizeof(blockSize); i++) {
		bs[i] = block[index++];
	}
	//freeSpace
	position_start start;
	offset_length length;
	char * st = (char*)&start;
	for (unsigned int i = 0; i < sizeof(start); i++) {
		st[i] = block[index++];
	}
	char * lh = (char*)&length;
	for (unsigned int i = 0; i < sizeof(length); i++) {
		lh[i] = block[index++];
	}
	freeSpace = new Position(start, length);
	//tups
	char * tp = (char*)&tups;
	for (unsigned int i = 0; i < sizeof(tups); i++){
		tp[i] = block[index++];
	}
	//pos
	for (int i = 0; i < tups; i++) {
		for (unsigned int j = 0; j < sizeof(start); j++) {
				st[j] = block[index++];
		}
		for (unsigned int j = 0; j < sizeof(length); j++) {
				lh[j] = block[index++];
		}
		Position * p = new Position(start, length);
		pos.push_back(p);
	}
}
void Block::writeBack() {
	//如果块中的数据发生该表 change = true 则将数据写回文件
	if (change) {
//		cout << "Block::writeBack blockId = " << blockId << " blockSize: " << blockSize << endl;
		string url(Dictionary::getDictionary()->getCurDatabaseName());
		url = "data/" + url + "/";	//data/school/
		url.append(relation->getRelationFileName());	// data/school/student.rel  data/school/teacher.rel
		FILE * f;
		if ((f = fopen(url.c_str(), "rb+")) == NULL) {
			throw FileNotFoundException("can't open file : " + url);
		}
		fseek(f, 1024*blockSize*blockId, SEEK_SET);
//		cout << "Block::writeBack ftell() = " << ftell(f) << endl;
		fwrite(block, 1024*blockSize, 1, f);
		fclose(f);
		change = false;
	}
}


void Block::printBlock(){
	printf("block id : %u\n", blockId);
	printf("block size : %d\n", blockSize);
	printf("tups : %d\n", tups);
	printf("free space : %d\n", freeSpace->getLength());

	for (int i = 0; i < tups; i++) {
		position_start start = pos.at(i)->getStart();
		offset_length len = pos.at(i)->getLength();
		cout << "start : " << start << "  len : " << len << endl;
		char * b = (char*)malloc(len);
		for (int i = 0; i < len; i++){
			b[i] = block[start++];
		}
		Tuple * tuple = new Tuple(b, relation);
		tuple->printTuple();
		cout << endl;
		delete tuple;
	}
}

vector<Tuple *> Block::getBlockTupls() {
	vector<Tuple *> tupls;
	for (int i = 0; i < tups; i++) {
		position_start start = pos.at(i)->getStart();
		offset_length len = pos.at(i)->getLength();
		char * b = (char*)malloc(len);
		for (int i = 0; i < len; i++){
			b[i] = block[start++];
		}
		Tuple * tuple = new Tuple(b, relation, len);
		tupls.push_back(tuple);
	}
	return tupls;
}
block_id Block::getBlockId() {
	return this->blockId;
}
int Block::getTups() {
	return this->tups;
}






