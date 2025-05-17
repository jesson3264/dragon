#include "lexer_test.h"
#include "lexer.h"
#include "token.h"

#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;
using std::cout;
using std::endl;

typedef struct Test
{
    Test(token::TokenType tt, const string &str) {
        expectedType = tt;
        expectedLiteral = str;
    }
	token::TokenType expectedType;
	string expectedLiteral;
}Test;

void TestNextToken()
{
	string input = R"(let five = 5;
let ten = 10;

let add = fn(x, y) {
  x + y;
};

let result = add(five, ten);
!-/*5;
5 < 10 > 5;

if (5 < 10) {
	return true;
} else {
	return false;
}

10 == 10;
10 != 9;
"foobar"
"foo bar"
[1, 2];
{"foo": "bar"})";


	vector<Test> expected =
    {
		Test{LET, "let"},//0
        Test{IDENT, "five"},
        Test{ASSIGN, "="},
        Test{INT, "5"},
        Test{SEMICOLON, ";"},
        Test{LET, "let"},//5
        Test{IDENT, "ten"},
        Test{ASSIGN, "="},
        Test{INT, "10"},
        Test{SEMICOLON, ";"},
        Test{LET, "let"}, // 10
        Test{IDENT, "add"},
        Test{ASSIGN, "="},
        Test{FUNCTION, "fn"},
        Test{LPAREN, "("},
        Test{IDENT, "x"},//15
        Test{COMMA, ","},
        Test{IDENT, "y"},
        Test{RPAREN, ")"},
        Test{LBRACE, "{"},
        Test{IDENT, "x"},
        Test{PLUS, "+"},
        Test{IDENT, "y"},
        Test{SEMICOLON, ";"},
        Test{RBRACE, "}"},
        Test{SEMICOLON, ";"},
        Test{LET, "let"},
        Test{IDENT, "result"},
        Test{ASSIGN, "="},
        Test{IDENT, "add"},
        Test{LPAREN, "("},
        Test{IDENT, "five"},
        Test{COMMA, ","},
        Test{IDENT, "ten"},
        Test{RPAREN, ")"},
        Test{SEMICOLON, ";"},
        Test{BANG, "!"},
        Test{MINUS, "-"},
        Test{SLASH, "/"},
        Test{ASTERISK, "*"},
        Test{INT, "5"},
        Test{SEMICOLON, ";"},
        Test{INT, "5"},
        Test{LT, "<"},
        Test{INT, "10"},
        Test{GT, ">"},
        Test{INT, "5"},
        Test{SEMICOLON, ";"},
        Test{IF, "if"},
        Test{LPAREN, "("},
        Test{INT, "5"},
        Test{LT, "<"},
        Test{INT, "10"},
        Test{RPAREN, ")"},
        Test{LBRACE, "{"},
        Test{RETURN, "return"},
        Test{TRUE, "true"},
        Test{SEMICOLON, ";"},
        Test{RBRACE, "}"},
        Test{ELSE, "else"},
        Test{LBRACE, "{"},
        Test{RETURN, "return"},
        Test{FALSE, "false"},
        Test{SEMICOLON, ";"},
        Test{RBRACE, "}"},
        Test{INT, "10"},
        Test{EQ, "=="},
        Test{INT, "10"},
        Test{SEMICOLON, ";"},
        Test{INT, "10"},
        Test{NOT_EQ, "!="},
        Test{INT, "9"},
        Test{SEMICOLON, ";"},
        Test{STRING, "foobar"},
        Test{STRING, "foo bar"},
        Test{LBRACKET, "["},
        Test{INT, "1"},
        Test{COMMA, ","},
        Test{INT, "2"},
        Test{RBRACKET, "]"},
        Test{SEMICOLON, ";"},
        Test{LBRACE, "{"},
        Test{STRING, "foo"},
        Test{COLON, ":"},
        Test{STRING, "bar"},
        Test{RBRACE, "}"},
        Test{MEOF, ""},
	};

	lexer::Lexer lexer(input);
	token::Token tok;
	for (size_t i = 0; i < expected.size(); ++i)
	{
		tok = lexer.NextToken();
		if (tok.Type != expected[i].expectedType)
		{
			cout << i << " ERROR real:" << tok.Type << " expected:" << expected[i].expectedType << endl;
		}
	}
}