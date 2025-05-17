#ifndef __TOKEN__
#define __TOKEN__

#include <string>
using std::string;

namespace token {
typedef string TokenType;
//namespace token{
#define ILLEGAL  "ILLEGAL"
#define MEOF     "EOF"

// Identifiers + literals
#define IDENT  "IDENT"// num, foobar, x, y, ...
#define INT    "INT"  // 123
#define STRING "STRING"

// Operators
#define ASSIGN    "="
#define PLUS      "+"
#define MINUS     "-"
#define BANG      "!"
#define ASTERISK  "*"
#define SLASH     "/"

#define LT   "<"
#define GT   ">"

#define EQ       "=="
#define NOT_EQ   "!="

// Delimiters
#define COMMA       ","
#define SEMICOLON   ";"
#define COLON       ":"  // For HASH

#define LPAREN   "("
#define RPAREN   ")"
#define LBRACE   "{"
#define RBRACE   "}"
#define LBRACKET "["    // For Array
#define RBRACKET "]"    // For Array

// Keywords
#define FUNCTION   "FUNCTION"
#define LET        "LET"
#define TRUE       "TRUE"
#define FALSE      "FALSE"
#define IF         "IF"
#define ELSE       "ELSE"
#define RETURN     "RETURN"

typedef struct Token {
	Token()
	{
		Type = ILLEGAL;
		Literal = ILLEGAL;
	}

	Token(TokenType tt, string s)
	{
		Type = tt;
		Literal = s;
	}
	
	TokenType Type;
	string Literal;
}Token;


TokenType LookupIdent(string ident);
} // namespace token
#endif