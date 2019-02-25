/*
 * DescSql.cpp
 *
 *  Created on: Feb 18, 2019
 *      Author: weizy
 */

#include "head/DescSql.h"
#include "../head/DBMS.h"
#include "../head/Dictionary.h"
#include "../head/Global.h"

#include "../exception/head/SqlSyntaxException.h"
#include "../exception/head/Error.h"

#include <iomanip>

DescSql::DescSql(const vector<string> ws): words(ws) {
	// TODO Auto-generated constructor stub

}

DescSql::~DescSql() {
	// TODO Auto-generated destructor stub
}

void DescSql::execute() {
	if (DBMS::getDBMSInst()->getCurrentDatabase() == "") {
		throw Error("no database selected");
	}
	if (words.size() != 3) {
		throw SqlSyntaxException("sql syntax exception");
	}
	Relation * rel = Dictionary::getDictionary()->getRelation(words[1].c_str());
	//Table 'sampdb.studen' doesn't exist
	if (rel == nullptr) {
		string error("Table \'");
		error.append(DBMS::getDBMSInst()->getCurrentDatabase());
		error.append(".");
		error.append(words[1]);
		error.append("\' doesn't exist");
		throw Error(error);
	}
	unsigned int maxFieldLen = 5;
	unsigned int maxTypeLen  = 13;
	unsigned int maxIndexNameLen = 5;
	int totalProperty = rel->getTotalProperty();
	for (int i = 0; i < totalProperty; i++) {
		string attr = rel->getAttribute(i);
		if (attr.length() > maxFieldLen) {
			maxFieldLen = attr.length();
		}
		string indexName = Dictionary::getDictionary()->getIndexName(rel->getRelationName(), attr);
		if (indexName.length() > maxIndexNameLen) {
			maxIndexNameLen = indexName.length();
		}
	}
	maxFieldLen += 1;
	maxIndexNameLen += 1;

	cout << setfill('-') << left << setw(maxFieldLen+2) << "+";
	cout << setfill('-') << left << setw(maxTypeLen+2) << "+";
	cout << setfill('-') << left << setw(maxIndexNameLen+2) << "+";
	cout << "+" << endl;

	cout << "| " << setfill(' ') << left << setw(maxFieldLen) << "Field";
	cout << "| " << setfill(' ') << left << setw(maxTypeLen) << "Type";
	cout << "| " << setfill(' ') << left << setw(maxIndexNameLen) << "Index";
	cout << "|" << endl;

	cout << setfill('-') << left << setw(maxFieldLen+2) << "+";
	cout << setfill('-') << left << setw(maxTypeLen+2) << "+";
	cout << setfill('-') << left << setw(maxIndexNameLen+2) << "+";
	cout << "+" << endl;

	for (int i = 0; i < totalProperty; i++) {
		cout << "| " << setfill(' ') << left << setw(maxFieldLen) << rel->getAttribute(i);
		string type;
		int typeName = rel->getTypeName(i);
		if (typeName == Global::INTEGER) {
			type = "int";
		}  else if (typeName == Global::FLOAT) {
			type = "float";
		} else if (typeName == Global::DOUBLE) {
			type = "double";
		} else if (typeName == Global::CHAR) {
			type = "char(";
			type.append(to_string(rel->getTypeValue(i)));
			type.append(")");
		} else if (typeName == Global::VARCHAR) {
			type = "varchar(";
			type.append(to_string(rel->getTypeValue(i)));
			type.append(")");
		}
		cout << "| " << setfill(' ') << left << setw(maxTypeLen) << type;

		string indexName = Dictionary::getDictionary()->getIndexName(rel->getRelationName(), rel->getAttribute(i));
		cout << "| " << setfill(' ') << left << setw(maxIndexNameLen) << indexName;

		cout << "|" << endl;
	}
	cout << setfill('-') << left << setw(maxFieldLen+2) << "+";
	cout << setfill('-') << left << setw(maxTypeLen+2) << "+";
	cout << setfill('-') << left << setw(maxIndexNameLen+2) << "+";
	cout << "+" << endl;
}








