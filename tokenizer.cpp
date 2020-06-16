#include "tokenizer.h"

#define TT_L 46
std::string TT[] = {"EOL","CREATE", "TABLE", "COMMA", "STAR", "EQ", "LPAREN", "RPAREN", "PRIMARY KEY", "NOT NULL",
    "AUTO_INCREMENT", "SEMICOLON", "STRING_CONS", "TRUNCATE", "DROP", "WHERE", "DELETE", "FROM", "INSERT", "INTO", "VALUES",
     "SELECT", "TEXT_STR", "ERROR", "INT", "FLOAT", "CHAR", "TEXT", "DATE", "TIME", "AND", "UPDATE", "SET", "NOTEQ",
     "LTEQ", "LT", "GT", "GTEQ", "ORDER", "BY", "ASC", "DESC", "BETWEEN", "QUIT", "FOREIGN KEY", "REFERENCES"};


std::string KEYWORDS[] = {"CREATE", "TABLE", "NOT", "NULL", "PRIMARY", "KEY", "AUTO_INCREMENT",
							"TRUNCATE", "DROP", "WHERE", "DELETE", "FROM", "INSERT", "INTO", "VALUES", "SELECT",
							 "INT", "FLOAT", "CHAR", "TEXT", "DATE", "TIME", "AND", "UPDATE", "SET", "ORDER", "BY", "ASC", "DESC", "BETWEEN", "QUIT", "FOREIGN", "REFERENCES"};

Token Tokenizer::GetToken() {
    if(isValid) {
        if(!tokens.empty())
        {
            Token t = tokens[tokens.size()-1];
            tokens.pop_back();
            return t;
        }
    }
    Token t;
    t.name = "";
    t.type = tokens.empty() ? EOL : ERROR;
    return t;
}

TokenType Tokenizer::UngetToken(Token token) {
    if(isValid) {
            tokens.push_back(token);
            return token.type;
    }
    return ERROR;
}

Token Tokenizer::expect(TokenType type) {
    Token t  = GetToken();
    if(t.type == type) return t;
    else {
        UngetToken(t);
        t.type = ERROR;
        t.name = "";
    }
    return t;
}
Tokenizer::Tokenizer(std::string str)
{
	Token current;
	skipspace(str);
	std::string tmp;
    bool done = false;
	while(!str.empty()) {
		current.name = "";
		current.type = EOL;
		switch(str[0]) {
			case ',':
				current.type = COMMA;
				str.erase(0, 1);
				break;
			case '(':
				current.type = LPAREN;
				str.erase(0, 1);
				break;
			case ')':
				current.type = RPAREN;
				str.erase(0, 1);
				break;
			case ';':
				current.type = SEMICOLON;
				str.erase(0, 1);
				break;
			case '=':
				current.type = EQ;
				str.erase(0, 1);
				break;
			case '<':
			    switch(str[1]) {
                    case '>':
                        current.type = NOTEQ;
                        str.erase(0,1);
                        break;
                    case '=':
                        current.type = LTEQ;
                        str.erase(0,1);
                        break;
                    default:
                        current.type = LT;
                        break;
			    }
				str.erase(0, 1);
				break;
			case '>':
			    switch(str[1]) {
                    case '=':
                        current.type = GTEQ;
                        str.erase(0,1);
                        break;
                    default:
                        current.type = GT;
                        break;
			    }
				str.erase(0, 1);
				break;
			case '*':
				current.type = STAR;
				str.erase(0, 1);
				break;
			case '"':
				current.type = TEXT_STR;
				tmp = str.substr(1, str.length());
				done = false;
				while(!tmp.empty()) {
					if(tmp[0] != '"') {
						current.name += tmp[0];
						tmp.erase(0, 1);
					} else {
						tmp.erase(0, 1);
						done = true;
						break;
					}
				}
				if(!done) {
                    isValid = false;
                    return;
				}
				str.erase(0, current.name.length()+2);
				break;
			default:
			    if(!isalnum(str[0])) {
                    isValid = false;
                    return;
			    }
				current.name = scanKeyword(str);
				if(current.name == "") {
					//not keyword
					current.type = STRING_CONS;
					current.name = scanString(str);
				} else {
					current.type = str_to_type(current.name);
					if(current.type == ERROR) {
                        isValid  = false;
                        return;
					}
				}
			break;
		}
		skipspace(str);
		tokens.push_back(current);
	}
    //for(const Token &token: tokens)
    //    std::cout<<token.name<<":"<<TT[token.type]<<" ";
    //std::cout<<"\n";

    std::reverse(tokens.begin(), tokens.end());
    isValid = true;
    //ctor
}

TokenType str_to_type(std::string str) {
	for(int i=0;i<TT_L;i++) {
		if(TT[i] == str) return (TokenType)i;
	}
	return ERROR;
}

void Tokenizer::skipspace(std::string &str) {
	if(str.empty()) return;
	while(str[0] == ' ' || str[0] == 10) {
		str.erase(0, 1);
		if(str.empty()) return;
	}
}

std::string Tokenizer::scanString(std::string &str) {
	char c;
	std::string tmp = "", tmp2;

	tmp2 = str.substr(0, str.length());

	while(!tmp2.empty()) {
		c = tmp2[0];
		tmp2 = tmp2.substr(1, tmp2.length());
		if(isalnum(c)==0 && c != '_')
			break;
		tmp += c;
	}
	str.erase(0, tmp.length());
	return tmp;
}
std::string Tokenizer::scanKeyword(std::string &str) {
//	if(str.size() > 5)
	char c;
	std::string tmp = "", tmp2;

	tmp2 = str.substr(0, str.length());

	while(!tmp2.empty()) {
		c = tmp2[0];
		tmp2 = tmp2.substr(1, tmp2.length());
		if(isalpha(c)==0 && c != '_')
			break;
		tmp += c;
	}

	for(const std::string &s : KEYWORDS) {
		if(s == tmp) {
		    tmp2 = str;
			str.erase(0, tmp.length());
			if(tmp == "NOT") {
				skipspace(str);
				if(scanKeyword(str) == "NULL") {
					return "NOT NULL";
				} else {
                    str = tmp2;
                    return "NOT";
				}
			}
			if(tmp == "PRIMARY") {
				skipspace(str);
				if(scanKeyword(str) == "KEY") {
					return "PRIMARY KEY";
				} else {
                    str = tmp2;
                    return "PRIMARY";
				}
			}
			if(tmp == "FOREIGN") {
				skipspace(str);
				if(scanKeyword(str) == "KEY") {
					return "FOREIGN KEY";
				} else {
                    str = tmp2;
                    return "FOREIGN";
				}
			}

			return tmp;
		}
	}
	return "";
}

Tokenizer::~Tokenizer()
{
    //dtor
}


