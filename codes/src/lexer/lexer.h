#ifndef __LEXER__
#define __LEXER__
#include <string>
#include "token/token.h"

using std::string;

namespace lexer
{
class Lexer
{
public:
	Lexer(const string &input);
public:
	token::Token NextToken();
	void skipWhitespace();
	void readChar();
	char peekChar();
	string readIdentifier();
	string readNumber();
    string readString();
	bool isLetter(char ch);
	bool isDigit(char ch);
	token::Token newToken(token::TokenType tt, char ch);

public:
	string input_;
	size_t position_;       // current position in input (points to current char)
	size_t readPosition_;   // current reading position in input (after current char)
	char ch_;               // current char under examination
};
} // namespace lexer
#endif

