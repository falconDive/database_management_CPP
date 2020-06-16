#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>
#include <math.h>
#include <iostream>
#include <bits/stdc++.h>
typedef enum {
	EOL = 0, CREATE, TABLE, COMMA, STAR, EQ, LPAREN, RPAREN, PRIMARY_KEY, NOT_NULL, AUTO_INCREMENT, SEMICOLON, STRING_CONS,
	TRUNCATE, DROP, WHERE, DELETE, FROM, INSERT, INTO, VALUES, SELECT, TEXT_STR, ERROR, INT, FLOAT, CHAR, TEXT, DATE,
	 TIME, AND, UPDATE, SET, NOTEQ, LTEQ, LT, GT, GTEQ, ORDER, BY, ASC, DESC, BETWEEN, QUIT, FOREIGN_KEY, REFERENCES
}TokenType;

class Token {
	public:
	std::string name;
	TokenType type;
};

TokenType str_to_type(std::string str);

class Tokenizer
{
    public:
        Tokenizer(std::string);
        ~Tokenizer();
        Token GetToken();
        TokenType UngetToken(Token);
        bool get_isValid() {
            return isValid;
        }
        Token expect(TokenType);
        void skipspace(std::string &);
        private:
        std::string input_string;
        bool isValid;
        std::vector<Token> tokens;
        std::string scanString(std::string &);
        std::string scanKeyword(std::string &);

};

#endif // TOKENIZER_H
