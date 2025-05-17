//
// Created by Jesson on 2025/5/11.
//

#include "parser_test.h"
#include "parser.h"
#include "lexer.h"
#include "test_tool.h"

#include <string>
//#include <variant>
#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>
#define E std::endl
#define S std::cerr
#define ERRINFO std::cerr<<"error:" << __FUNCTION__ << " " << __LINE__ << std::endl;

struct TestCase {
    std::string input;
    std::string expectedIdentifier;
    Value expectedValue;

};

// 辅助函数实现
void checkParserErrors(parser::Parser& p) {
    auto errors = p.getErrors();
    if (errors.empty()) return;

    std::cerr << "parser has " << errors.size() << " errors" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "parser error: " << msg << std::endl;
    }
    std::cerr << "Parser encountered errors";
}

bool testLetStatement(ast::Statement* stmt, const std::string& name) {
    if (stmt->TokenLiteral() != "let") {
        std::cerr << "ERROR:tokenLiteral not 'let'. got=" << stmt->TokenLiteral();
        return false;
    }

    auto* letStmt = dynamic_cast<ast::LetStatement*>(stmt);
    if (!letStmt) {
        std::cerr << "ERROR:stmt not LetStatement";
        return false;
    }

    if (letStmt->name_->value_ != name) {
        std::cerr << "letStmt.name.value not '" << name
                      << "'. got=" << letStmt->name_->value_;
        return false;
    }

    if (letStmt->name_->TokenLiteral() != name) {
        std::cerr << "FAILED: letStmt.name.tokenLiteral() not '" << name
                      << "'. got=" << letStmt->name_->TokenLiteral();
        return false;
    }

    return true;
}

bool testIntergerLiteral(ast::Expression *exp, const Value & v)
{
    auto * integ = dynamic_cast<ast::IntegerLiteral*> (exp);
    if (!integ) {
        ERRINFO
        std::cerr << "ERROR" << std::endl;
        return false;
    }

    if (integ->value_ != v.getInt()) {
        ERRINFO
        std::cerr << "ERROR" << E;
        return false;
    }

    if (integ->TokenLiteral() != std::to_string(v.getInt())) {
        std::cerr << "integ.TokenLiteral not " << v.getInt() << " got"
            << integ->TokenLiteral() << E;
        return false;
    }

    return true;
}

bool testBooleanLiteral(ast::Expression *exp, const Value & v)
{
    auto * boolLiter = dynamic_cast<ast::Boolean*>(exp);
    if (!boolLiter) {
        S << "ERROR" << E;
        return false;
    }

    if (v.getBool() != boolLiter->value_) {
        S << "ERROR" << E;
        return false;
    }

    if (boolLiter->TokenLiteral() != (v.getBool()?"true":"false")) {
        S << std::to_string(v.getBool());
        S << "ERROR" << E;
        return false;
    }
    return true;
}

bool testIdentifier(ast::Expression* expr, const Value& v)
{
    auto * ident = dynamic_cast<ast::Identifier*>(expr);
    if (!ident) {
        S << "ERROR" << E;
        return false;
    }

    if (ident->value_ != v.getString()) {
        S << "ERROR" << E;
        return false;
    }

    if (ident->TokenLiteral() != v.getString()) {
        S << "ERROR" << E;
        return false;
    }
    return true;
}

bool testLiteralExpression(ast::Expression *exp,const Value & v)
{
    // 判断期望值的类型
    if (v.getType() == Value::VINT) {
        return testIntergerLiteral(exp, v);
    }
    else if (v.getType() == Value::VBOOL) {
        return testBooleanLiteral(exp, v);
    }
    else if (v.getType() == Value::VSTRING) {
        return testIdentifier(exp, v);
    }
    return false;
}

bool testLiteralExpression2(ast::Expression* expr, const Value &expected) {
    switch (expected.getType()) {
        case 0: { // int
            auto* intLit = dynamic_cast<ast::IntegerLiteral*>(expr);
            if (!intLit) {
                std::cerr << "ERROR expr not IntegerLiteral";
                return false;
            }
            if (intLit->value_ != expected.getInt()) {
                std::cerr << "ERROR: integer value not " << expected.getInt()
                              << ". got=" << intLit->value_;
                return false;
            }
            return true;
        }
        case 1: { // bool
            auto* boolLit = dynamic_cast<ast::Boolean*>(expr);
            if (!boolLit) {
                std::cerr << "ERROR: expr not Boolean";
                return false;
            }
            if (boolLit->value_ != expected.getBool()) {
                std::cerr << "ERROR::boolean value not " << expected.getBool()
                              << ". got=" << boolLit->value_;
                return false;
            }
            return true;
        }
        case 2: { // string (identifier)
            auto* ident = dynamic_cast<ast::Identifier*>(expr);
            if (!ident) {
                std::cerr << "ERROR:expr not Identifier";
                return false;
            }
            if (ident->value_ != expected.getString()) {
                std::cerr << "ERROR:identifier value not " << expected.getString()
                              << ". got=" << ident->value_;
                return false;
            }
            return true;
        }
        default:
            std::cerr << "ERROR:type of expr not handled";
            return false;
    }
}

void TestLetStatements()
{
    std::vector<TestCase> tests = {
            {"let x = 5;", "x", 5},
            {"let y = true;", "y", true}, // 这里填的 true 是 Boolean 类中的 value_ 的类型
            TestCase{"let foobar = y;", "foobar", string("y")}
    };

    for (const auto & tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program->statements_.size(), (size_t)1);

        auto* stmt = program->statements_[0].get();
        ASSERT_TRUE(testLetStatement(stmt, tt.expectedIdentifier));

        auto* letStmt = dynamic_cast<ast::LetStatement*>(stmt);
        // ASSERT_NE(letStmt, nullptr);

        auto* val = letStmt->value_.get();
        ASSERT_TRUE(testLiteralExpression(val, tt.expectedValue));
    }

}

void TestReturnStatements()
{
    struct Test {
        std::string input;
        Value expectedValue;
    };
    std::vector<Test> tests = {
        {"return 5;", 5},
        {"return true;", true},
        {"return foobar;", "foobar"}
    };
    
    for (const auto& tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program->statements_.size(), (size_t)1);

        auto* stmt = program->statements_[0].get();
        auto* retStmt = dynamic_cast<ast::ReturnStatement*>(stmt);
        // ASSERT_NE(letStmt, nullptr);
        if (retStmt == nullptr) {
            std::cout << "retStmt should not nullptr" << std::endl;
            exit(1);
        }

        if (retStmt->token_.Type != RETURN) {
            std::cout << "type error." << std::endl;
        }

        auto expr = retStmt->returnValue_.get();
        testLiteralExpression(expr, tt.expectedValue);
    }
}

void TestIdentifierExpression() {
    std::string input("foobar;");
    lexer::Lexer l(input);
    parser::Parser p(l);

    auto program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements_.size() != 1) {
        std::cerr << "ERROR:identifier expression should have 1 statement" << std::endl;
    }

    auto x = program->statements_[0].get();
    (void)x;
    auto *expr = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());

    auto* ident = dynamic_cast<ast::Identifier*> (expr->expression_.get());
    // auto* ident = dynamic_cast<ast::Identifier*>(stmt);
    if (ident == nullptr) {
        std::cerr << "ERROR:identifier expression should not be ExpressionStatement" << std::endl;
        exit(1);
    }

    if (ident->value_ != "foobar") {
        std::cerr << "ERROR:identifier value not " << ident->value_;
        exit(1);
    }

    if (ident->TokenLiteral() != "foobar") {
        std::cerr << "ERROR:identifier tokenLiteral not 'foobar'";
        exit(1);
    }

}

void TestIntegerLiteralExpression( ) {
    std::string input("5;");
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements_.size(), (size_t)1);

    auto* stmt = program->statements_[0].get();
    auto* expr = dynamic_cast<ast::ExpressionStatement*>(stmt);
    if (expr == nullptr) {
        std::cerr << "ERROR:expression should not be ExpressionStatement" << std::endl;
        exit(1);
    }

    auto* inter = dynamic_cast<ast::IntegerLiteral*>(expr->expression_.get());
    if (inter == nullptr) {
        std::cerr << "ERROR:expression should not be IntegerLiteralExpression" << std::endl;
        exit(1);
    }

    if (inter->value_ != 5) {
        std::cerr << "ERROR:integer value not " << inter->value_;
    }
}
// 前缀表达式
void TestParsingPrefixExpressions()
{
    struct TestCase {
        string input;
        string operator_;
        Value value;
    };

    std::vector<TestCase> tests{
            {"!5", "!", 5},
            {"-15;", "-", 15},
            {"!foobar;", "!", "foobar"},
            {"-foobar;", "-", "foobar"},
            {"!true;", "!", true},
            {"!false;", "!", false},
    };

    for(auto &tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        if (program->statements_.size() != 1) {
            std::cerr << "ERROR:" <<std::endl;
            exit(1);
        }

        auto* expr = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());

        auto* prefixStmt = dynamic_cast<ast::PrefixExpression*>(expr->expression_.get());

        if (!prefixStmt) {
            std::cerr <<"ERROR " << tt.input<< std::endl;
            exit(1);
        }

        if (prefixStmt->operator_ != tt.operator_) {
            std::cerr << "ERROR " << tt.input << std::endl ;
            exit(2);
        }

        testLiteralExpression(prefixStmt->right_.get(), tt.value);
    }
}

bool testInfixExpression(ast::Expression* exp, const Value & left,
                         string operator_, const Value & right)
{
    auto *opExp = dynamic_cast<ast::InfixExpression*> (exp);
    if (!opExp) {
        return false;
    }

    if (!testLiteralExpression(opExp->left_.get(), left)) {
        return false;
    }

    if (opExp->operator_ != operator_) {
        return false;
    }

    if (!testLiteralExpression(opExp->right_.get(), right)) {
        return false;
    }

    return true;
}

void TestParsingInfixExpressions()
{
    struct TestCase {
        std::string input;
        Value leftValue;
        std::string operator_;
        Value rightValue;

    };

    std::vector<TestCase> tests
    {
            {"5 + 5;", 5, "+", 5},
            {"5 - 5;", 5, "-", 5},
            {"5 * 5;", 5, "*", 5},
            {"5 / 5;", 5, "/", 5},
            {"5 > 5;", 5, ">", 5},
            {"5 < 5;", 5, "<", 5},
            {"5 == 5;", 5, "==", 5},
            {"5 != 5;", 5, "!=", 5},
            {"foobar + barfoo;", "foobar", "+", "barfoo"},
            {"foobar - barfoo;", "foobar", "-", "barfoo"},
            {"foobar * barfoo;", "foobar", "*", "barfoo"},
            {"foobar / barfoo;", "foobar", "/", "barfoo"},
            {"foobar > barfoo;", "foobar", ">", "barfoo"},
            {"foobar < barfoo;", "foobar", "<", "barfoo"},
            {"foobar == barfoo;", "foobar", "==", "barfoo"},
            {"foobar != barfoo;", "foobar", "!=", "barfoo"},
            {"true == true", true, "==", true},
            {"true != false", true, "!=", false},
            {"false == false", false, "==", false},
    };

    for (auto &tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        if (program->statements_.size() != 1) {
            S << "ERROR" << E;
            exit(1);
        }

        auto *expr =dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
        if (!expr) {
            S << "ERROR" << E;
            exit(1);
        }
        auto* infixStmt = dynamic_cast<ast::InfixExpression*>(expr->expression_.get());
        if (!infixStmt) {
            S << "ERROR" << E;
            exit(1);
        }

        if (!testInfixExpression(infixStmt, tt.leftValue, tt.operator_, tt.rightValue)) {
            S << "ERROR" << E;
            exit(2);
        }
    }
}
void TestOperatorPrecedenceParsing() {
    struct TestCase {
        string input;
        string exptected;
    };

    std::vector<TestCase> tests =
    {
            {
                    "-a * b",
                    "((-a) * b)",
            },
            {
                    "!-a",
                    "(!(-a))",
            },
            {
                    "a + b + c",
                    "((a + b) + c)",
            },
            {
                    "a + b - c",
                    "((a + b) - c)",
            },
            {
                    "a * b * c",
                    "((a * b) * c)",
            },
            {
                    "a * b / c",
                    "((a * b) / c)",
            },
            {
                    "a + b / c",
                    "(a + (b / c))",
            },
            {
                    "a + b * c + d / e - f",
                    "(((a + (b * c)) + (d / e)) - f)",
            },
            {
                    "3 + 4; -5 * 5",
                    "(3 + 4)((-5) * 5)",
            },
            {
                    "5 > 4 == 3 < 4",
                    "((5 > 4) == (3 < 4))",
            },
            {
                    "5 < 4 != 3 > 4",
                    "((5 < 4) != (3 > 4))",
            },
            {
                    "3 + 4 * 5 == 3 * 1 + 4 * 5",
                    "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
            },
            {
                    "true",
                    "true",
            },
            {
                    "false",
                    "false",
            },
            {
                    "3 > 5 == false",
                    "((3 > 5) == false)",
            },
            {
                    "3 < 5 == true",
                    "((3 < 5) == true)",
            },
            {
                    "1 + (2 + 3) + 4",
                    "((1 + (2 + 3)) + 4)",
            },
            {
                    "(5 + 5) * 2",
                    "((5 + 5) * 2)",
            },
            {
                    "2 / (5 + 5)",
                    "(2 / (5 + 5))",
            },
            {
                    "(5 + 5) * 2 * (5 + 5)",
                    "(((5 + 5) * 2) * (5 + 5))",
            },
            {
                    "-(5 + 5)",
                    "(-(5 + 5))",
            },
            {
                    "!(true == true)",
                    "(!(true == true))",
            },
            {
                    "a + add(b * c) + d",
                    "((a + add((b * c))) + d)",
            },
            {
                    "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
                    "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
            },
            {
                    "add(a + b + c * d / f + g)",
                    "add((((a + b) + ((c * d) / f)) + g))",
            },
            {
                "5 + 6 + 7 + 8 -10",
                "((((5 + 6) + 7) + 8) - 10)",
            },
    };

    for (auto & tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        auto actual = program->String();
        if (actual != tt.exptected) {
            S << "ERROR " << actual << E;
            exit(1);
        }
    }
}

void TestBooleanExpression() {
    struct TestCase {
        string input;
        bool expectedBoolean;
    };

    std::vector<TestCase> tests {
            {"true", true},
            {"false", false},
    };

    for(auto & tt : tests) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();

        checkParserErrors(p);

        if (program->statements_.size() != 1) {
            S << "Error" << E;
            exit(1);
        }

        auto *expr = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
        if (!expr) {
            S << "ERROR" << E;
            exit(1);
        }

        auto * boolean = dynamic_cast<ast::Boolean*> (expr->expression_.get());
        if (!boolean) {
            S << "ERROR" << E;
        }

        if (boolean->value_ != tt.expectedBoolean) {
            S << "ERROR" << E;
        }

    }
}

bool testInfixExpression(ast::Expression* exp, const Value& left,std::string& oper, const Value& right) {
    auto *opExpr = dynamic_cast<ast::InfixExpression*>(exp);
    if (!opExpr) {
        return false;
    }
    if (!testLiteralExpression(opExpr->left_.get(), left)) {
        ERRINFO
        return false;
    }

    if (opExpr->operator_ != oper) {
        ERRINFO
        return false;
    }
    if (!testLiteralExpression(opExpr->right_.get(), right)) {
        ERRINFO
        return false;
    }

    return true;
}

void TestIfExpression() {
    string input = "if (x < y) { x }";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements_.size() != 1) {
        ERRINFO;
        exit(1);
    }

    auto *expr = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!expr) {
        ERRINFO;
        exit(1);
    }

    auto * ifexpr = dynamic_cast<ast::IfExpression*> (expr->expression_.get());
    if (!ifexpr) {
        ERRINFO;
        exit(1);
    }

    if (!testInfixExpression(ifexpr->condition_.get(), "x", "<", "y")) {
        ERRINFO
        exit(1);
    }

    if (ifexpr->consequence_->statements_.size() != 1) {
        ERRINFO
        exit(1);
    }

    auto* consequence = dynamic_cast<ast::ExpressionStatement*>(ifexpr->consequence_->statements_[0].get());
    if (!consequence) {
        ERRINFO
        exit(1);
    }
    if (!testIdentifier(consequence->expression_.get(), "x")) {
        ERRINFO
        exit(1);
    }

    if (ifexpr->alternative_.get() != nullptr) {
        ERRINFO
        exit(1);
    }

}

void TestIfElseExpression() {
    string input = "if (x < y) { x } else { y }";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements_.size() != 1) {
        ERRINFO
        exit(1);
    }

    auto *stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        ERRINFO
        exit(1);
    }

    auto* exp = dynamic_cast<ast::IfExpression*>(stmt->expression_.get());
    if (!exp) {
        ERRINFO
        exit(1);
    }

    if (!testInfixExpression(exp->condition_.get(), "x", "<", "y")) {
        ERRINFO
        exit(1);
    }

    if (exp->consequence_->statements_.size() != 1) {
        ERRINFO
        exit(1);
    }

    auto *consequence =  dynamic_cast<ast::ExpressionStatement*>(exp->consequence_->statements_[0].get());
    if (!consequence) {
        ERRINFO
        exit(1);
    }

    if (!testIdentifier(consequence->expression_.get(), "x")) {
        ERRINFO
        exit(1);
    }

    if (exp->alternative_->statements_.size() != 1) {
        ERRINFO
        exit(1);
    }

    auto *alternative =  dynamic_cast<ast::ExpressionStatement*>(exp->alternative_->statements_[0].get());
    if (!alternative) {
        ERRINFO
        exit(1);
    }

    if (!testIdentifier(alternative->expression_.get(), "y")) {
        ERRINFO
        exit(1);
    }
}

void TestFunctionLiteralParsing(TestingT &t) {
    std::string input("fn(x, y) { x + y; }");
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements_.size() != 1) {
        ERRINFO
        exit(1);
    }

    auto* stmt = dynamic_cast<ast::ExpressionStatement*> (program->statements_[0].get());
    if (!stmt) {
        ERRINFO
        exit(1);
    }

    auto *func = dynamic_cast<ast::FunctionLiteral*>(stmt->expression_.get());
    if (!func) {
        ERRINFO
        exit(1);
    }

    if (func->parameters_.size() != 2) {
        ERRINFO
        exit(1);
    }

    if (!testLiteralExpression(func->parameters_[0].get(), "x")) {
        t.Fatalf("func->parameters_[0].get() should be x");
        exit(1);
    }

    if (!testLiteralExpression(func->parameters_[1].get(), "y")) {
        t.Fatalf("func->parameters_[1].get() should be y");
        exit(1);
    }

    if (func->body_->statements_.size() != 1) {
        t.Fatalf("function.Body.Statements has not 1 statements. got=%d\n", func->body_->statements_.size());
    }

    auto* bodyStatement = dynamic_cast<ast::ExpressionStatement*>(func->body_->statements_[0].get());
    if (!bodyStatement) {
        t.Fatalf("body.Statements should not be NULL");
    }

    if (!testInfixExpression(bodyStatement->expression_.get(), "x", "+", "y")) {
        t.Fatalf("%s", R"(bodyStatement->expression_.get(), x, +, y)");
    }
}

void TestFunctionParameterParsing(TestingT &t) {
    struct TestCase {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<TestCase> testCases = {
    {"fn() {};",std::vector<std::string> () },
    {"fn(x) {};", std::vector<std::string>{"x"}},
    {"fn(x, y, z) {};", std::vector<std::string> {"x", "y", "z"}},
    };

    for (auto &tt :testCases) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        auto* stmt =dynamic_cast<ast::ExpressionStatement*>( program->statements_[0].get());
        if (!stmt) {
            t.Fatalf("stmt.Statements should not be NULL");
        }
        auto* func = dynamic_cast<ast::FunctionLiteral*>(stmt->expression_.get());
        if (!func) {
            t.Fatalf("func.Statements should not be NULL");
        }

        if (func->parameters_.size() != tt.expectedParams.size()) {
            t.Fatalf("func.Parameters has not the same number of arguments");
        }

        for (decltype(tt.expectedParams.size())  i = 0; i < tt.expectedParams.size(); ++i) {
            testLiteralExpression(func->parameters_[i].get(), tt.expectedParams[i]);
        }
    }
}

void TestCallExpressionParsing(TestingT &t) {
    std::string input = "add(1, 2 * 3, 4 + 5);";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements_.size() != 1) {
        t.Fatalf("program.Statements does not contain %d statements. got=%d\n", 1, program->statements_.size());
    }

    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt should not be NULL");
    }

    auto* exp = dynamic_cast<ast::CallExpression*>(stmt->expression_.get());
    if (!exp) {
        t.Fatalf("exp should not be NULL");
    }

    if (!testIdentifier(exp->function_.get(), "add")) {
        t.Fatalf("");
    }

    if (exp->arguments_.size() != 3) {
        t.Fatalf("exp->arguments_.size() should be 3");
    }

    if (exp->arguments_.size() != 3) {
        t.Fatalf("wrong length of arguments. got=%d", exp->arguments_.size());
    }

    if (!testLiteralExpression(exp->arguments_[0].get(), 1)) {
        t.Fatalf("");
    }
    if (!testInfixExpression(exp->arguments_[1].get(), 2, "*", 3)) {
        t.Fatalf("");
    }

    if (!testInfixExpression(exp->arguments_[2].get(), 4, "+", 5)) {
        t.Fatalf("");
    }
}

void TestCallExpressionParameterParsing(TestingT &t) {
    struct TestCase {
        std::string input;
        std::string expectedIdent;
        std::vector<std::string> expectedArgs;
    };

    std::vector<TestCase> testCases = {
        {"add();", "add", std::vector<std::string>()},
        {"add(1);", "add", std::vector<std::string>{"1"}},
        {"add(1, 2 * 3, 4 + 5);", "add", std::vector<std::string>{"1", "(2 * 3)", "(4 + 5)"}},
    };

    for (auto &tt :testCases) {
        lexer::Lexer l(tt.input);
        parser::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
        if (!stmt) {
            t.Fatalf("stmt.Statements should not be NULL");
        }

        auto* exp = dynamic_cast<ast::CallExpression*>(stmt->expression_.get());
        if (!exp) {
            t.Fatalf("exp should not be NULL");
        }

        if (!testIdentifier(exp->function_.get(), tt.expectedIdent)) {
            t.Fatalf("exp->function_.get() should be %s", tt.expectedIdent.c_str());
        }
        if (exp->arguments_.size() != tt.expectedArgs.size()) {
            t.Fatalf("");
        }

        for (decltype(tt.expectedArgs.size())  i = 0; i < tt.expectedArgs.size(); ++i) {
            if (exp->arguments_[i]->String() != tt.expectedArgs[i]) {
                t.Fatalf("");
            }
        }
    }
}

void TestStringLiteralExpression(TestingT &t) {
    string input = R"("hello world";)";

    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto *literal = dynamic_cast<ast::StringLiteral*> (stmt->expression_.get());
    if (!literal) {
        t.Fatalf("TODO");
    }

    if (literal->value_ != "hello world") {
        t.Fatalf("%s should be %s", literal->value_.c_str(), "hello world");
    }
}

// 单测数组
// -- 单测空数组
void TestParsingEmptyArrayLiterals(TestingT & t)
{
    string input = "[]";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();

    checkParserErrors(p);

    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto *array = dynamic_cast<ast::ArrayLiteral*>(stmt->expression_.get());
    if (!array) {
        t.Fatalf("array should not be null");
    }

    if (array->elements_.size() != 0) {
        t.Fatalf("array size should be 0");
    }
}

// -- 测试数组非空的情况
void TestParsingArrayLiterals(TestingT &t)
{
    string input = R"([1, 2 * 2, 3 + 3])";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();

    checkParserErrors(p);
    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto *array = dynamic_cast<ast::ArrayLiteral*>(stmt->expression_.get());
    if (!array) {
        t.Fatalf("array should not be null");
    }

    if (array->elements_.size() != 3) {
        t.Fatalf("array size should be 3");
    }

    testIntergerLiteral(array->elements_[0].get(), 1);
    testInfixExpression(array->elements_[1].get(), 2, "*", 2);
    testInfixExpression(array->elements_[2].get(), 3, "+", 3);
}

// --- 增加数组索引测试
void TestParsingIndexExpressions(TestingT &t)
{
    string input = R"(myArray[1 + 1])";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();

    checkParserErrors(p);
    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto *index = dynamic_cast<ast::IndexExpression*>(stmt->expression_.get());
    if (!index) {
        t.Fatalf("array should not be null");
    }

    if (!testIdentifier(index->left_.get(), "myArray")) {
        t.Fatalf("index error.");
    }

    if (!testInfixExpression(index->index_.get(), 1, "+", 1)) {
        t.Fatalf("index error.");
    }
}
// 单测hash 表
// -- 空 Hash 表
void TestParsingEmptyHashLiteral(TestingT &t)
{
    string input = R"({})";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();

    checkParserErrors(p);
    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto* hash = dynamic_cast<ast::HashLiteral*>(stmt->expression_.get());
    if (!hash) {
        t.Fatalf("hash should not be null");
    }

    if (hash->pairs_.size() != 0) {
        t.Fatalf("hash size should not be 0");
    }
}

void TestParsingHashLiteralsStringKeys(TestingT &t) {
    string input = R"({"one": 1, "two" : 2})";
    lexer::Lexer l(input);
    parser::Parser p(l);
    auto program = p.parseProgram();

    checkParserErrors(p);
    auto* stmt = dynamic_cast<ast::ExpressionStatement*>(program->statements_[0].get());
    if (!stmt) {
        t.Fatalf("stmt.Statements should not be NULL");
    }

    auto* mm = dynamic_cast<ast::HashLiteral*>(stmt->expression_.get());
    if (!mm) {
        t.Fatalf("mm should not be null");
    }

    std::map<string, int> expected= {
            {"one" , 1},
            {"two", 2},
    };

    if (mm->pairs_.size() != expected.size()) {
        t.Fatalf("size not equal");
    }

    for (const auto &pair : mm->pairs_) {
        auto* key = dynamic_cast<ast::StringLiteral*>(pair.first.get());
        auto* value = dynamic_cast<ast::IntegerLiteral*>(pair.second.get());
//        if (expected[key->value_] != value->value_) {
//
//        }
        testIntergerLiteral(value, expected[key->value_]);
    }
}
void ParserTest()
{
    TestingT t;
//    TestLetStatements();
//    TestReturnStatements();
//    TestIdentifierExpression();
//    TestIntegerLiteralExpression();
//    TestParsingPrefixExpressions();
//    TestParsingInfixExpressions();
//    TestOperatorPrecedenceParsing();
//    TestBooleanExpression();
//    TestIfElseExpression();
//    TestIfExpression();
//    TestFunctionParameterParsing(t);
//    TestFunctionLiteralParsing(t);
//    TestCallExpressionParsing(t);
//    TestCallExpressionParameterParsing(t);
//    TestStringLiteralExpression(t);
//    TestParsingEmptyArrayLiterals(t);
//    TestParsingArrayLiterals(t);
//    TestParsingIndexExpressions(t);
//    TestParsingEmptyHashLiteral(t);
    TestParsingHashLiteralsStringKeys(t);
}

