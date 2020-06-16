#include <iostream>
using namespace std;
#include "Table.h"
#include "tokenizer.h"
#include <fstream>


std::string TT2[] = {"EOL","CREATE", "TABLE", "COMMA", "STAR", "EQ", "LPAREN", "RPAREN", "PRIMARY KEY", "NOT NULL",
    "AUTO_INCREMENT", "SEMICOLON", "STRING_CONS", "TRUNCATE", "DROP", "WHERE", "DELETE", "FROM", "INSERT", "INTO", "VALUES",
     "SELECT", "TEXT_STR", "ERROR", "INT", "FLOAT", "CHAR", "TEXT", "DATE", "TIME", "AND", "UPDATE", "SET", "NOTEQ",
     "LTEQ", "LT", "GT", "GTEQ", "ORDER", "BY", "ASC", "DESC", "QUIT", "FOREIGN KEY", "REFERENCES"};

std::vector<Table> tables;

int monthDays[12] = {31, 28, 31, 30, 31, 30,
                        31, 31, 30, 31, 30, 31};

bool ParseInt(std::string input, int &out) {
    try {
        out = std::stoi(input);
        return true;
    }
    catch(...) {
        std::cout<<"Invalid value for type INT : "<<input<<"\n";
        return false;
    }
}
bool ParseFloat(std::string input, float &out) {
    try {
        out = std::stof(input);
        return true;
    }
    catch(...) {
        std::cout<<"Invalid value for type INT : "<<input<<"\n";
        return false;
    }
}

bool ParseDateString(std::string input, int &output) {
    int inp;
    if(!ParseInt(input, inp)) return false;
    output = 0;
    int day = inp % 100;
    int month = (inp / 100) % 100;
    int year  = inp  / 10000;
    if( day > 31 || month > 12 || year > 9999) return -1;


    int leap_years = (year / 4 - year / 100 + year / 400);
    output = year * 365 + leap_years + day;
    for (int i=0; i<month - 1; i++)
        output += monthDays[i];
    return true;
}
bool ParseTimeString(std::string input, int &output) {
    int val;
    if(!ParseInt(input, val)) return false;
    output = (val / 10000)*3600 + ((val/100)%100) * 60 + val%100;
    return true;
}

std::string getDateString(int input) {

    int years = input/365;
    int current_year = input%365;

    int leap_count = years / 4 - years / 100 + years / 400;
    current_year -= leap_count;

    bool isLeap = false;
    if(years % 4 == 0) {
        isLeap = true;
        if(years % 100 == 0)
            isLeap = false;
        if(years % 400 == 0)
            isLeap=true;
    }
    if(isLeap) current_year++;


    while(current_year <= 0) {
        current_year += 365;
        years -= 1;
    }
    int current_month = 0;
    while(current_year > monthDays[current_month]) {
        current_year -= monthDays[current_month];
        current_month++;
        if(isLeap && current_month == 1) current_year -= 1;
    }
    int days = current_year;
    return std::to_string(years * 10000 + (current_month+1) * 100 + days);
}

std::string getTimeString(int val) {
    return std::to_string((val / 3600)*10000 + ((val/60)%60) * 100 + val%60);
}

void printNode(Node &node) {
    if(node.type == DATE) {
        int in = std::stoi(node.data);
        std::string print = getDateString(in);
        std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<print;
        } else if(node.type == TIME) {
        int in = std::stoi(node.data);
        std::string print = getTimeString(in);
        std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<print;
        } else std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<node.data;

}

Token peek(Tokenizer &tzr) {
    Token t = tzr.GetToken();
    tzr.UngetToken(t);
    return t;
}

int ParseDropStmt(Tokenizer &tzr) {
    if((tzr.expect(DROP)).type == ERROR) return 0;
    if((tzr.expect(TABLE)).type == ERROR) return 0;
    std::string table_name;
    Token table = tzr.expect(STRING_CONS);
    if((tzr.expect(SEMICOLON)).type == ERROR) return 0;
    if(table.type == ERROR) return 0;
    table_name = table.name;
    for(unsigned int i=0;i<tables.size();i++) {
        if(tables[i].get_Name() == table_name)
        {
            tables.erase(tables.begin()+i, tables.begin()+i+1);
            return 1;
        }
    }
    std::cout<<"Table to be dropped does not exist\n";
    return 2;
}

int ParseTruncateStmt(Tokenizer &tzr) {
    if((tzr.expect(TRUNCATE)).type == ERROR) return 0;
    if((tzr.expect(TABLE)).type == ERROR) return 0;
    std::string table_name;
    Token table = tzr.expect(STRING_CONS);
    if((tzr.expect(SEMICOLON)).type == ERROR) return 0;
    if(table.type == ERROR) return 0;
    table_name = table.name;
    for(unsigned int i=0;i<tables.size();i++) {
        if(tables[i].get_Name() == table_name)
        {
            tables[i].clearData();
            return 1;
        }
    }
    std::cout<<"Table to be truncated does not exist\n";
    return 2;
}

int ParseCreateStmt(Tokenizer &tzr) {
    std::vector<Column> columns;
    if((tzr.expect(CREATE)).type == ERROR) return 0;
    if((tzr.expect(TABLE)).type == ERROR) return 0;
    std::string table_name;
    Token table = tzr.expect(STRING_CONS);
    if(table.type == ERROR) return 0;
    table_name = table.name;
    if((tzr.expect(LPAREN)).type == ERROR) return 0;
    bool hasPrimaryKey = false;
    while(true) {
        Token cname = tzr.GetToken();
        if(cname.type == SEMICOLON) break;
        if(cname.type == ERROR) return 0;
        if(cname.type == PRIMARY_KEY || cname.type == FOREIGN_KEY) tzr.UngetToken(cname);

        TokenType coltype = peek(tzr).type;
        if(coltype == INT || coltype == FLOAT || coltype == TEXT || coltype == DATE || coltype == TIME) {
            tzr.GetToken();
            bool isNotNull = false;
            bool isAutoIncrement = false;
            if(peek(tzr).type == NOT_NULL) {
                isNotNull = true;
                tzr.GetToken();
                if(peek(tzr).type == AUTO_INCREMENT) {
                    tzr.GetToken();
                    isAutoIncrement = true;
                    if(coltype != INT) {
                        std::cout<<"Only INT can be AUTO_INCREMENT\n";
                        return 2;
                    }
                }
            }
            Column c(coltype, cname.name, isNotNull, false, isAutoIncrement);
            columns.push_back(c);
        } else if(coltype == CHAR) {

            tzr.GetToken();
            int ch_size = 0;
            if((tzr.expect(LPAREN)).type == ERROR) return 0;
            Token csize = tzr.expect(STRING_CONS);
            if(csize.type == ERROR) return 0;
            std::string chsize = csize.name;
            bool isNotNull = false;
            if(!ParseInt(chsize, ch_size)) return 2;
            if((tzr.expect(RPAREN)).type == ERROR) return 0;
            if(peek(tzr).type == NOT_NULL) {
                isNotNull = true;
                tzr.GetToken();
                if(peek(tzr).type == AUTO_INCREMENT) {
                    std::cout<<"Only INT can be AUTO_INCREMENT\n";
                    return 2;
                }
            }
            Column c(coltype, cname.name, isNotNull, false, false);
            c.char_size = ch_size;
            columns.push_back(c);

        } else if(coltype == PRIMARY_KEY) {
            if(hasPrimaryKey) {
                std::cout<<"Cannot have multiple primary keys!\n";
                return 2;
            }
            hasPrimaryKey = true;
            tzr.GetToken();
            if((tzr.expect(LPAREN)).type == ERROR) return 0;
            Token tmp = tzr.expect(STRING_CONS);
            bool foundKey = false;
            if(tmp.type == ERROR) return 0;
            else {
                for(Column &col : columns) {
                    if(col.colname == tmp.name) {
                        col.isPrimaryKey = true;
                        foundKey = true;
                    }
                }
            }
            if(!foundKey) {
                std::cout<<"Column marked as primary key is not present\n";
                return 2;
            }
            if((tzr.expect(RPAREN)).type == ERROR) return 0;

        } else if(coltype == FOREIGN_KEY) {
            tzr.GetToken();
            std::string reftable_name;
            std::string colname;
            std::string refcolname;
            TokenType refcoltype;

            if((tzr.expect(LPAREN)).type == ERROR) return 0;

            Token t = tzr.GetToken();
            if(t.type != STRING_CONS) return 0;
            colname = t.name;

            if((tzr.expect(RPAREN)).type == ERROR) return 0;
            if((tzr.expect(REFERENCES)).type == ERROR) return 0;

            t = tzr.GetToken();
            if(t.type != STRING_CONS) return 0;
            reftable_name = t.name;

            if((tzr.expect(LPAREN)).type == ERROR) return 0;

            t = tzr.GetToken();
            if(t.type != STRING_CONS) return 0;
            refcolname = t.name;

            if((tzr.expect(RPAREN)).type == ERROR) {
                    return 0;
            }

            bool reftable_exists = false;
            for(Table &table : tables) {
                if(table.get_Name() == reftable_name) {
                    reftable_exists = true;
                    refcoltype = table.get_Column(refcolname).type;
                    if(refcoltype == ERROR) {
                        std::cout<<"Referenced column does not exist or it does not match type of Foreign Key column\n";
                        return 2;
                    }
                }
            }
            if(!reftable_exists) {
                std::cout<<"Referenced table does not exist\n";
                    return 2;
            }

            bool colexists = false;
            for(Column &col : columns) {
                if(col.colname == colname && col.type == refcoltype) {
                    colexists = true;
                    col.isForeignKey = true;
                    col.foreign_column  = refcolname;
                    col.foreign_table = reftable_name;
                }
            }
            if(!colexists) {
                    std::cout<<"Foreign key column does not exist or it does not match type of reference column\n";
                    return 2;
            }
        } else return 0;

        if(peek(tzr).type == COMMA) {
            tzr.GetToken();
            continue;
        }
        else if(peek(tzr).type == RPAREN) {
            tzr.GetToken();
            if(peek(tzr).type == SEMICOLON) {
                tzr.GetToken();
                break;
            }
            return 0;
        }
        else return 0;

    }
    if(!hasPrimaryKey) {
        std::cout<<"No primary key in table\n";
        return 2;
    }
    Table t(table_name, columns);
    for(Table &table : tables) {
        if(table.get_Name() == t.get_Name()) {
                std::cout<<"Table already exists\n";
                return 2;
        }
    }
    if(t.isValid) tables.push_back(t);
    else {
        std::cout<<"Table is not valid. Please check input\n";
        return 2;
    }
    return 1;
}

int ParseInsertStmt(Tokenizer &tzr) {
    if((tzr.expect(INSERT)).type == ERROR) return 0;
    if((tzr.expect(INTO)).type == ERROR) return 0;
    Token table = tzr.expect(STRING_CONS);
    if(table.type == ERROR) return 0;
    std::string table_name = table.name;
    if((tzr.expect(LPAREN)).type == ERROR) return 0;
    std::vector<Token> colnames;
    while(true) {
        Token tmp = tzr.expect(STRING_CONS);
        if(tmp.type == ERROR) return 0;
        else {
            for(Token &c : colnames) {
                if(tmp.name == c.name) {
                    std::cout<<"Cannot INSERT value of same column twice\n";
                    return 2;
                }
            }
            colnames.push_back(tmp);
        }

        tmp = peek(tzr);
        if(tmp.type == RPAREN) break;
        else if(tmp.type == COMMA) {
            tzr.GetToken();
            continue;
        } else return 0;
    }
    if((tzr.expect(RPAREN)).type == ERROR) return 0;
    if((tzr.expect(VALUES)).type == ERROR) return 0;
    if((tzr.expect(LPAREN)).type == ERROR) return 0;
    std::vector<Token> values;
    while(true) {
        Token tmp = peek(tzr);
        if(tmp.type == TEXT_STR || tmp.type == STRING_CONS) {
            tzr.GetToken();
            values.push_back(tmp);
        } else return 0;
        tmp = peek(tzr);
        if(tmp.type == RPAREN) break;
        else if(tmp.type == COMMA) {
            tzr.GetToken();
            continue;
        } else return 0;
    }
    if((tzr.expect(RPAREN)).type == ERROR) return 0;
    if((tzr.expect(SEMICOLON)).type == ERROR) return 0;

    if(values.size() != colnames.size()) {
        std::cout<<"Number of columns listed and number of values listed are not equal\n";
    }

    Table *current_table;
    bool exists = false;
    for(Table &t : tables) {
        if(t.get_Name() == table_name) {
            exists = true;
            current_table = &t;
            break;
        }
    }
    if(!exists) {
        std::cout<<"Table to be inserted into does not exist\n";
    }

    std::vector<Node> nodes;
    std::vector<Column> columns;
    for(int i=0;i<colnames.size();i++) {
        Token colname = colnames[i];
        Column c = current_table->get_Column(colname.name);
        if(c.type == ERROR) {
            std::cout<<"Column "<<colname.name<<" not present in table \n";
            return 2;
        }
        if(c.isAutoIncrement) {
            std::cout<<"Cannot INSERT value to AUTO_INCREMENT column : "<<colname.name<<"\n";
            return 2;
        }
        if(c.type == TEXT && values[i].type != TEXT_STR) {
            std::cout<<"Data of type TEXT should be enclosed by \"\"\n ";
            return 2;
        }
        columns.push_back(c);
        Node n(values[i].name, c.type, c.colname);
        nodes.push_back(n);
    }

    for(int i=0;i<columns.size();i++) {
        Column column = columns[i];
        if(column.isForeignKey) {
            exists = false;
            Table *foreign_table;
            for(Table &t : tables) {
                if(t.get_Name() == column.foreign_table) {
                    foreign_table = &t;
                    exists = true;
                    break;
                }
            }
            if(!exists) {
                std::cout<<"Foreign table referenced by column does not exist\n";
                return 2;
            }
            std::vector<Row> rows;
            rows = foreign_table->get_Rows();
            int colnum = -1;
            for(int i=0;i<rows[0].rowNodes.size();i++) {
                if(column.foreign_column == rows[0].rowNodes[i].colname) {
                        colnum = rows[0].rowNodes[0].column_num;
                        break;
                }
            }
            if(colnum == -1) {
                std::cout<<"Column referenced in foreign table does not exist\n";
                return 2;
            }
            exists = false;
            for(Row &r : rows) {
                if(r.rowNodes[colnum].data == nodes[i].data) {
                    exists = true;
                    break;
                }
            }
            if(!exists) {
                std::cout<<"Data being added in FOREIGN KEY column does not exist in referenced table\n";
                return 2;
            }
        }
    }

    for(int i=0;i<nodes.size();i++) {
        int output;
        float tmp;
        switch(nodes[i].type) {
            case INT:
                if(!ParseInt(nodes[i].data, output)) {
                    std::cout<<"Data entered into column "<<nodes[i].colname<<" should be of type INT\n";
                    return 2;
                }
                break;
            case FLOAT:
                if(!ParseFloat(nodes[i].data, tmp)) {
                    std::cout<<"Data entered into column "<<nodes[i].colname<<" should be of type FLOAT\n";
                    return 2;
                }
                break;
            case TEXT:
                break;
            case CHAR:
                if(nodes[i].data.size() > (unsigned int)columns[i].char_size) {
                    std::cout<<"Size of char exceeds specified char size. Truncating to allowed size\n";
                    nodes[i].data = nodes[i].data.substr(0, (unsigned int)columns[i].char_size-1);
                }
                break;
            case DATE:
                if(!ParseDateString(nodes[i].data, output)) {
                    std::cout<<"Entered data for DATE type columns should be YYYYMMDD format in numeric values\n";
                    return 2;
                }
                nodes[i].data = std::to_string(output);
                break;
            case TIME:
                if(!ParseTimeString(nodes[i].data, output)) {
                    std::cout<<"Entered data for TIME type columns should be HHMMSS format in numeric values\n";
                    return 2;
                }
                nodes[i].data = std::to_string(output);
                break;
            default:
                return 0;
                break;
        }
    }
    bool ret = current_table->AddRow(nodes);
    if(ret) return 1;
    else {
        std::cout<<"Error while adding data to table\n";
        return 2;
    }
}


int parseEQ(Token lhs, Token rhs, std::vector<Row> rows, std::vector<int> &selected) {
    for(unsigned int i=0;i<rows.size();i++) {
        std::vector<Node> rnodes = rows[i].rowNodes;
        for(unsigned int j=0;j<rnodes.size();j++) {
            Node n = rnodes[j];
            std::string compare_with = rhs.name;
            if(n.type == DATE) {
                int tmp;
                if(!ParseDateString(rhs.name, tmp)) return 2;
                compare_with = std::to_string(tmp);
            } else if(n.type == TIME) {
                int tmp;
                if(!ParseTimeString(rhs.name, tmp)) return 2;
                compare_with = std::to_string(tmp);
            }
            if(n.colname == lhs.name && n.data == compare_with) {
                if(n.type == TEXT && rhs.type != TEXT_STR) return 0;
                if(n.type != TEXT && rhs.type == TEXT_STR) return 0;
                selected.push_back(i);
            }
        }
    }
    return 1;
}

int parseIneq(Token lhs, Token rhs, std::vector<Row> rows, std::vector<int> &selected, TokenType type) {
    int colnum = -1;
    for(unsigned int i=0;i<rows[0].rowNodes.size();i++) {
        std::vector<Node> rnodes = rows[0].rowNodes;
        if(rnodes[i].colname == lhs.name) colnum = i;
    }
    if(colnum == -1) return false;
    for(unsigned int i=0;i<rows.size();i++) {
        std::vector<Node> rnodes = rows[i].rowNodes;
        if(rnodes[colnum].type == FLOAT) {
            float v1, v2;
            if(!ParseFloat(rnodes[colnum].data, v1)) return 2;
            if(!ParseFloat(rhs.name, v2)) return 2;
            switch(type) {
                case LTEQ:
                    if(v1 <= v2) selected.push_back(i);
                    break;
                case LT:
                    if(v1 < v2) selected.push_back(i);
                    break;
                case GTEQ:
                    if(v1 >= v2) selected.push_back(i);
                    break;
                case GT:
                    if(v1 > v2) selected.push_back(i);
                    break;
                default:
                    cout<<"Invalid comparison operator\n";
                    return 2;
                    break;
            }
        } else if(rnodes[colnum].type == INT || rnodes[colnum].type == DATE || rnodes[colnum].type == TIME) {
            int v1, v2;
            switch(rnodes[colnum].type) {
                case INT:
                    if(!ParseInt(rnodes[colnum].data, v1)) return 2;
                    if(!ParseInt(rhs.name, v2)) return 2;
                    break;
                case DATE:
                    if(!ParseInt(rnodes[colnum].data, v1)) return 2;
                    if(!ParseDateString(rhs.name, v2)) return 2;
                    break;
                case TIME:
                    if(!ParseInt(rnodes[colnum].data, v1)) return 2;
                    if(!ParseTimeString(rhs.name, v2)) return 2;
                    break;
                default:
                    return 0;
            }
            switch(type) {
                case LTEQ:
                    if(v1 <= v2) selected.push_back(i);
                    break;
                case LT:
                    if(v1 < v2) selected.push_back(i);
                    break;
                case GTEQ:
                    if(v1 >= v2) selected.push_back(i);
                    break;
                case GT:
                    if(v1 > v2) selected.push_back(i);
                    break;
                default:
                    cout<<"Invalid comparison operator\n";
                    return 2;
                    break;
            }
        } else return 0;
    }
    return 1;
}

int parseWhere(Tokenizer &tzr, std::string table_name, std::vector<int> &result) {
    std::vector<Row> rows;
    Table *table = nullptr;
    for(Table &t : tables) {
        if(t.get_Name() == table_name) {
            table = &t;
        }
    }
    if(table == nullptr) return 0;
    rows = table->get_Rows();

    Token val = tzr.expect(STRING_CONS);
    if(val.type == ERROR) return 0;
    std::string value = val.name;
    Token op = peek(tzr);

    Token rhs;
    unsigned int num_rows;
    std::vector<int> res, res2;
    Token Between1, Between2;
    int r;
    switch(op.type) {
        case EQ:
            tzr.GetToken();
            rhs = peek(tzr);
            if(rhs.type != STRING_CONS && rhs.type != TEXT_STR) return 0;
            tzr.GetToken();
            if(rhs.type == ERROR) return 0;
            parseEQ(val, rhs, rows, result);
            break;
        case NOTEQ:
            tzr.GetToken();
            rhs = peek(tzr);
            if(rhs.type != STRING_CONS && rhs.type != TEXT_STR) return 0;
            tzr.GetToken();
            if(rhs.type == ERROR) return 0;
            parseEQ(val, rhs, rows, res);
            num_rows = table->get_RowsPTR()->size();
            for(unsigned int i=0;i<num_rows;i++) {
                bool found = false;
                for(int j : res)
                {
                    if(i==(unsigned int)j) {
                        found = true;
                        break;
                    }
                }
                if(!found) result.push_back(i);
            }
            break;
        case LTEQ:
        case LT:
        case GTEQ:
        case GT:
            tzr.GetToken();
            rhs = peek(tzr);
            if(rhs.type != STRING_CONS) return 0;
            tzr.GetToken();
            if(rhs.type == ERROR) return 0;
            if(!parseIneq(val, rhs, rows, result, op.type)) return 0;
            break;
        case BETWEEN:
            tzr.GetToken();
            Between1 = peek(tzr);
            if(Between1.type != STRING_CONS) return 0;
            tzr.GetToken();
            if(tzr.expect(AND).type == ERROR) return 0;
            Between2 = peek(tzr);
            if(Between2.type != STRING_CONS) return 0;
            tzr.GetToken();
            r = parseIneq(val, Between1, rows, res, GTEQ);
            if(r!= 1) return r;
            r = parseIneq(val, Between2, rows, res2, LT);
            if(r!= 1) return r;
            std::sort(res.begin(), res.end());
            std::sort(res2.begin(), res2.end());
            std::set_intersection(res.begin(), res.end(), res2.begin(), res2.end(), back_inserter(result));
        default:
            return false;
            break;
    }
    Token t = peek(tzr);
    if(t.type == SEMICOLON || t.type == ORDER)
        return true;
    else if(t.type == AND) {
        tzr.GetToken();
        std::vector<int> result2;
        int r = parseWhere(tzr, table_name, result2);
        if(r!=1) return r;
        std::sort(result2.begin(), result2.end());
        std::vector<int> result1_temp = result;
        result.clear();
        std::sort(result1_temp.begin(), result1_temp.end());
        std::set_intersection(result1_temp.begin(), result1_temp.end(), result2.begin(), result2.end(), back_inserter(result));
        return true;
    }
    else return false;


    return false;
}

int ParseDeleteStmt(Tokenizer &tzr) {
    if((tzr.expect(DELETE)).type == ERROR) return 0;
    if((tzr.expect(FROM)).type == ERROR) return 0;
    Token table = tzr.expect(STRING_CONS);
    if(table.type == ERROR) return 0;
    std::string table_name = table.name;
    Token t = peek(tzr);
    if(t.type == WHERE) {
        tzr.GetToken();
        std::vector<int> result;
        int r = parseWhere(tzr, table_name, result);
        if(r != 1) return r;
        if(tzr.expect(SEMICOLON).type == ERROR) return 0;

        std::vector<Row> *rows;
        for(Table &t : tables) {
            if(t.get_Name() == table_name) {
                rows = t.get_RowsPTR();
            }
        }
        std::sort(result.begin(), result.end());
        for(int i=result.size()-1;i>=0;i--) {
            rows->erase(rows->begin()+result[i], rows->begin()+result[i]+1);
        }

    } else if(t.type == SEMICOLON) {
        tzr.GetToken();
        for(Table &table : tables) {
            if(table.get_Name() == table_name) table.clearData();
        }
    } else return 0;

    return 1;

}
bool ParseUpdateStmt(Tokenizer &tzr) {
    std::vector<std::vector<Token>> set_list;
    if((tzr.expect(UPDATE)).type == ERROR) return 0;
    Token table = tzr.expect(STRING_CONS);
    if(table.type == ERROR) return false;
    std::string table_name = table.name;
    if((tzr.expect(SET)).type == ERROR) return 0;
    while(true) {
        std::vector<Token> arr;
        Token set1 = tzr.expect(STRING_CONS);
        if(set1.type == ERROR) return 0;
        // name of column
        arr.push_back(set1);

        if((tzr.expect(EQ)).type == ERROR) return 0;
        Token t = peek(tzr);
        if(t.type != STRING_CONS && t.type != TEXT_STR) return 0;
        tzr.GetToken();
        // value to set to
        arr.push_back(t);
        set_list.push_back(arr);
        if(peek(tzr).type == COMMA) tzr.GetToken();
        else break;
    }
    std::vector<int> result;
    std::vector<Row> *rows;
    bool exists = false;
    Table *ct;
    for(Table &t : tables) {
        if(t.get_Name() == table_name) {
            rows = t.get_RowsPTR();
            ct = &t;
            exists = true;
        }
    }
    if(!exists) {
        std::cout<<"Table doesn't exist\n";
        return 2;
    }
    std::cout<<"REACHED 2\n";
    if(peek(tzr).type == WHERE) {
        tzr.GetToken();
        int r = parseWhere(tzr, table_name, result);
        std::cout<<"Result of where : "<<r<<"\n";
        if(r != 1) return r;
        if((tzr.expect(SEMICOLON)).type == ERROR) return 0;
    } else if(peek(tzr).type == SEMICOLON) {
        for(int i=0;i<(int)rows->size();i++)
            result.push_back(i);
    }
    std::sort(result.begin(), result.end());
    std::cout<<"REACHED\n";
    for(unsigned int j=0;j<set_list.size();j++) {
        Column c = ct->get_Column((set_list[j])[0].name);
        if(c.isAutoIncrement && c.isNotNull) {
            std::cout<<"Cannot update AUTO_INCREMENT columns\n";
            return 2;
        }
        TokenType type = c.type;
        Token value = (set_list[j])[1];
        if(type == TEXT && value.type != TEXT_STR) {
            std::cout<<"For columns of type TEXT, strings should be enclosed by \"\" ";
            return 2;
        }
        if(type != TEXT && value.type != STRING_CONS) return 0;
        int output;
        float tmp;
        switch(type) {
            case INT:
                if(!ParseInt(value.name, output)) {
                    std::cout<<"Data entered into column "<<c.colname<<" should be of type INT\n";
                    return 2;
                }
                break;
            case FLOAT:
                if(!ParseFloat(value.name, tmp)) {
                    std::cout<<"Data entered into column "<<c.colname<<" should be of type FLOAT\n";
                    return 2;
                }
                break;
            case TEXT:
                break;
            case CHAR:
                if(value.name.size() > (unsigned int)c.char_size) {
                    std::cout<<"Size of char exceeds specified char size. Truncating to allowed size\n";
                    value.name = value.name.substr(0, (unsigned int)c.char_size-1);
                }
                break;
            case DATE:
                if(!ParseDateString(value.name, output)) {
                    std::cout<<"Entered data for DATE type columns should be YYYYMMDD format in numeric values\n";
                    return 2;
                }
                value.name = std::to_string(output);
                break;
            case TIME:
                if(!ParseTimeString(value.name, output)) {
                    std::cout<<"Entered data for TIME type columns should be HHMMSS format in numeric values\n";
                    return 2;
                }
                value.name = std::to_string(output);
                break;
            default:
                return 0;
                break;
        }
    }
    for(const int &i : result) {
        for(Node &node : (*rows)[i].rowNodes) {
            for(const std::vector<Token> &tokens : set_list) {
                Token col = tokens[0];
                Token val = tokens[1];
                if(col.name == node.colname) node.data = val.name;
            }
        }
    }
    return 1;

}

bool ParseOrder(Tokenizer &tzr, std::vector<int> &result, std::string table_name) {
    tzr.GetToken();
    if((tzr.expect(BY)).type == ERROR) return false;
    Token order_by = tzr.expect(STRING_CONS);
    if(order_by.type == ERROR) return false;
    std::string order = order_by.name;
    Token dir = tzr.GetToken();
    if(dir.type != ASC && dir.type != DESC) return false;
    std::vector<Row> *rows;
    for(Table &t : tables) {
        if(t.get_Name() == table_name) {
            rows = t.get_RowsPTR();
        }
    }
    int colnum = -1;
    for(unsigned int i=0;i<(*rows)[0].rowNodes.size();i++) {
        if((*rows)[0].rowNodes[i].colname == order) colnum = (int)i;
    }
    if(colnum == -1) return false;
    std::vector<int> tmp_result = result;
    result.clear();
    unsigned int s = tmp_result.size();
    for(unsigned int i=0;i<s;i++)
    {
        unsigned int low = 0;
        for(unsigned int j=1;j<tmp_result.size();j++) {
            const Node n = (*rows)[tmp_result[low]].rowNodes[colnum];
            const Node n2 = (*rows)[tmp_result[j]].rowNodes[colnum];
            switch(n.type) {
                case CHAR:
                case TEXT:
                    if(strcmp(n.data.c_str(), n2.data.c_str()) > 0) low = j;
                    break;
                case INT:
                case DATE:
                case TIME:
                    if(std::stoi(n.data) > std::stoi(n2.data)) low = j;
                    break;
                case FLOAT:
                    if(std::stof(n.data) > std::stof(n2.data)) low = j;
                    break;
                default:
                    return false;
            }
        }
        result.push_back(tmp_result[low]);
        tmp_result.erase(tmp_result.begin()+low, tmp_result.begin()+low+1);

    }
    if(dir.type == DESC) std::reverse(result.begin(), result.end());
    return true;
}

int ParseSelectStmt(Tokenizer &tzr) {
    std::vector<Token> show_list;
    if((tzr.expect(SELECT)).type == ERROR) return 0;
    Token t = peek(tzr);
    if(t.type == STAR) {
        tzr.GetToken();
        show_list.push_back(t);
    } else if(t.type == STRING_CONS) {
        bool cont = true;
        while(cont) {
            Token s = tzr.expect(STRING_CONS);
            show_list.push_back(s);
            if(t.type == ERROR) return 0;
            switch(peek(tzr).type) {
                case COMMA:
                    tzr.GetToken();
                    break;
                case FROM:
                    cont = 0;
                    break;
                default:
                    return 0;
            }
        }
    }
    if((tzr.expect(FROM)).type == ERROR) return 0;
    Token table = tzr.expect(STRING_CONS);
    if(table.type == ERROR) return 0;

    std::string table_name = table.name;
    std::vector<int>result;
    std::vector<Row> rows;
    Table *ct = nullptr;
    for(Table &t : tables) {
        if(t.get_Name() == table_name) {
            rows = t.get_Rows();
            ct = &t;
        }
    }

    if(ct == nullptr) {
        std::cout<<"Table does not exist\n";
        return 2;
    }
    int r;
    switch(peek(tzr).type) {
        case WHERE:
            tzr.GetToken();
            r = parseWhere(tzr, table_name, result);
            if(r != 1) return r;
            if(peek(tzr).type == ORDER) {
                r = ParseOrder(tzr, result, table_name);
                if(r != 1) return r;
            }
            if((tzr.expect(SEMICOLON)).type == ERROR) return 0;
            break;
        case SEMICOLON:
            for(unsigned int i=0;i<rows.size();i++) result.push_back(i);
            break;
        case ORDER:
            for(unsigned int i=0;i<rows.size();i++) result.push_back(i);
            r = ParseOrder(tzr, result, table_name);
            if(r != 1) return r;
            break;
        default:
            return 0;
    }

    if(show_list[0].type == STAR) {
        ct->displayColumns();
    } else {
        for(const Token &token : show_list) {
            Column c = ct->get_Column(token.name);
            std::cout<<std::left<<std::setw(20)<<std::setfill(' ')<<c.colname;
        }
        std::cout<<"\n";
    }
    for(const int &i : result) {

        if(show_list[0].type == STAR) {
            for(Node &node : rows[i].rowNodes) printNode(node);
            std::cout<<"\n";
        } else {
            for(const Token &token : show_list) {
                for(Node &node : rows[i].rowNodes) {
                    if(token.name == node.get_colname())
                        printNode(node);
                }
            }
            std::cout<<"\n";
        }
    }
    return 1;
}

void split_str(std::string const &str, const char delim,
			std::vector<std::string> &out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}
void RestoreTables() {
    ifstream savedata; // indata is like cin
    savedata.open("save_data"); // opens the file
    if(!savedata) { // file couldn't be opened
      cerr << "Error: database not found." << endl;
      return;
    }
    while(!savedata.eof()) {
        std::string data;
        getline(savedata, data, ';');
        std::vector<std::string> line;
        split_str(data, '#', line);
        if(line.empty()) return;
        if(line.size() != 9) {
            std::cout<<line.size()<<" Corrupted database 7. Clearing all tables\n";
            cout<<line[0]<<"\n"<<line[1]<<"\n"<<line[2]<<"\n"<<line[3]<<"\n"<<line[4]<<"\n"<<line[5]<<"\n"<<line[6]<<"\n"<<line[7]<<"\n";
            tables.clear();
            return;
        }
        std::vector<Column> columns;
        std::vector<std::string> colnames;
        split_str(line[1], ',', colnames);
        std::vector<std::string> coltypes;
        split_str(line[2], ',', coltypes);
        std::vector<std::string> colnull;
        split_str(line[3], ',', colnull);
        std::vector<std::string> colinc;
        split_str(line[4], ',', colinc);
        std::vector<std::string> colfor;
        split_str(line[5], ',', colfor);

        int primary_col;
        try {
            primary_col = std::stoi(line[6]);
        } catch(...) {
            std::cout<<"Corrupted database 6. Clearing all tables\n";
            tables.clear();
            return;
        }

        std::vector<std::string> colcs;
        if(line[7] != "NULL")
            split_str(line[7], ',', colcs);

        int c = 0;
        for(unsigned int i=0;i<colnames.size();i++) {
            std::string name = colnames[i];
            TokenType type;
            try {
                type  = (TokenType)std::stoi(coltypes[i]);
            } catch(...) {
                std::cout<<"Corrupted database 5. Clearing all tables\n";
                tables.clear();
                return;
            }
            bool isNotNull;
            try {
                int inn  = std::stoi(colnull[i]);
                if(inn == 1) isNotNull = true;
                else if(inn == 0) isNotNull = false;
                else throw "Invalid argument";
            } catch(...) {
                std::cout<<"Corrupted database 4. Clearing all tables\n";
                tables.clear();
                return;
            }
            bool isAutoIncrement;
            int next_increment;
            try {
                int isa  = std::stoi(colinc[i]);
                if(isa > 0) {
                    isAutoIncrement = true;
                    next_increment = isa;
                }
                else if(isa == 0) isAutoIncrement = false;
                else throw "Invalid argument";
            } catch(...) {
                std::cout<<"Corrupted database 2. Clearing all tables\n";
                tables.clear();
                return;
            }
            bool isPrimary = false;
            if(i == (unsigned int)primary_col) isPrimary = true;
            int char_size = 0;
            if(type == CHAR) {
                try {
                    char_size  = std::stoi(colcs[c++]);
                } catch(...) {
                    std::cout<<"Corrupted database 3. Clearing all tables\n";
                    tables.clear();
                    return;
                }
            }
            bool isForeignKey = false;
            std::string foreign_table = "";
            std::string foreign_column = "";

            std::vector<std::string> colfor_split;
            if(colfor[i] != "NULL") {
                isForeignKey = true;
                split_str(colfor[i], '+', colfor_split);
                try {
                    foreign_table = colfor_split[0];
                    foreign_column = colfor_split[1];
                } catch(...) {
                    std::cout<<"Corrupted database 3. Clearing all tables\n";
                    tables.clear();
                    return;
                }
            }
            Column c(type, name, isNotNull, isPrimary, isAutoIncrement);
            c.next_increment = next_increment;
            c.char_size = char_size;
            c.isForeignKey = isForeignKey;
            c.foreign_column = foreign_column;
            c.foreign_table = foreign_table;
            columns.push_back(c);
        }
        Table t(line[0],columns);

        if(line[8] != "NULL") {
            std::vector<std::string> rows;
            split_str(line[8], '/', rows);
            for(std::string &row : rows) {
                std::vector<std::string> data;
                split_str(row, '@', data);
                std::vector<Node> nodes;
                for(int i=0;i<t.num_columns;i++){
                    Node n(data[i], columns[i].type, i);
                    n.colname = columns[i].colname;
                    nodes.push_back(n);
                }
                Row r(nodes);
                t.rows.push_back(r);
            }
        }
        tables.push_back(t);
    }
    savedata.close();
}

void SaveTables() {
    ofstream savedata;
    savedata.open ("save_data");
    for(Table &t : tables) {
        std::string save_string;
        save_string = t.get_Name() + "#";
        for(const Column &c: t.columns) {
            save_string += c.colname + ",";
        }
        save_string[save_string.size()-1] = '#';

        for(const Column &c: t.columns) {
            save_string += std::to_string(c.type) + ",";
        }
        save_string[save_string.size()-1] = '#';


        for(const Column &c: t.columns) {
            save_string += std::to_string(c.isNotNull) + ",";
        }
        save_string[save_string.size()-1] = '#';

        for(const Column &c: t.columns) {
            if(c.isAutoIncrement) {
                save_string += std::to_string(c.next_increment)+",";
            } else save_string += "0,";
        }
        save_string[save_string.size()-1] = '#';

        for(const Column &c: t.columns) {
            if(c.isForeignKey) {
                save_string += c.foreign_table + "+" + c.foreign_column + ",";
            } else save_string += "NULL,";
        }
        save_string[save_string.size()-1] = '#';

        save_string += std::to_string(t.primary_key_column) + "#";

            bool any = false;
        for(const Column &c: t.columns) {
            if(c.type == CHAR) {
                any =true;
                save_string += std::to_string(c.char_size)+",";
            }
        }
        if(!any) save_string += "NULL#";
        else save_string[save_string.size()-1] = '#';

        if(t.rows.empty()) save_string += "NULL#";
        for(Row &r : t.rows) {
            for(Node &node : r.rowNodes) {
                save_string += node.data + "@";
            }
            save_string[save_string.size()-1] = '/';
        }
        save_string[save_string.size()-1] = ';';
        savedata << save_string;
    }
    savedata.close();
}

void TryParse(std::string cmd) {
    Tokenizer tzr(cmd);
    Token t = peek(tzr);
    int result = 2;
    if(!tzr.get_isValid()) result = 2;
    else {
        if(t.type == CREATE) {
            result = ParseCreateStmt(tzr);
        } else if(t.type == DROP) {
            result = ParseDropStmt(tzr);
        } else if(t.type == INSERT) {
            result = ParseInsertStmt(tzr);
         } else if(t.type == TRUNCATE) {
            result = ParseTruncateStmt(tzr);
        } else if(t.type == DELETE) {
            result = ParseDeleteStmt(tzr);
        } else if(t.type == UPDATE) {
            result = ParseUpdateStmt(tzr);
        } else if(t.type == SELECT) {
            result = ParseSelectStmt(tzr);
            if(result == 1) return;
        } else if(t.type == QUIT) {
            if(!((tzr.expect(QUIT)).type == ERROR))
                if(!((tzr.expect(LPAREN)).type == ERROR))
                    if(!((tzr.expect(RPAREN)).type == ERROR))
                        if(!((tzr.expect(SEMICOLON)).type == ERROR)) {
                            SaveTables();
                            std::cout<<"EXITING\n";
                            exit(0);
                        }
            result = 0;
        } else {
            result = 0;
        }
    }
    switch(result) {
        case 0:
            std::cout<<"Error while parsing statement\n";
            break;
        case 1:
            std::cout<<"SUCCESS\n";
            break;
        case 2:
            break;
        default:
            std::cout<<"Unknown Error\n";
            break;
    }
}

int main() {
    RestoreTables();

    std::string cmd = "";
    while(true) {
    getline(cin, cmd, ';');
    std::cin.ignore(255, '\n');

    cmd += ';';
    TryParse(cmd);
    }

	return 0;

}

