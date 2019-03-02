/*
 * UpdateSql.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: weizy
 */

#include "head/UpdateSql.h"
#include "../head/Dictionary.h"
#include "../head/DBMS.h"
#include "../exception/head/Error.h"

#include <iostream>

UpdateSql::UpdateSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

UpdateSql::~UpdateSql() {
	// TODO Auto-generated destructor stub
	for (auto it = exprs.begin(); it != exprs.end(); it++) {
		delete (*it);
	}
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		delete (*it);
	}
}
//update Person set Address = 'Zhongshan', City = 'Nanjing', age = 18 where LastName = 'Wilson' and firstName = "wei"
void UpdateSql::execute() {
	if (DBMS::getDBMSInst()->getCurrentDatabase() == "") {
		throw Error("no database selected");
	}
	if (words.size() < 7) {				//至少也得有7个单词吧
		throw Error("sql syntax error");
	}
	Relation * rel = Dictionary::getDictionary()->getRelation(words[1].c_str());
	if (rel == nullptr) {
		string error("Table \'");
		error.append(DBMS::getDBMSInst()->getCurrentDatabase());
		error.append(".");
		error.append(words[1]);
		error.append("\' doesn't exist");
		throw Error(error);
	}
	handleSet();
	checkSet(rel);

	handleWhere();
	checkWhere(rel);

//	cout << "expression:" << endl;
//	for (unsigned int i = 0; i < exprs.size(); i++) {
//		cout << exprs[i]->left << " " << exprs[i]->symbol << " " << exprs[i]->right << " index = " << exprs[i]->leftIndex << endl;
//	}
//	cout << "conditions:" << endl;
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		cout <<conditions[i]->table << "." << conditions[i]->left << " " << conditions[i]->symbol << " " << conditions[i]->right;
//		cout << " index = " << conditions[i]->leftIndex << endl;
//	}

}

void UpdateSql::handleSet() {
	unsigned int index = 3;
	while (index < words.size()) {
		string left = words[index];
		string symbol;
		string right;
		if (index + 1 < words.size()) {
			if (words[index+1] != "=") {
				throw Error("sql syntax error");
			}
			symbol = words[index+1];
		} else {
			throw Error("sql syntax error");
		}
		if (index + 2 < words.size()) {
			if (words[index+2] == "\"" || words[index+2] == "\'") {
				right = words[index+3];
				index += 5;
			} else {
				right = words[index+2];
				index += 3;
			}
		} else {
			throw Error("sql syntax error");
		}
		Expression * exp = new Expression(left, right, symbol, words[1]);
		exprs.push_back(exp);

		if (words[index] == ",") {
			index++;
		} else if (words[index] == "where" || words[index] == ";") {
			break;
		} else {
			throw Error("sql syntax error");
		}
	}
}

void UpdateSql::handleWhere() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "where") {
		index++;
	}
	index++;
	if (index >= words.size()) {
		return;
	}
	joins.push_back("and");
	while (words[index] != ";") {
		string left;
		string right;
		string symbol;
		string table;
		if (index+1 < words.size() && words[index+1] == ".") {
			table = words[index];
			if (index+2 < words.size()) {
				left = words[index+2];
			} else {
				throw Error("sql syntax error");
			}
			index += 3;
		} else {
			table = words[1];
			left = words[index];
			index += 1;
		}
		//由于之前进行sql解析的是否没考虑!=，所以这里没有!=
		if (words[index] == "=" || words[index] == ">" || words[index] == "<" || words[index] == ">=" || words[index] == "<=") {
			symbol = words[index];
			index++;
		} else {
			throw Error("sql syntax exception");
		}

		if (words[index] == "\"" || words[index] == "\'") {
			right = words[index + 1];
			index += 3;
		} else {
			right = words[index];
			index += 1;
		}
		Expression * con = new Expression(left, right, symbol, table);
		conditions.push_back(con);

		if (words[index] == "and" || words[index] == "or") {
			joins.push_back(words[index]);
			index += 1;
		} else if (words[index] == ";") {
			break;
		} else {
			throw Error("sql syntax error");
		}
	}
}

void UpdateSql::checkSet(Relation * rel) {
	if (exprs.size() == 0) {
		throw Error("sql syntax error");
	} else {
		for (auto it = exprs.begin(); it != exprs.end(); it++) {
			int index = rel->getAttributeIndex((*it)->left);
			if (index == -1) {
				string error("Unknown column \'");
				error.append(words[1]);
				error.append(".");
				error.append((*it)->left);
				error.append("\' in \'field list\'");
				throw Error(error);
			} else {
				(*it)->leftIndex = index;
			}
		}
	}
}
void UpdateSql::checkWhere(Relation * rel) {
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		if ((*it)->table != words[1]) {
			string error("Unknown column \'");
			error.append((*it)->table);
			error.append(".");
			error.append((*it)->left);
			error.append("\' in \'where clause\'");
			throw Error(error);
		}
		int index = rel->getAttributeIndex((*it)->left);
		if (index == -1) {
			string error("Unknown column \'");
			error.append(words[1]);
			error.append(".");
			error.append((*it)->left);
			error.append("\' in \'where clause\'");
			throw Error(error);
		} else {
			(*it)->leftIndex = index;
		}
	}
}

void UpdateSql::update(Relation * rel) {
	int index = checkIndex();
	if (index != 1) {
		/*
		 * 使用索引查询更新
		 */
		set<unsigned long int> blocksId;
		int type = rel->getTypeName(conditions[index]->leftIndex);
		if (type == Global::CHAR || type == Global::VARCHAR) {
			BPlusTree<string, unsigned long int> * stringTree =
					Dictionary::getDictionary()->getStringIndex(conditions[index]->table, conditions[index]->left);
			blocksId = stringTree->get(conditions[index]->right);

		} else if (type == Global::INTEGER) {
			Bplustree<int, unsigned long int> * intTree =
					Dictionary::getDictionary()->getIntIndex(conditions[index]->table, conditions[index]->left);
			int key;
			try {
				key = stoi(conditions[index]->right);
			} catch (invalid_argument & e) {
				string error("Cannot convert \'");
				error.append(conditions[index]->right);
				error.append("\' to \'int\'");
				throw Error(error);
			}
			blocksId = intTree->get(key);

		} else if (type == Global::FLOAT) {
			Bplustree<float, unsigned long int> * floatTree =
					Dictionary::getDictionary()->getFloatIndex(conditions[index]->table, conditions[index]->left);
			float key;
			try {
				key = stof(conditions[index]->right);
			} catch (invalid_argument & e) {
				string error("Cannot convert \'");
				error.append(conditions[index]->right);
				error.append("\' to \'float\'");
				throw Error(error);
			}
			blocksId = floatTree->get(key);

		} else if (type == Global::DOUBLE) {
			Bplustree<double, unsigned long int> * doubleTree =
					Dictionary::getDictionary()->getDoubleIndex(conditions[index]->table, conditions[index]->left);
			double key;
			try {
				key = stod(conditions[index]->right);
			} catch (invalid_argument & e) {
				string error("Cannot convert \'");
				error.append(conditions[index]->right);
				error.append("\' to \'double\'");
				throw Error(error);
			}
			blocksId = doubleTree->get(key);
		}
		vector<unsigned long int> ids;
		unsigned int lastBlockId = rel->getTotalBlock() - 1;
		bool hasLastBlock = false;
		for (auto it = blocksId.begin(); it != blocksId.end(); it++) {
			if ((*it) == lastBlockId) {
				hasLastBlock = true;
			} else {
				ids.push_back(*it);
			}
		}
		if (hasLastBlock) {
			Block * block = DBMS::getDBMSInst()->getBlock(words[1], lastBlockId);
			if (block == nullptr) {
				block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), lastBlockId);
				DBMS::getDBMSInst()->putBlock(words[1], lastBlockId, block);
			}
			updateBlock(block, rel, true);
		}
		for (auto it = ids.begin(); it != ids.end(); it++) {
			Block * block = DBMS::getDBMSInst()->getBlock(words[1], *it);
			if (block == nullptr) {
				block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *it);
				DBMS::getDBMSInst()->putBlock(words[1], *it, block);
			}
			updateBlock(block, rel, false);
		}
	} else {
		unsigned int totalBlock = rel->getTotalBlock();
		unsigned int lastBlockId = totalBlock - 1;
		Block * lastBlock = DBMS::getDBMSInst()->getBlock(words[1], lastBlockId);
		if (lastBlock == nullptr) {
			lastBlock = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), lastBlockId);
			DBMS::getDBMSInst()->putBlock(words[1], lastBlockId, lastBlock);
		}
		updateBlock(lastBlock, rel, true);

		for (unsigned int i = 0; i < totalBlock - 1; i++) {
			Block * block = DBMS::getDBMSInst()->getBlock(words[1], i);
			if (block == nullptr) {
				block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
				DBMS::getDBMSInst()->putBlock(words[1], i, block);
			}
			updateBlock(block, rel, false);
		}
	}
}
//还需要考虑如果更新的列已经创建了索引的情况
void UpdateSql::updateBlock(Block * block, Relation * rel, bool lastBlock) {
	vector<Tuple *> tuples = block->getBlockTupls();
	vector<Tuple *> updatedTuples;		//更新后的元组
	vector<bool> updatedFlag;
	vector<Tuple *> removedTuples;		//被更新的元组
	bool updateFlag = false;			//确定该块是否需要更新的标志
	for (auto tup = tuples.begin(); tup != tuples.end(); tup++) {
		bool flag = true;
		for (unsigned int k = 0; k < conditions.size(); k++) {
			BasicType * left = (*tup)->getTupleBasicType(conditions[k]->leftIndex);
			int type = rel->getTypeName(conditions[k]->leftIndex);
			bool result = check(left, type, conditions[k]->symbol,	conditions[k]->right);
			if (joins[k] == "or") {
				flag |= result;
			} else {
				flag &= result;
			}
		}
		if (flag) {
			//更新
			updateFlag = true;			//更新块标志为true
			Tuple * tu = new Tuple(rel);
			int totalProperty = rel->getTotalProperty();
			for (int t = 0; t < totalProperty; t++) {		//新建一个元组,把原有内容和需要更新的内容放入新家的元组
				int type = rel->getTypeName(t);
				int exp = isInExpres(t);
				if (exp != -1) {
					/*
					 * 该元素在需要更新的列中
					 */
					updateFlag = true;				//更新块需要更新的标志
					if (type == Global::INTEGER) {
						try {
							int value = stoi(exprs[exp]->right);
							tu->addInteger(value);
						} catch (invalid_argument & e) {
							string error("Cannot convert \'");
							error.append(exprs[exp]->right);
							error.append("\' to \'int\'");
							throw Error(error);
						}
					} else if (type == Global::FLOAT) {
						try {
							float value = stof(exprs[exp]->right);
							tu->addFload(value);
						} catch (invalid_argument & e) {
							string error("Cannot convert \'");
							error.append(exprs[exp]->right);
							error.append("\' to \'float\'");
							throw Error(error);
						}
					} else if (type == Global::DOUBLE) {
						try {
							double value = stod(exprs[exp]->right);
							tu->addDouble(value);
						} catch (invalid_argument & e) {
							string error("Cannot convert \'");
							error.append(exprs[exp]->right);
							error.append("\' to \'double\'");
							throw Error(error);
						}
					} else if (type == Global::CHAR) {
						if (exprs[exp]->right.length() > (unsigned int) rel->getTypeValue(t)) {
							string error("\'");
							error.append(exprs[exp]->right);
							error.append("\' exceeds the defined length");
							throw Error(error);
						} else {
							tu->addChar(exprs[exp]->right.c_str(), rel->getTypeValue(t));
						}
					} else if (type == Global::VARCHAR) {
						if (exprs[exp]->right.length() > (unsigned int) rel->getTypeValue(t)) {
							string error("\'");
							error.append(exprs[exp]->right);
							error.append("\' exceeds the defined length");
							throw Error(error);
						} else {
							tu->addVarchar(exprs[exp]->right.c_str(), exprs[exp]->right.length());
						}
					}
				} else {
					/*
					 * 保持原内容不变
					 */
					BasicType * basic = (*tup)->getTupleBasicType(t);
					if (type == Global::INTEGER) {
						int * value = (int*)basic->getData();
						tu->addInteger(*value);
					} else if (type == Global::FLOAT) {
						float * value = (float*)basic->getData();
						tu->addFload(*value);
					} else if (type == Global::DOUBLE) {
						double * value = (double*)basic->getData();
						tu->addDouble(*value);
					} else if (type == Global::CHAR) {
						char * value = basic->getData();
						tu->addChar(value, basic->getDataLength());
					} else if (type == Global::VARCHAR) {
						char * value = basic->getData();
						tu->addVarchar(value, basic->getDataLength());
					}
				}
			} //for (int t = 0; t < totalProperty; t++)新建一个元组
			tu->processData();
			updatedTuples.push_back(tu);
			removedTuples.push_back(*tup);			//把原来的元组放入待删除的列表中
			updatedFlag.push_back(true);
		} else {
			/*
			 * 该元素不在更新的列中，把原有内容复制到新的元组中
			 */
			updatedTuples.push_back(*tup);
			updatedFlag.push_back(false);
		}

	} //for (auto tup = tuples.begin(); tup != tuples.end(); tup++)
	if (updateFlag) {
		/*
		 * 该块需要更新
		 * 1)跟新后该块还能放完
		 * 2)更新后超出了块的大小
		 *
		 * 如果是最后一块更新后超出了块的大小则需要新建一块
		 * 如果不是最后一块更新后超出了块的大小，先考虑最后一块能不能放下，如果不能新建一块
		 */
		block->clearBlock();

		for (unsigned int tups = 0; tups < updatedTuples.size(); tups++) {
			if (block->getFreespace() > (int)updatedTuples[tups]->getTupLength()) {
				block->addTuple(updatedTuples[tups]->getResult(), updatedTuples[tups]->getTupLength());
			} else {
				if (lastBlock) {		//如果满的这块是最后一块，则需要新建一块
					block->writeBack();						//先把之前放满的那块放入磁盘中,然后再新建一块
					int totalBlock = rel->getTotalBlock();
					block = new Block(totalBlock, rel, Dictionary::getDictionary()->getBlockSize());
					block->addTuple(updatedTuples[tups]->getResult(), updatedTuples[tups]->getTupLength());
					//放入LRU
					DBMS::getDBMSInst()->putBlock(rel->getRelationName(), block->getBlockId(), block);

					totalBlock += 1;
					rel->setTotalBlock(totalBlock);
					Dictionary::getDictionary()->setChange(true);
				} else {
					//如果满的这块不是最后一块，则先看最后一块是否还能放入，不能的话再新建一块
					int lastBlockId = rel->getTotalBlock() - 1;
					//Block * block = DBMS::getDBMSInst()->getBlock(words[1], *it);
					block = DBMS::getDBMSInst()->getBlock(rel->getRelationName(), lastBlockId);
					if (block == nullptr) {
						block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), lastBlockId);
						DBMS::getDBMSInst()->putBlock(rel->getRelationName(), lastBlockId, block);
					}
					lastBlock = true;	//标志以后放入的是放在最后一块中
					//如果最后一块也放不下了，需要新建一块
					if (block->getFreespace() > (int)updatedTuples[tups]->getTupLength()) {
						//能放下
						block->addTuple(updatedTuples[tups]->getResult(), updatedTuples[tups]->getTupLength());
					} else {
						//不能放下
						block->writeBack();						//先把之前放满的那块放入磁盘中,然后再新建一块
						int totalBlock = rel->getTotalBlock();
						block = new Block(totalBlock, rel, Dictionary::getDictionary()->getBlockSize());
						block->addTuple(updatedTuples[tups]->getResult(), updatedTuples[tups]->getTupLength());
						//放入LRU
						DBMS::getDBMSInst()->putBlock(rel->getRelationName(), block->getBlockId(), block);

						totalBlock += 1;
						rel->setTotalBlock(totalBlock);
						Dictionary::getDictionary()->setChange(true);
					}
				}
			}
		}
	}
	//删除tuples
	for (auto it = updatedTuples.begin(); it != updatedTuples.end(); it++) {
		delete (*it);
	}
	for (auto it = removedTuples.begin(); it != removedTuples.end(); it++) {
		delete (*it);
	}
}
int UpdateSql::checkIndex() {
	for (auto it = joins.begin(); it != joins.end(); it++) {
		if ((*it) != "and") {
			return -1;
		}
	}
	int index = findIndex();
	return index;
}
int UpdateSql::findIndex() {
	int index = -1;
	for (unsigned int i = 0; i < conditions.size(); i++) {
		if (Dictionary::getDictionary()->isIndex(conditions[i]->table, conditions[i]->left) && conditions[i]->symbol == "=") {
			index = i;
			break;
		}
	}
	return index;
}

int UpdateSql::isInExpres(int index) {
	int exp = -1;
	for (unsigned int i = 0; i < exprs.size(); i++) {
		if (exprs[i]->leftIndex == index) {
			exp = i;
			break;
		}
	}
	return exp;
}

bool UpdateSql::check(BasicType * left, int type, string symbol, string right) {
	bool flag = false;
	if (type == Global::INTEGER) {
		int * leftData = (int*)left->getData();
		int rightData;
		try {
			rightData = stoi(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'int\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::FLOAT) {
		float * leftData = (float*)left->getData();
		float rightData;
		try {
			rightData = stof(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'float\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::DOUBLE) {
		double * leftData = (double*)left->getData();
		double rightData;
		try {
			rightData = stod(right);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(right);
			error.append("\' to \'double\'");
			throw Error(error);
		}
		if (symbol == "=") {
			return (*leftData) == rightData;
		} else if (symbol == ">") {
			return (*leftData) > rightData;
		} else if (symbol == "<") {
			return (*leftData) < rightData;
		} else if (symbol == ">=") {
			return (*leftData) >= rightData;
		} else if (symbol == "<=") {
			return (*leftData) <= rightData;
		}
	} else if (type == Global::CHAR) {
		char * data = left->getData();
		string leftData(data);
		if (symbol == "=") {
			return leftData == right;
		} else if (symbol == ">") {
			return leftData > right;
		} else if (symbol == "<") {
			return leftData < right;
		} else if (symbol == ">=") {
			return leftData >= right;
		} else if (symbol == "<=") {
			return leftData <= right;
		}
	} else if (type == Global::VARCHAR) {
		char * data = left->getData();
		string leftData(data);
		if (symbol == "=") {
			return leftData == right;
		} else if (symbol == ">") {
			return leftData > right;
		} else if (symbol == "<") {
			return leftData < right;
		} else if (symbol == ">=") {
			return leftData >= right;
		} else if (symbol == "<=") {
			return leftData <= right;
		}
	}
	return flag;
}



















