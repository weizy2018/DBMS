/*
 * SelectSql.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: weizy
 */

#include "head/SelectSql.h"

#include "../head/Global.h"
#include "../head/DBMS.h"
#include "../head/Dictionary.h"
#include "../head/Block.h"
#include "../head/Tuple.h"

#include "../exception/head/SqlSyntaxException.h"
#include "../exception/head/Error.h"

#include "../tools/head/BPlusTree.h"
#include "../tools/head/BPlusTree2.h"
#include "../tools/head/lru.h"
#include "../tools/head/CalTime.h"

#include <string.h>
#include <iomanip>

#define TEMP_LRU_SIZE	1024


SelectSql::SelectSql(const vector<string> ws) : words(ws) {
	// TODO Auto-generated constructor stub

}

SelectSql::~SelectSql() {
	// TODO Auto-generated destructor stub
	for (auto it = conditions.begin(); it != conditions.end(); it++) {
		delete (*it);
	}
}
void SelectSql::execute() {
	if (words[1] == "database") {
		if (words[2] == "(" && words[3] ==")" && words[4] == ";") {
			unsigned int maxLen = 10;
			string currentDatabase = DBMS::getDBMSInst()->getCurrentDatabase();
			if (currentDatabase == "") {
				currentDatabase = "NULL";
			}
			if (currentDatabase.length() > maxLen) {
				maxLen = currentDatabase.length();
			}
			maxLen++;
			cout << setfill('-') << left << setw(maxLen+2) << "+";
			cout << "+" << endl;
			cout << "| ";
			cout << setfill(' ') << left << setw(maxLen) << "database()";
			cout << "|" << endl;
			cout << setfill('-') << left << setw(maxLen+2) << "+";
			cout << "+" << endl;
			cout << "| ";
			cout << setfill(' ') << left << setw(maxLen) << currentDatabase;
			cout << "|" << endl;
			cout << setfill('-') << left << setw(maxLen+2) << "+";
			cout << "+" << endl;
			return;
		}
	}

	if (DBMS::getDBMSInst()->getCurrentDatabase() == "") {
		throw Error("no database selected");
	}
	if (words[1] != "*" || words[2] != "from") {
		throw SqlSyntaxException("sql syntax error");
	}
	handleTables();
	checkTable();

	handleConditions();
	checkCondition();

	if (conditions.size() == 0) {
		if (tableNames.size() == 1) {
			selectAll();						//单表查询、无条件
		} else {
			selectAll2();						//双表查询、无条件
		}
	} else {
		if (tableNames.size() == 1) {
			int index = checkIndex();
			if (index != -1) {
				selectInIndex1(index);			//单表查询、有条件、使用索引
			} else {
				select1();						//单表查询、有条件、不使用索引
			}
		} else {
			select2();							//双表查询、有条件、在能用索引情况下使用索引加速查询
		}
	}

//	cout << "after check condition" << endl;
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		Condition * con = conditions[i];
//		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
//	}

}

void SelectSql::handleTables() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "from") {
		index++;
	}
	//没有from关键字
	if (index == words.size()) {
		throw SqlSyntaxException("sql syntax error");
	}
	index++;
	while (words[index] != ";" && words[index] != "where") {
		tableNames.push_back(words[index]);
		index++;
		if (words[index] == ",") {
			index++;
		} else if (words[index] == ";" || words[index] == "where") {
			break;
		}
	}
	if (tableNames.size() == 0) {
		throw SqlSyntaxException("sql syntax error");
	}

//	for (auto it = tableNames.begin(); it != tableNames.end(); it++) {
//		cout << *it << "  ";
//	}
//	cout << endl;
}
void SelectSql::handleConditions() {
	unsigned int index = 0;
	while (index < words.size() && words[index] != "where") {
		index++;
	}
	index++;
	if (index >= words.size()) {
		return;
	}

	//向join中加入一个"and"
	join.push_back("and");

	while (words[index] != ";") {
		string table1 = "";
		string column1 = "";
		string table2 = "";
		string column2 = "";
		string sym;

		//table.id = ... 类型
		if (index+1 < words.size() && words[index+1] == ".") {
			table1 = words[index];
			if (index+2 < words.size()) {
				column1 = words[index+2];
			} else {
				throw SqlSyntaxException("sql syntax exception");
			}
			index += 3;
		} else {				//id = ... 类型
			column1 = words[index];
			index += 1;
		}
		//符号 = 、> 、< 、>= 、<= 、由于之前的设置，这里没有!=
		if (words[index] == "=" || words[index] == ">" || words[index] == "<" || words[index] == ">=" || words[index] == "<=") {
			sym = words[index];
			index++;
		} else {
			throw SqlSyntaxException("sql syntax exception");
		}
		//... = table2.id类型
		if (index+1 < words.size() && words[index+1] == ".") {
			table2 = words[index];
			if (index+2 < words.size()) {
				column2 = words[index+2];
			} else {
				throw SqlSyntaxException("sql syntax exception");
			}
			index += 3;
		} else {			//... = id 或者 ... = "id" 或者 ... = 'id'  类型
			if (words[index] == "\"" || words[index] == "\'") {
				index++;
				column2 = words[index];
				index += 2;
			} else {
				column2 = words[index];
				index++;
			}
		}
		Condition * con = new Condition(table1, column1, table2, column2, sym);
		conditions.push_back(con);
		// and   or
		if (words[index] == "and" || words[index] == "or") {
			join.push_back(words[index]);
			index++;
		} else if (words[index] == ";") {
			break;
		} else {
			throw SqlSyntaxException("sql syntax exception");
		}
	}
//	for (unsigned int i = 0; i < conditions.size(); i++) {
//		Condition * con = conditions[i];
//		cout << con->table1 << " " << con->column1 << con->symbol << con->table2 << " " << con->column2 << endl;
//	}
}
//检查from的table是否在currentDatabase中
void SelectSql::checkTable() {
	const string currentDatabase = DBMS::getDBMSInst()->getCurrentDatabase();
	for (unsigned int i = 0; i < tableNames.size(); i++) {
		Relation * r = Dictionary::getDictionary()->getRelation(tableNames[i].c_str());
		if (r == nullptr) {
			string error("Table \'");
			error.append(currentDatabase);
			error.append(".");
			error.append(tableNames[i]);
			error.append("\' doesn't exist");
			throw Error(error);
		}
	}
}
//检查where中关系表以及对应的列是否合法
void SelectSql::checkCondition() {
	for (unsigned int i = 0; i < conditions.size(); i++) {
		Condition * con = conditions[i];

		//table1.column1
		if (con->table1 == "") {
			//从from的关系表中找到该列所在的关系表
			bool flag = false;
			Relation * r;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				r = Dictionary::getDictionary()->getRelation(tableNames[j].c_str());
				if (r->hasAttribute(con->column1)) {
					flag = true;
					break;
				}
			}
			//Unknown column 'student_name' in 'where clause'
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			} else {
				con->table1 = r->getRelationName();
			}
		} else {
			//需判断table1是否在tableNames中
			bool flag = false;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				if (con->table1 == tableNames.at(j)) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->table1);
				error.append(".");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
			//检查是否有对应的column
			Relation * rel = Dictionary::getDictionary()->getRelation(con->table1.c_str());
			if (!(rel->hasAttribute(con->column1))) {
				string error("Unknown column \'");
				error.append(con->table1);
				error.append(".");
				error.append(con->column1);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
		}

		//table2.column2 / value
		if (con->table2 == "") {
			//do nothing
		} else {
			bool flag = false;
			for (unsigned int j = 0; j < tableNames.size(); j++) {
				if (con->table2 == tableNames.at(j)) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				string error("Unknown column \'");
				error.append(con->table2);
				error.append(".");
				error.append(con->column2);
				error.append("\' in \'where clause\'");
				throw Error(error);
			}
		}
	}
	for (unsigned int i = 0; i < conditions.size(); i++) {
		if (conditions[i]->table1 != "") {
			Relation * rel1 = Dictionary::getDictionary()->getRelation(conditions[i]->table1.c_str());
			unsigned int column1Index = rel1->getAttributeIndex(conditions[i]->column1);
			conditions[i]->column1Index = column1Index;
		}
		if (conditions[i]->table2 != "") {
			Relation * rel2 = Dictionary::getDictionary()->getRelation(conditions[i]->table2.c_str());
			unsigned int column2Index = rel2->getAttributeIndex(conditions[i]->column2);
			conditions[i]->column2Index = column2Index;
		}
	}
}
//tableNames.size() == 1
//单表无条件查询
void SelectSql::selectAll() {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames.at(0).c_str());

	vector<Relation *> rels;
	rels.push_back(rel);
	printHead(rels);

	unsigned int totalBlock = rel->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock; i++) {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), i);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), i, block);
		}
		vector<Tuple *> tuples = block->getBlockTupls();
		for (unsigned int i = 0; i < tuples.size(); i++) {
			Tuple * tup = tuples[i];
			tup->printTuple();
			cout << endl;
			CalTime::getCalTimeInst()->addRow();
		}
		//释放内存
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			delete (*it);
		}
	}
//	int b = 0;
//	for (int i = 0; i < 2000000000; i++) {
//		b++;
//	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}
//tableNames.size() == 2	最多只能支持两个表联立了
//双表无条件查询
void SelectSql::selectAll2() {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	Relation * rel1 = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	Relation * rel2 = Dictionary::getDictionary()->getRelation(tableNames[1].c_str());

	vector<Relation *> rels;
	rels.push_back(rel1);
	rels.push_back(rel2);
	printHead(rels);

	unsigned int totalBlock1 = rel1->getTotalBlock();
	unsigned int totalBlock2 = rel2->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock1; i++) {
		Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], i);
		if (block1 == nullptr) {
			block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames[0], i, block1);
		}
		vector<Tuple *> tuples1 = block1->getBlockTupls();
		for (unsigned int j = 0; j < totalBlock2; j++) {
			Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], j);
			if (block2 == nullptr) {
				block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
				DBMS::getDBMSInst()->putBlock(tableNames[1], j, block2);
			}
			vector<Tuple *> tuples2 = block2->getBlockTupls();
			for (auto it1 = tuples1.begin(); it1 != tuples1.end(); it1++) {
				for (auto it2 = tuples2.begin(); it2 != tuples2.end(); it2++) {
					(*it1)->printTuple();
					(*it2)->printTuple();
					cout << endl;
					CalTime::getCalTimeInst()->addRow();
				}
			}
			for (auto it = tuples2.begin(); it != tuples2.end(); it++) {
				delete (*it);
			}
		}

		for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
			delete (*it);
		}
	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}
//条件语句中没有优先级之分(没有括号),同意从左到右依次运算
//单表有条件查询（没有使用索引）
void SelectSql::select1() {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());

	vector<Relation *> rels;
	rels.push_back(rel);
	printHead(rels);

	unsigned int totalBlock = rel->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock; i++)  {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), i);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), i, block);
		}
		vector<Tuple *> tuples = block->getBlockTupls();
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			bool flag = true;
			for (unsigned int k = 0; k < conditions.size(); k++) {
				BasicType * left = (*it)->getTupleBasicType(conditions[k]->column1Index);
				int type = rel->getTypeName(conditions[k]->column1Index);
				bool result = check(left, type, conditions[k]->symbol, conditions[k]->column2);
				if (join[k] == "or") {
					flag |= result;
				} else {
					flag &= result;
				}
			}
			if (flag) {
				(*it)->printTuple();
				cout << endl;
				CalTime::getCalTimeInst()->addRow();
			}

		}
		//释放内存
		for (auto it = tuples.begin(); it != tuples.end(); it++) {
			delete (*it);
		}
	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}
//select * from table1, table2 where table1.id = table2.id and table1.col1 > 100 and/or table2.col2 < 200;
//双表有条件查询（没有使用索引）
void SelectSql::select2() {
	//需要分两种情况，1）条件中含有or连接词的和不含有的
	bool flag = true;
	for (auto it = join.begin(); it != join.end(); it++) {
		if ((*it) == "or") {
			flag = false;
			break;
		}
	}
	if (flag)
		select2NoOr();
	else
		select2WithOr();
}
//双表有条件查询（没有使用索引）
//条件中含有or连接的没有任何技巧，从头到尾做自然连接，选择符合条件的输出
void SelectSql::select2WithOr() {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	Relation * rel1 = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	Relation * rel2 = Dictionary::getDictionary()->getRelation(tableNames[1].c_str());

	vector<Relation *> rels;
	rels.push_back(rel1);
	rels.push_back(rel2);
	printHead(rels);

	unsigned int totalBlock1 = rel1->getTotalBlock();
	unsigned int totalBlock2 = rel2->getTotalBlock();
	for (unsigned int i = 0; i < totalBlock1; i++) {
		Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], i);
		if (block1 == nullptr) {
			block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
			DBMS::getDBMSInst()->putBlock(tableNames[0], i, block1);
		}
		vector<Tuple *> tuples1 = block1->getBlockTupls();

		for (unsigned int j = 0; j < totalBlock2; j++) {
			Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], j);
			if (block2 == nullptr) {
				block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
				DBMS::getDBMSInst()->putBlock(tableNames[0], j, block2);
			}
			vector<Tuple *> tuples2 = block2->getBlockTupls();
			//条件选择
			for (unsigned int u = 0; u < tuples1.size(); u++) {
				for (unsigned int v = 0; v < tuples2.size(); v++) {
					bool flag = true;
					for (unsigned int k = 0; k < conditions.size(); k++) {
						bool result = false;
						int type;
						BasicType * left;
						if (conditions[k]->table2 == "") {					//table.name = "abc"类型
							if (conditions[k]->table1 == tableNames[0]) {	//table1的条件
								//BasicType * left = (*it)->getTupleBasicType(conditions[k]->column1Index);
								//int type = rel->getTypeName(conditions[k]->column1Index);
								left = tuples1[u]->getTupleBasicType(conditions[k]->column1Index);
								type = rel1->getTypeName(conditions[k]->column1Index);
								result = check(left, type, conditions[k]->symbol, conditions[k]->column2);
							} else {										//table2的条件
								left = tuples2[v]->getTupleBasicType(conditions[k]->column1Index);
								type = rel2->getTypeName(conditions[k]->column1Index);
								result = check(left, type, conditions[k]->symbol, conditions[k]->column2);
							}
						} else {											//table1.id = table2.id类型
							left = tuples1[u]->getTupleBasicType(conditions[k]->column1Index);
							BasicType * right = tuples2[v]->getTupleBasicType(conditions[k]->column2Index);
							type = rel1->getTypeName(conditions[k]->column1Index);
							result = check(left, type, conditions[k]->symbol, right->getData());
						}
						if (join[k] == "and") {
							flag &= result;
						} else {
							flag |= result;
						}
					}
					if (flag) {
						tuples1[u]->printTuple();
						tuples2[v]->printTuple();
						cout << endl;
						CalTime::getCalTimeInst()->addRow();
					}
				}
			}
			for (auto it = tuples2.begin(); it != tuples2.end(); it++) {
				delete (*it);
			}
		}
		for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
			delete (*it);
		}
	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}
//双表有条件查询（没有使用索引）
//where中都是用and连接起来的，可以先选择满足table2中的条件放入临时文件中，然后再做自然连接减少比较次数
void SelectSql::select2NoOr() {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	vector<Condition *> table1;
	vector<Condition *> table2;
	vector<Condition *> both;
	//将查询条件分类
	for (unsigned int i = 0; i < conditions.size(); i++) {
		if (conditions[i]->table1 == tableNames[0] && conditions[i]->table2 == "") {	//table1.column1 > 25类型
			table1.push_back(conditions[i]);
		} else if (conditions[i]->table1 == tableNames[1] && conditions[i]->table2 == "") { //table2.column2 > 4000类型
			table2.push_back(conditions[i]);
		} else {																		//table1.column1 = table2.column2类型
			both.push_back(conditions[i]);
		}
	}
	Relation * rel1 = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	Relation * rel2 = Dictionary::getDictionary()->getRelation(tableNames[1].c_str());

	vector<Relation *> rels;
	rels.push_back(rel1);
	rels.push_back(rel2);
	printHead(rels);

	unsigned int totalBlock1 = rel1->getTotalBlock();
	unsigned int totalBlock2 = rel2->getTotalBlock();
	if (table2.size() == 0) {
		/*
		 * 对于table2没有条件限定的情况
		 */
		int index = findIndex(table1);
		if (index != -1) {					//从table1中使用索引筛选满足table1的条件元组
			/*
			 * 对于table2没有条件限定的情况下，table1能够使用索引查询
			 */
			int type = rel1->getTypeName(table1[index]->column1Index);
			set<unsigned long int> blocksId;

			if (type == Global::CHAR || type == Global::VARCHAR) {
				BPlusTree<string, unsigned long int> * stringTree =
						Dictionary::getDictionary()->getStringIndex(table1[index]->table1, table1[index]->column1);
				blocksId = stringTree->get(table1[index]->column2);
			} else if (type == Global::INTEGER) {
				Bplustree<int, unsigned long int> * intTree =
						Dictionary::getDictionary()->getIntIndex(table1[index]->table1, table1[index]->column1);
				int key;
				try {
					key = stoi(table1[index]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table1[index]->column2);
					error.append("\' to \'int\'");
					throw Error(error);
				}
				blocksId = intTree->get(key);

			} else if (type == Global::FLOAT) {
				Bplustree<float, unsigned long int> * floatTree =
						Dictionary::getDictionary()->getFloatIndex(table1[index]->table1, table1[index]->column1);
				float key;
				try {
					key = stof(table1[index]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table1[index]->column2);
					error.append("\' to \'float\'");
					throw Error(error);
				}
				blocksId = floatTree->get(key);

			} else if (type == Global::DOUBLE) {
				Bplustree<double, unsigned long int> * doubleTree =
						Dictionary::getDictionary()->getDoubleIndex(table1[index]->table1, table1[index]->column1);
				double key;
				try {
					key = stod(table1[index]->column2);
 				} catch (invalid_argument & e) {
 					string error("Cannot convert \'");
 					error.append(table1[index]->column2);
 					error.append("\' to \'double\'");
 					throw Error(error);
 				}
 				blocksId = doubleTree->get(key);
			}
			for (auto it = blocksId.begin(); it != blocksId.end(); it++) {
				Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], *it);
				if (block1 == nullptr) {
					block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *it);
					DBMS::getDBMSInst()->putBlock(tableNames[0], *it, block1);
				}
				vector<Tuple *> tuples1 = block1->getBlockTupls();
				//选择满足table1条件的元组
				vector<Tuple *> tups1;
				for (auto tup = tuples1.begin(); tup != tuples1.end(); tup++) {
					bool flag = true;
					for (unsigned int k = 0; k < table1.size(); k++) {
						BasicType * left = (*tup)->getTupleBasicType(table1[k]->column1Index);
						int type = rel1->getTypeName(table1[k]->column1Index);
						flag = check(left, type, table1[k]->symbol, table1[k]->column2);
						if (!flag) {
							break;
						}
					}
					if (flag) {
						tups1.push_back(*tup);
					}
				}
				//table1与table2自然连接，选择满足both中的条件
				//这里table2没有限定条件
				for (unsigned int j = 0; j < totalBlock2; j++) {
					Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], j);
					if (block2 == nullptr) {
						block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
						DBMS::getDBMSInst()->putBlock(tableNames[1], j, block2);
					}
					vector<Tuple *> tups2 = block2->getBlockTupls();
					for (auto tup1 = tups1.begin(); tup1 != tups1.end(); tup1++) {
						for (auto tup2 = tups2.begin(); tup2 != tups2.end(); tup2++) {
							bool flag = true;
							for (unsigned int k = 0; k < both.size(); k++) {
								BasicType * left = (*tup1)->getTupleBasicType(both[k]->column1Index);
								BasicType * right = (*tup2)->getTupleBasicType(both[k]->column2Index);
								int type = rel1->getTypeName(both[k]->column1Index);
								flag = check(left, type, both[k]->symbol, right->getData());
								if (!flag) {
									break;
								}
							}
							if (flag) {
								(*tup1)->printTuple();
								(*tup2)->printTuple();
								cout << endl;
								CalTime::getCalTimeInst()->addRow();
							}
						}
					}
					for (auto it = tups2.begin(); it != tups2.end(); it++) {
						delete (*it);
					}
				}
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					delete (*it);
				}
			}
		} else {
			/*
			 * 对于table2没有跳线限定的情况下，table1没有索引进行查询
			 */
			for (unsigned int i = 0; i < totalBlock1; i++) {
				Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], i);
				if (block1 == nullptr) {
					block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
					DBMS::getDBMSInst()->putBlock(tableNames[0], i, block1);
				}
				vector<Tuple *> tuples1 = block1->getBlockTupls();
				//选择满足table1条件的元组
				vector<Tuple *> tups1;
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					bool flag = true;
					for (unsigned int k = 0; k < table1.size(); k++) {
						BasicType * left = (*it)->getTupleBasicType(table1[k]->column1Index);
						int type = rel1->getTypeName(table1[k]->column1Index);
						flag = check(left, type, table1[k]->symbol, table1[k]->column2);
						//由于都是用and连接的，所以只要有一个不满足条件就是false
						if (!flag) {
							break;
						}
					}
					if (flag) {
						tups1.push_back(*it);
					}
				}
				//table1与table2自然连接，选择满足both中的条件
				//这里table2没有限定条件
				for (unsigned int j = 0; j < totalBlock2; j++) {
					Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], j);
					if (block2 == nullptr) {
						block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
						DBMS::getDBMSInst()->putBlock(tableNames[1], j, block2);
					}
					vector<Tuple *> tups2 = block2->getBlockTupls();
					for (auto tup1 = tups1.begin(); tup1 != tups1.end(); tup1++) {
						for (auto tup2 = tups2.begin(); tup2 != tups2.end(); tup2++) {
							bool flag = true;
							for (unsigned int k = 0; k < both.size(); k++) {
								BasicType * left = (*tup1)->getTupleBasicType(both[k]->column1Index);
								BasicType * right = (*tup2)->getTupleBasicType(both[k]->column2Index);
								int type = rel1->getTypeName(both[k]->column1Index);
								flag = check(left, type, both[k]->symbol, right->getData());
								if (!flag) {
									break;
								}
							}
							if (flag) {
								(*tup1)->printTuple();
								(*tup2)->printTuple();
								cout << endl;
								CalTime::getCalTimeInst()->addRow();
							}
						}
					}
					for (auto it = tups2.begin(); it != tups2.end(); it++) {
						delete (*it);
					}
				}
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					delete (*it);
				}
			}
		}

	} else {
		/*
		 * 对于table2有条件限定
		 * 选出table2中符合的条件放入临时文件中，然后再进行自然连接
		 */
		unsigned int block2Cnt = 0;		//table2中满足table2限定条件的块数
		//Relation(unsigned int totalBlock, int totalProperty, char * relName, char * relFileName);
		char * relName = (char*)malloc(Global::MAX_RELATION_FILE_NAME);
		char * relFileName = (char*)malloc(Global::MAX_RELATION_FILE_NAME);
		int totalProperty = rel2->getTotalProperty();
		strcpy(relName, "temp");
		strcpy(relFileName, "temp.tm");
		//必须先新建一个临时文件temp.tm
		string url("data/");
		url.append(DBMS::getDBMSInst()->getCurrentDatabase());
		url.append("/");
		url.append(relFileName);
		FILE * f;
		if ((f = fopen(url.c_str(), "wb")) == NULL) {
			throw FileNotFoundException("can't open file : " + url);
		}
		fclose(f);
		Relation * tempRel = new Relation(block2Cnt, totalProperty, relName, relFileName);
		for (int i = 0; i < totalProperty; i++) {
			tempRel->addType(rel2->getTypeName(i), rel2->getTypeValue(i));
			tempRel->addAttribute(rel2->getAttribute(i).c_str());
		}

		Block * tempBlock = new Block(block2Cnt, tempRel, Dictionary::getDictionary()->getBlockSize());
		tempRel->setTotalBlock(1);
		//定义一个lru缓冲区便于加速查找
		LruCache<string, Block *> * tempLru = new LruCache<string, Block *>(TEMP_LRU_SIZE);

		/*
		 * table2中筛选满足条件的元组并放入临时文件中
		 */
		int index2 = findIndex(table2);
		if (index2 != -1) {
			/*
			 * table2能使用索引查询
			*/
			set<unsigned long int> blocks2Id;
			int type = rel2->getTypeName(table2[index2]->column1Index);
			if (type == Global::CHAR || type == Global::VARCHAR) {
				BPlusTree<string, unsigned long int> * stringTree =
						Dictionary::getDictionary()->getStringIndex(table2[index2]->table1, table2[index2]->column1);
				blocks2Id = stringTree->get(table2[index2]->column2);

			} else if (type == Global::INTEGER) {
				Bplustree<int, unsigned long int> * intTree =
						Dictionary::getDictionary()->getIntIndex(table2[index2]->table1, table2[index2]->column1);
				int key;
				try {
					key = stoi(table2[index2]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table2[index2]->column2);
					error.append("\' to \'int\'");
					throw Error(error);
				}
				blocks2Id = intTree->get(key);

			} else if (type == Global::FLOAT) {
				Bplustree<float, unsigned long int> * floatTree =
						Dictionary::getDictionary()->getFloatIndex(table2[index2]->table1, table2[index2]->column1);
				float key;
				try {
					key = stof(table2[index2]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table2[index2]->column2);
					error.append("\' to \'float\'");
					throw Error(error);
				}
				blocks2Id = floatTree->get(key);

			} else if (type == Global::DOUBLE) {
				Bplustree<double, unsigned long int> * doubleTree =
						Dictionary::getDictionary()->getDoubleIndex(table2[index2]->table1, table2[index2]->column1);
				double key;
				try {
					key = stod(table2[index2]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table2[index2]->column2);
					error.append("\' to \'double\'");
					throw Error(error);
				}
				blocks2Id = doubleTree->get(key);
			}
			//遍历通过索引所得到的块，筛选出满足条件的元组放入临时文件中
			for (auto it = blocks2Id.begin(); it != blocks2Id.end(); it++) {
				Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1], *it);
				if (block2 == nullptr) {
					block2 = rel2->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *it);
					DBMS::getDBMSInst()->putBlock(tableNames[1], *it, block2);
				}
				vector<Tuple *> tuples2 = block2->getBlockTupls();
				for (auto tup2 = tuples2.begin(); tup2 != tuples2.end(); tup2++) {
					bool flag = true;
					for (unsigned int k = 0; k < table2.size(); k++) {
						BasicType * left = (*tup2)->getTupleBasicType(table2[k]->column1Index);
						int type = rel2->getTypeName(table2[k]->column1Index);
						flag = check(left, type, table2[k]->symbol, table2[k]->column2);
						if (!flag) {
							break;
						}
					}
					if (flag) {
						if ((int)(*tup2)->getTupLength() > tempBlock->getFreespace()) {
							tempBlock->writeBack();
							string key(relName);
							key.append(to_string(block2Cnt));
							Block * b = tempLru->put(key, tempBlock);
							if (b) {
								delete b;
							}
							block2Cnt++;
							tempBlock = new Block(block2Cnt, tempRel, Dictionary::getDictionary()->getBlockSize());
							tempRel->setTotalBlock(block2Cnt+1);
						}
						tempBlock->addTuple((*tup2)->getResult(), (*tup2)->getTupLength());
					}
				}
				for (auto tup = tuples2.begin(); tup != tuples2.end(); tup++) {
					delete (*tup);
				}
			}
			tempBlock->writeBack();
			string key(relName);
			key.append(to_string(block2Cnt));
			tempLru->put(key, tempBlock);
			block2Cnt++;						//不用质疑、不用质疑、不用质疑
		} else {
			/*
			 * table2不能使用索引查找
			 */
			for (unsigned int i = 0; i < totalBlock2; i++) {
				Block * block2 = DBMS::getDBMSInst()->getBlock(tableNames[1],
						i);
				if (block2 == nullptr) {
					block2 = rel2->getBlock(
							DBMS::getDBMSInst()->getCurrentDatabase(), i);
					DBMS::getDBMSInst()->putBlock(tableNames[1], i, block2);
				}
				vector<Tuple *> tups2 = block2->getBlockTupls();
				for (auto it = tups2.begin(); it != tups2.end(); it++) {
					bool flag = true;
					for (unsigned int k = 0; k < table2.size(); k++) {
						BasicType * left = (*it)->getTupleBasicType(
								table2[k]->column1Index);
						int type = rel2->getTypeName(table2[k]->column1Index);
						flag = check(left, type, table2[k]->symbol,
								table2[k]->column2);
						if (!flag) {
							break;
						}
					}
					if (flag) {
						if ((int) (*it)->getTupLength()
								> tempBlock->getFreespace()) {
							tempBlock->writeBack();
							string key(relName);
							key.append(to_string(block2Cnt));
							Block * b = tempLru->put(key, tempBlock);
							if (b) {
								delete b;
							}
							block2Cnt++;
							tempBlock = new Block(block2Cnt, tempRel, Dictionary::getDictionary()->getBlockSize());
							tempRel->setTotalBlock(block2Cnt + 1);
						}
						tempBlock->addTuple((*it)->getResult(),
								(*it)->getTupLength());
					}
				}
				for (auto it = tups2.begin(); it != tups2.end(); it++) {
					delete (*it);
				}
			}
			tempBlock->writeBack();
			string key(relName);
			key.append(to_string(block2Cnt));
			tempLru->put(key, tempBlock);
			block2Cnt++;						//不用质疑、不用质疑、不用质疑
		}
		/*
		 * table1与刚刚产生的临时文件中的元组做自然连接
		 */
		int index1 = findIndex(table1);
		if (index1 != -1) {
			/*
			 * table1能使用索引查询
			 */
			set<unsigned long int> blocks1Id;
			int type = rel1->getTypeName(table1[index1]->column1Index);
			if (type == Global::CHAR || type == Global::VARCHAR) {
				BPlusTree<string, unsigned long int> * stringTree =
						Dictionary::getDictionary()->getStringIndex(table1[index1]->table1, table1[index1]->column1);
				blocks1Id = stringTree->get(table1[index1]->column2);

			} else if (type == Global::INTEGER) {
				Bplustree<int, unsigned long int> * intTree =
						Dictionary::getDictionary()->getIntIndex(table1[index1]->table1, table1[index1]->column1);
				int key;
				try {
					key = stoi(table1[index1]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table1[index1]->column2);
					error.append("\' to \'int\'");
					throw Error(error);
				}
				blocks1Id = intTree->get(key);

			} else if (type == Global::FLOAT) {
				Bplustree<float, unsigned long int> * floatTree =
						Dictionary::getDictionary()->getFloatIndex(table1[index1]->table1, table1[index1]->column1);
				float key;
				try {
					key = stof(table1[index1]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table1[index1]->column2);
					error.append("\' to \'float\'");
					throw Error(error);
				}
				blocks1Id = floatTree->get(key);

			} else if (type == Global::DOUBLE) {
				Bplustree<double, unsigned long int> * doubleTree =
						Dictionary::getDictionary()->getDoubleIndex(table1[index1]->table1, table1[index1]->column1);
				double key;
				try {
					key = stod(table1[index1]->column2);
				} catch (invalid_argument & e) {
					string error("Cannot convert \'");
					error.append(table1[index1]->column2);
					error.append("\' to \'double\'");
					throw Error(error);
				}
				blocks1Id = doubleTree->get(key);
			}
			for (auto id = blocks1Id.begin(); id != blocks1Id.end(); id++) {
				Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], *id);
				if (block1 == nullptr) {
					block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *id);
					DBMS::getDBMSInst()->putBlock(tableNames[0], *id, block1);
				}
				vector<Tuple *> tuples1 = block1->getBlockTupls();
				//选择满足table1条件的元组
				vector<Tuple *> tups1;
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					bool flag = true;
					for (unsigned int k = 0; k < table1.size(); k++) {
						BasicType * left = (*it)->getTupleBasicType(table1[k]->column1Index);
						int type = rel1->getTypeName(table1[k]->column1Index);
						flag = check(left, type, table1[k]->symbol, table1[k]->column2);
						if (!flag) {
							break;
						}
					}
					if (flag) {
						tups1.push_back(*it);
					}
				}
				for (unsigned int j = 0; j < block2Cnt; j++) {
					string key(relName);
					key.append(to_string(j));
					Block * block2 = nullptr;
					try {
						block2 = tempLru->get(key);
					} catch (exception & e) {
						block2 = tempRel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
					}
					vector<Tuple *> tups2 = block2->getBlockTupls();
					for (auto tup1 = tups1.begin(); tup1 != tups1.end(); tup1++) {
						for (auto tup2 = tups2.begin(); tup2 != tups2.end(); tup2++) {
							bool flag = true;
							for (unsigned int k = 0; k < both.size(); k++) {
								BasicType * left = (*tup1)->getTupleBasicType(both[k]->column1Index);
								int type = rel1->getTypeName(both[k]->column1Index);
								BasicType * right = (*tup2)->getTupleBasicType(both[k]->column2Index);
								flag = check(left, type, both[k]->symbol, right->getData());
								if (!flag) {
									break;
								}
							}
							if (flag) {
								(*tup1)->printTuple();
								(*tup2)->printTuple();
								cout << endl;
								CalTime::getCalTimeInst()->addRow();
							}
						}
					}
					for (auto it = tups2.begin(); it != tups2.end(); it++) {
						delete *it;
					}
				}
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					delete (*it);
				}
			}
		} else {
			/*
			 * table1不能使用索引查询
			 */
			for (unsigned int i = 0; i < totalBlock1; i++) {
				Block * block1 = DBMS::getDBMSInst()->getBlock(tableNames[0], i);
				if (block1 == nullptr) {
					block1 = rel1->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), i);
					DBMS::getDBMSInst()->putBlock(tableNames[0], i, block1);
				}
				vector<Tuple *> tuples1 = block1->getBlockTupls();
				//选择满足table1条件的元组
				vector<Tuple *> tups1;
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					bool flag = true;
					for (unsigned int k = 0; k < table1.size(); k++) {
						BasicType * left = (*it)->getTupleBasicType(table1[k]->column1Index);
						int type = rel1->getTypeName(table1[k]->column1Index);
						flag = check(left, type, table1[k]->symbol, table1[k]->column2);
						if (!flag) {
							break;
						}
					}
					if (flag) {
						tups1.push_back(*it);
					}
				}
				//从临时文件中获取元组
				for (unsigned int j = 0; j < block2Cnt; j++) {
					string key(relName);
					key.append(to_string(j));
					Block * block2 = nullptr;
					try {
						block2 = tempLru->get(key);
					} catch (exception & e) {
						block2 = tempRel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), j);
					}
					vector<Tuple *> tups2 = block2->getBlockTupls();
					for (auto tup1 = tups1.begin(); tup1 != tups1.end(); tup1++) {
						for (auto tup2 = tups2.begin(); tup2 != tups2.end(); tup2++) {
							bool flag = true;
							for (unsigned int k = 0; k < both.size(); k++) {
								BasicType * left = (*tup1)->getTupleBasicType(
										both[k]->column1Index);
								int type = rel1->getTypeName(
										both[k]->column1Index);
								BasicType * right = (*tup2)->getTupleBasicType(
										both[k]->column2Index);
								flag = check(left, type, both[k]->symbol,
										right->getData());
								if (!flag) {
									break;
								}
							}
							if (flag) {
								(*tup1)->printTuple();
								(*tup2)->printTuple();
								cout << endl;
								CalTime::getCalTimeInst()->addRow();
							}
						}
					}
					for (auto it = tups2.begin(); it != tups2.end(); it++) {
						delete *it;
					}
				}
				for (auto it = tuples1.begin(); it != tuples1.end(); it++) {
					delete *it;
				}
			}
		}

		delete tempLru;
		delete tempRel;	//relName和relFileName由tempRel释放
	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}

//单表有条件查询（使用索引）
//select * from student where id = '123' and name = 'zhangsan';
void SelectSql::selectInIndex1(int index) {
	CalTime::getCalTimeInst()->setStartTime();
	CalTime::getCalTimeInst()->resetRow();

	Relation * rel = Dictionary::getDictionary()->getRelation(tableNames[0].c_str());
	int type = rel->getTypeName(conditions[index]->column1Index);

	vector<Relation *> rels;
	rels.push_back(rel);
	printHead(rels);

	set<unsigned long int> blocksId;

	if (type == Global::CHAR || type == Global::VARCHAR) {
		BPlusTree<string, unsigned long int> * stringTree =
				Dictionary::getDictionary()->getStringIndex(conditions[index]->table1, conditions[index]->column1);
		blocksId = stringTree->get(conditions[index]->column2);
//		cout << "b tree" << endl;
//		stringTree->printTree();
	} else if (type == Global::INTEGER) {
		Bplustree<int, unsigned long int> * intTree =
				Dictionary::getDictionary()->getIntIndex(conditions[index]->table1, conditions[index]->column1);
		int key;
		try {
			key = stoi(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'int\'");
			throw Error(error);
		}
		blocksId = intTree->get(key);
	} else if (type == Global::FLOAT) {
		Bplustree<float, unsigned long int> * floatTree =
				Dictionary::getDictionary()->getFloatIndex(conditions[index]->table1, conditions[index]->column1);
		float key;
		try {
			key = stof(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'float\'");
			throw Error(error);
		}
		blocksId = floatTree->get(key);
	} else if (type == Global::DOUBLE) {
		Bplustree<double, unsigned long int> * doubleTree =
				Dictionary::getDictionary()->getDoubleIndex(conditions[index]->table1, conditions[index]->column1);
		double key;
		try {
			key = stod(conditions[index]->column2);
		} catch (invalid_argument & e) {
			string error("Cannot convert \'");
			error.append(conditions[index]->column2);
			error.append("\' to \'double\'");
			throw Error(error);
		}
		blocksId = doubleTree->get(key);
	}

//	cout << "blocksId.size() = " << blocksId.size() << endl;

	for (auto it = blocksId.begin(); it != blocksId.end(); it++) {
		Block * block = DBMS::getDBMSInst()->getBlock(tableNames.at(0), *it);
		if (block == nullptr) {
			block = rel->getBlock(DBMS::getDBMSInst()->getCurrentDatabase(), *it);
			DBMS::getDBMSInst()->putBlock(tableNames.at(0), *it, block);
		}

		vector<Tuple *> tuples = block->getBlockTupls();
		for (auto tup = tuples.begin(); tup != tuples.end(); tup++) {
			bool flag = true;
			for (unsigned int i = 0; i < conditions.size(); i++) {
				BasicType * left = (*tup)->getTupleBasicType(conditions[i]->column1Index);
				int type = rel->getTypeName(conditions[i]->column1Index);
				bool result = check(left, type, conditions[i]->symbol, conditions[i]->column2);
				if (join[i] == "or") {
					flag |= result;
				} else {
					flag &= result;
				}
			}
			if (flag) {
				(*tup)->printTuple();
				cout << endl;
				CalTime::getCalTimeInst()->addRow();
			}
		}
		//释放内存
		for (auto tup = tuples.begin(); tup != tuples.end(); tup++) {
			delete (*tup);
		}
	}
	printTail();
	CalTime::getCalTimeInst()->setEndTime();
	cout << CalTime::getCalTimeInst()->getRow() << " rows in set (";
	cout << fixed << setprecision(2) << CalTime::getCalTimeInst()->getTime() << " sec)" << endl;
	cout << endl;
}
//判断是否满足条件
bool SelectSql::check(BasicType * left, int type, string symbol, string right) {
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

//检查是否可以使用索引搜索（where 字句只有一个条件或者所有条件都是使用 and 连接起来的，并且所在列已经创建了索引)
//返回where字句中第几个条件下标，如果没有返回-1
//(单表查询下）
int SelectSql::checkIndex() {
	//先检查是否都是用and连接起来
	for (auto it = join.begin(); it != join.end(); it++) {
		if (*it != "and") {
			return -1;
		}
	}
	//检查condition中的table1.column1是否创建了索引 (有一个就得了)
	int index = findIndex(conditions);
	return index;
}

int SelectSql::findIndex(vector<Condition *> cons) {
	int index = -1;
	for (unsigned int i = 0; i < cons.size(); i++) {
		if (Dictionary::getDictionary()->isIndex(cons[i]->table1, cons[i]->column1) && cons[i]->symbol == "=") {
			index = i;
			break;
		}
	}
	return index;
}

void SelectSql::printHead(vector<Relation *> relations) {
	for (auto it = relations.begin(); it != relations.end(); it++) {
		int totalProperty = (*it)->getTotalProperty();
		for (int i = 0; i < totalProperty; i++) {
			string attr = (*it)->getAttribute(i);
			head.push_back(attr.length() + 1);
		}
	}
	for (auto it = head.begin(); it != head.end(); it++) {
		cout << setfill('-') << left << setw(*it + 2) << "+";
	}
	cout << "+" << endl;
	for (auto it = relations.begin(); it != relations.end(); it++) {
		int totalProperty = (*it)->getTotalProperty();
		for (int i = 0; i < totalProperty; i++) {
			string attr = (*it)->getAttribute(i);
			cout << "| ";
			cout << attr << " ";
		}
	}
	cout << "|" << endl;
	for (auto it = head.begin(); it != head.end(); it++) {
		cout << setfill('-') << left << setw(*it + 2) << "+";
	}
	cout << "+" << endl;
}
void SelectSql::printTail() {
	for (auto it = head.begin(); it != head.end(); it++) {
		cout << setfill('-') << left << setw(*it + 2) << "+";
	}
	cout << "+" << endl;
}

















