/*
 * DropSql.cpp
 *
 *  Created on: Mar 3, 2019
 *      Author: weizy
 */

#include "head/DropSql.h"
#include "../head/Dictionary.h"
#include "../head/DBMS.h"
#include "../exception/head/Error.h"

DropSql::DropSql(const vector<string> ws):words(ws) {
	// TODO Auto-generated constructor stub

}

DropSql::~DropSql() {
	// TODO Auto-generated destructor stub
}

//drop index index_name on table_name ;
void DropSql::execute() {
	if (words[1] == "index") {
		if (words.size() != 6) {
			throw Error("sql syntax error");
		}
		if (words[3] != "on") {
			throw Error("sql syntax error");
		}
		string indexName(words[2]);
		string tableName(words[4]);
		//先检查tableName是否存在
		Relation * rel = Dictionary::getDictionary()->getRelation(tableName.c_str());
		if (rel == nullptr) {
			string error("Table \'");
			error.append(DBMS::getDBMSInst()->getCurrentDatabase());
			error.append(".");
			error.append(tableName);
			error.append("\' doesn't exist");
			throw Error(error);
		}
		//在检查在该tableName中是否含有相应的indexName
		int totalProperty = rel->getTotalProperty();
		int index;
		string colName;
		for (index = 0; index < totalProperty; index++) {
			colName = rel->getAttribute(index);
			if (indexName == Dictionary::getDictionary()->getIndexName(tableName, colName)) {
				break;
			}
		}
		if (index < totalProperty) {
			int type = rel->getTypeName(index);
			string key = tableName + "$" + colName;
			if (type == Global::INTEGER) {
				Dictionary::getDictionary()->delIntIndex(key);
			} else if (type == Global::FLOAT) {
				Dictionary::getDictionary()->delFloatIndex(key);
			} else if (type == Global::DOUBLE) {
				Dictionary::getDictionary()->delDoubleIndex(key);
			} else {
				Dictionary::getDictionary()->delStringIndex(key);
			}
		} else {
			string error("There is no index \'");
			error.append(indexName);
			error.append("\' in \'");
			error.append(DBMS::getDBMSInst()->getCurrentDatabase());
			error.append(".");
			error.append(tableName);
			error.append("\'");
			throw Error(error);
		}


	} else if (words[1] == "table") {

	}

}
