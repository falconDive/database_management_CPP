#include "Table.h"


std::string reserved[] = {"TEXT", "INT", "FLOAT", "CHAR", "DATE", "TIME"};


Table::Table(std::string table_name, std::vector<Column> columns) {
    name = table_name;
    isValid = false;
    bool has_Primary = false;
    for(int i=0;i<columns.size();i++) {
        Column column = columns[i];
        if(column.isPrimaryKey) {
            has_Primary = true;
            primary_key_column = i;
        }
    }

	if(!has_Primary) {
           std::cout<<"Table has no primary key\n";
           return;
	}
	for(Column &column : columns) {
		this->columns.push_back(column);
	}
	num_columns = this->columns.size();
	isValid = true;
}

std::string Table::get_Name() {
    return name;
}

void Table::clearData() {
    rows.clear();
}
void Table::display() {
    std::cout<<"Displaying Table : "<<name<<"\n";
	for(Column &column : this->columns) {
		std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<column.colname;
	}
	std::cout<<"\n";
	for(Row &row : this->rows) {
		for(Node &node : row.rowNodes) {
            std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<< node.get_data();
		}
	std::cout<<"\n";
	}
}

void Table::displayColumns() {
	for(Column &column : this->columns) {
        std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<column.colname;
	}
	std::cout<<"\n";
}
bool Table::AddRow(std::vector<Node> nodes) {

    // assign column number to each node
    for(Node &node : nodes) {
        if(node.column_num != -1) continue;
        for(int i=0;i<columns.size();i++) {
            Column column = columns[i];
            if(column.colname == node.colname) {
                node.column_num = i;
                if(i == primary_key_column) {
                    for(int j = 0;j<rows.size();j++) {
                        std::cout<<rows[j].rowNodes[primary_key_column].data<<" "<<node.data<<"\n";
                        if(rows[j].rowNodes[primary_key_column].data == node.data) return false;
                    }
                }
            }
        }
        if(node.column_num == -1) return false;
    }
    std::vector<Node> rnodes;
    for(int i=0;i<columns.size();i++) {
        bool found = false;
        for(int j=0;j<nodes.size();j++) {
            if(i == nodes[j].column_num) {
                if(!found) found = true;
                else return false;
                rnodes.push_back(nodes[j]);
            }
        }
        if(found && columns[i].isNotNull && columns[i].isAutoIncrement) return false;
        if(!found && columns[i].isNotNull && !columns[i].isAutoIncrement) return false;
        else if(!found && columns[i].isNotNull &&  columns[i].isAutoIncrement) {
            Node n(std::to_string(columns[i].next_increment++), columns[i].type, i);
            n.colname = columns[i].colname;
            rnodes.push_back(n);
        } else if(!found && !columns[i].isNotNull) {
            std::string def;
            switch(columns[i].type) {
                case TIME:
                case DATE:
                case INT:
                    def = "0";
                    break;
                case FLOAT:
                    def = "0.0";
                    break;
                case CHAR:
                case TEXT:
                    def = "NULL";
                    break;
                default:
                    return false;
                    break;
            }
            Node n(def, columns[i].type, i);
            n.colname = columns[i].colname;
            rnodes.push_back(n);
        }
    }
    Row r(rnodes);
    rows.push_back(r);
    return true;

}

Column Table::get_Column(std::string name) {
    for(Column &c : columns)
        if(c.colname == name) return c;

    Column c(ERROR, "ERROR");
    return c;
}

std::vector<Row> &Table::get_Rows() {
    return rows;
}

std::vector<Row> *Table::get_RowsPTR() {
    return &rows;
}
