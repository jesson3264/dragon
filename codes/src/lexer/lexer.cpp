#include "lexer.h"

namespace lexer
{
Lexer::Lexer(const string &input)
{
	input_ = input;
	position_ = 0;
	readPosition_ = 0;
	ch_ = '\0';
	this->readChar();
}

token::Token Lexer::NextToken()
{
	token::Token tok;
	string literal;
    skipWhitespace();
	switch (ch_) {
	case '=':
		if (peekChar() == '=') 
		{
			readChar();
			literal = "==";
			tok = token::Token(EQ, "==");
		} 
		else 
		{
			tok = newToken(ASSIGN, ch_);
		}
		break;

	case '+':
		tok = newToken(PLUS, ch_);
		break;

	case '-':
		tok = newToken(MINUS, ch_);
		break;

	case '!':
		if (peekChar() == '=')
		{
			// ch = ch_;
			readChar();
			tok = token::Token(NOT_EQ, NOT_EQ);

		} 
		else 
		{
			tok = newToken(BANG, ch_);
		}
		break;

	case '/':
		tok = newToken(SLASH, ch_);
		break;

	case '*':
		tok = newToken(ASTERISK, ch_);
		break;

	case '<':
		tok = newToken(LT, ch_);
		break;

	case '>':
		tok = newToken(GT, ch_);
		break;

	case ';':
		tok = newToken(SEMICOLON, ch_);
		break;
        case ':':
            tok = newToken(COLON, ch_);
            break;
	case ',':
		tok = newToken(COMMA, ch_);
		break;

	case '{':
		tok = newToken(LBRACE, ch_);
		break;

	case '}':
		tok = newToken(RBRACE, ch_);
		break;
		
	case '(':
		tok = newToken(LPAREN, ch_);
		break;
		
	case ')':
		tok = newToken(RPAREN, ch_);
		break;

    case '"':
        tok.Type = STRING;
        tok.Literal = readString();
        break;
    case '[':
        tok = newToken(LBRACKET, ch_);
        break;
    case ']':
         tok = newToken(RBRACKET, ch_);
         break;
	case 0:
		tok.Literal = "";
		tok.Type = MEOF;
		break;
	default:
	if (isLetter(ch_)) {
		tok.Literal = readIdentifier();
		tok.Type = token::LookupIdent(tok.Literal);
        return tok;  // 需要直接返回
	} else if (isDigit(ch_)) {
		tok.Type = INT;
		tok.Literal = readNumber();
		return tok;
	} else {
		tok = newToken(ILLEGAL, ch_);
	}
 	}
	readChar();
	return tok;
}

// 跳过所有空格
void Lexer::skipWhitespace()
{
	while (ch_ == ' ' || this->ch_ == '\t'
		|| ch_ == '\n'
		|| ch_ == '\r')
	{
		readChar(); // 吞掉当前字符
	}
}

void Lexer::readChar()
{
	if (readPosition_ >= input_.length())
	{
		ch_ = '\0';
	}
	else 
	{
		ch_ = input_[readPosition_];
	}

    position_ = readPosition_;
    readPosition_ += 1;
}

// 获取下一个要读取的字符
char Lexer::peekChar()  
{
	if (readPosition_ >= input_.length())
	{
		return '\0';
	}

	return input_[readPosition_];
}

string Lexer::readIdentifier()
{
	int pos = position_;
	while (isLetter(ch_)) 
	{
		readChar();
	}

	return input_.substr(pos, position_ - pos);
}

string Lexer::readNumber() 
{
	int pos = position_;
	while (isDigit(ch_)) {
		readChar();
	}

	return input_.substr(pos, position_ - pos);
}

string Lexer::readString() {
    int pos = position_ + 1;
    for (;;) {
        readChar();
        if (ch_ == '"' || ch_ == '\0') {
            break;
        }
    }

    return input_.substr(pos, position_ - pos);
}

bool Lexer::isLetter(char ch)
{
	return ('a' <= ch && ch <= 'z')  ||
			('A' <= ch && ch <= 'Z') || 
			('_' == ch);
	
}

bool Lexer::isDigit(char ch)
{
	return '0' <= ch && ch <= '9';
}

token::Token Lexer::newToken(token::TokenType tt, char ch)
{
	token::Token t;
	t.Literal = string(1, ch);
	t.Type = tt;
	return t;
}

} // namespace lexer