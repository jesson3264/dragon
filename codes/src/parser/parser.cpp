//
// Created by Jesson on 2025/5/11.
//

#include "parser.h"
#include <map>
#include "token.h"
namespace parser {
std::map<token::TokenType, parser::Precedence> precedences = {
        {EQ, parser::EQUALS},
        {NOT_EQ, parser::EQUALS},
        {LT, parser::LESSGREATER},
        {GT, parser::LESSGREATER},
        {PLUS, parser::SUM},
        {MINUS, parser::SUM},
        {SLASH, parser::PRODUCT},
        {ASTERISK, parser::PRODUCT},
        {LPAREN, parser::CALL},
        {LBRACKET, parser::INDEX},
};

} // namespace parser