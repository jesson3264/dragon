#include "token.h"


#include <map>
using std::map;
namespace token {
map<string, TokenType> keywords = {
	{"fn", FUNCTION},
	{"let", LET},
	{"true", TRUE},
	{"false", FALSE},
	{"if", IF},
	{"else", ELSE},
	{"return", RETURN},
};

TokenType LookupIdent(string ident)
{
	if (keywords.find(ident) != keywords.end())
	{
		return keywords[ident];
	}
	
	return IDENT;
}
}