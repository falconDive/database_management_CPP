#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include <vector>
#include <typeinfo>
#include <ctype.h>
#include "tokenizer.h"

class Node{
public:
	std::string data;
	std::string colname;
	int column_num;
	TokenType type;
	Node(std::string data, TokenType type, std::string colname) {
		this->colname = colname;
		this->data = data;
		this->type = type;
		this->column_num = -1;
	}
	Node(std::string data, TokenType type, int column_num) {
		this->colname = "\0";
		this->data = data;
		this->type = type;
		this->column_num = column_num;
	}
	std::string get_data() {
		return data;
	}
	void set_data(std::string data) {
		this->data = data;
	}
	std::string get_colname() {
		return colname;
	}
	void set_colname(std::string columname) {
		this->colname = columname;
	}
};

class Column {
public:
	TokenType type;
	std::string colname;
	bool isPrimaryKey;
	bool isNotNull;
	bool isForeignKey;
	std::string foreign_table;
	std::string foreign_column;
	bool isAutoIncrement;
	int next_increment;
	int char_size;
	Column(TokenType type, std::string name, bool isNotNull = false, bool isPrimary = false, bool isAutoIncrement = false) {
		this->next_increment = 1;
		this->type = type;
		this->colname = name;
		this->isPrimaryKey = isPrimary;
		this->isNotNull = isNotNull;
		this->isAutoIncrement = isAutoIncrement;
		char_size = 0;
		isForeignKey = false;
		foreign_table = "";
		foreign_column = "";
	}
};

class Row {
public:
	std::vector<Node> rowNodes;
	Row(std::vector<Node> &nodes) {
		for(Node &node : nodes) {
			rowNodes.push_back(node);
		}
	}
	bool updateRow(std::string colname, std::string data);
};

class Table
{
    friend void SaveTables();
    friend void RestoreTables();
    public:
        Table(std::string table_name, std::vector<Column> columns);
        void display();
        void displayColumns();
        bool AddRow(std::vector<Node> nodes);
        std::string get_Name();
        bool isValid;
        void clearData();
        Column get_Column(std::string name);
        std::vector<Row> *get_RowsPTR();
        std::vector<Row> &get_Rows();

    private:
        std::string name;
        int num_columns;
        int primary_key_column;
        std::vector<Column> columns;
        std::vector<Row> rows;
};

#endif // TABLE_H
