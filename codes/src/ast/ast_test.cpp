//====================================
// Author : jesson3264@163.com
//====================================
// ast 测试代码
//
#include "ast.h"
#include <memory>
#include <iostream>
/*
 // 测试用例
TEST(ASTTest, ProgramString) {
    auto program = std::make_unique<Program>();
    program->statements.push_back(
        std::make_unique<LetStatement>(
            Token{"LET", "let"},
            std::make_unique<Identifier>(Token{"IDENT", "myVar"}, "myVar"),
            std::make_unique<Identifier>(Token{"IDENT", "anotherVar"}, "anotherVar")
        )
    );

    EXPECT_EQ(program->String(), "let myVar = anotherVar;");
}
 */

void TestAst()
{
    auto p = std::make_unique<ast::Program>();
    token::Token token;
    token.Type = "LET";
    token.Literal = "let";

    auto identi = std::make_shared<ast::Identifier>();
    identi->token_ = token::Token("IDENT", "myVar");
    identi->value_ = "myVar";

    auto exp = std::make_shared<ast::Identifier>();
    exp->token_ = token::Token("IDENT", "anotherVar");
    exp->value_ = "anotherVar";

    auto letStatement = std::make_shared<ast::LetStatement>();
    letStatement->token_ = token;
    letStatement->name_ = identi;
    letStatement->value_ = exp;

    p->statements_.push_back(letStatement);
//    Token token_;
//    Identifier *name_;
//    Expression *value_;

    std::cout << p->String();

}