//
// Created by Jesson on 2025/5/11.
//

#ifndef MYPROJECT_PARSER_H
#define MYPROJECT_PARSER_H

#include "ast.h"
#include "lexer.h"

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <functional>
#include <cstdlib> // for strtol

namespace parser
{
// 优先级
enum Precedence {
    LOWEST,
    EQUALS, // ==
    LESSGREATER, // > or <
    SUM, // +
    PRODUCT, // *
    PREFIX, // -X or !X
    CALL, // myFunction(X)
    INDEX // array[index]
};

extern std::map<token::TokenType, Precedence> precedences;
class Parser {
public:
    using PrefixParseFn = std::function<std::unique_ptr<ast::Expression>()>;
    using InfixParseFn = std::function<std::unique_ptr<ast::Expression>(std::unique_ptr<ast::Expression>)>;
    explicit Parser(lexer::Lexer& l) : lexer(l) {
        // Initialize prefix parse functions
        registerPrefix(IDENT, [this]() { return parseIdentifier(); });
        registerPrefix(INT, [this]() { return parseIntegerLiteral(); });
        registerPrefix(STRING, [this](){return parseStringLiteral();});
        registerPrefix(BANG, [this]() { return parsePrefixExpression(); });
        registerPrefix(MINUS, [this]() { return parsePrefixExpression(); });
        registerPrefix(TRUE, [this]() { return parseBoolean(); });
        registerPrefix(FALSE, [this]() { return parseBoolean(); });
        registerPrefix(LPAREN, [this]() { return parseGroupedExpression(); });
        registerPrefix(IF, [this]() { return parseIfExpression(); });
        registerPrefix(FUNCTION, [this]() { return parseFunctionLiteral(); });
        registerPrefix(LBRACKET, [this]() { return parseArrayLiteral(); } ); //
        registerPrefix(LBRACE, [this](){return parseHashLiteral();});

        // Initialize infix parse functions
        registerInfix(PLUS, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(MINUS, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(SLASH, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(ASTERISK, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(EQ, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(NOT_EQ, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(LT, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(GT, [this](auto left) { return parseInfixExpression(std::move(left)); });
        registerInfix(LPAREN, [this](auto left) { return parseCallExpression(std::move(left)); });
        registerInfix(LBRACKET, [this](auto left){return parseIndexExpression(std::move(left));});
        // Read two tokens to initialize
        nextToken();
        nextToken();
    }

    std::shared_ptr<ast::Program> parseProgram() {
        auto program = std::make_shared<ast::Program>();

        while (currentToken.Type != MEOF) {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements_.push_back(std::move(stmt));
            }
            nextToken();
        }

        return program;
    }

    const std::vector<std::string>& getErrors() const { return errors; }

    lexer::Lexer& lexer;
    std::vector<std::string> errors;
    token::Token currentToken;
    token::Token peekToken;

    std::map<token::TokenType, PrefixParseFn> prefixParseFns;
    std::map<token::TokenType, InfixParseFn> infixParseFns;

    void nextToken() {
        currentToken = peekToken;
        peekToken = lexer.NextToken();
    }

    bool currentTokenIs(token::TokenType t) const {
        return currentToken.Type == t;
    }

    bool peekTokenIs(token::TokenType t) const {
        return peekToken.Type == t;
    }

    bool expectPeek(token::TokenType t) {
        if (peekTokenIs(t)) {
            nextToken();
            return true;
        } else {
            peekError(t);
            return false;
        }
    }

    void peekError(token::TokenType t) {
        errors.push_back("expected next token to be " +
                         t +
                         ", got " +
                         peekToken.Type +
                         " instead");
    }

    void noPrefixParseFnError(token::TokenType t) {
        errors.push_back("no prefix parse function for " +
                         t +
                         " found");
    }

    std::unique_ptr<ast::Statement> parseStatement() {
        if (currentToken.Type == LET) {
            return parseLetStatement();
        } else if (currentToken.Type == RETURN) {
            return parseReturnStatement();
        }
        else
        {
            return parseExpressionStatement();
        }
    }

    std::unique_ptr<ast::LetStatement> parseLetStatement() {
        auto stmt = std::make_unique<ast::LetStatement>();
        stmt->token_ = currentToken;

        if (!expectPeek(IDENT)) {
            return nullptr;
        }

        stmt->name_ = std::make_unique<ast::Identifier>();
        stmt->name_->token_ = currentToken;
        stmt->name_->value_ = currentToken.Literal;

        if (!expectPeek(ASSIGN)) {
            return nullptr;
        }

        nextToken();
        stmt->value_ = parseExpression(LOWEST);

        if (peekTokenIs(SEMICOLON)) {
            nextToken();
        }

        return stmt;
    }

    std::unique_ptr<ast::ReturnStatement> parseReturnStatement() {
        auto stmt = std::make_unique<ast::ReturnStatement>();
        stmt->token_ = currentToken;

        nextToken();
        stmt->returnValue_ = parseExpression(LOWEST);

        if (peekTokenIs(SEMICOLON)) {
            nextToken();
        }

        return stmt;
    }

    std::unique_ptr<ast::ExpressionStatement> parseExpressionStatement() {
        auto stmt = std::make_unique<ast::ExpressionStatement>();
        stmt->token_ = currentToken;
        stmt->expression_ = parseExpression(LOWEST);

        if (peekTokenIs(SEMICOLON)) {
            nextToken();
        }

        return stmt;
    }

    std::unique_ptr<ast::Expression> parseExpression(Precedence precedence) {
        auto prefix = prefixParseFns[currentToken.Type];
        if (!prefix) {
            noPrefixParseFnError(currentToken.Type);
            return nullptr;
        }

        auto leftExp = prefix();

        while (!peekTokenIs(SEMICOLON) && precedence < peekPrecedence()) {
            auto infix = infixParseFns[peekToken.Type];
            if (!infix) {
                return leftExp;
            }

            nextToken();
            leftExp = infix(std::move(leftExp));
        }

        return leftExp;
    }

    Precedence peekPrecedence() const {
        auto it = precedences.find(peekToken.Type);
        if (it != precedences.end()) {
            return it->second;
        }
        return LOWEST;
    }

    Precedence currentPrecedence() const {
        auto it = precedences.find(currentToken.Type);
        if (it != precedences.end()) {
            return it->second;
        }
        return LOWEST;
    }

    std::unique_ptr<ast::Expression> parseIdentifier() {
        auto ident = std::make_unique<ast::Identifier>();
        ident->token_ = currentToken;
        ident->value_ = currentToken.Literal;
        return ident;
    }

    std::unique_ptr<ast::Expression> parseIntegerLiteral() {
        auto lit = std::make_unique<ast::IntegerLiteral>();
        lit->token_ = currentToken;

        try {
            lit->value_ = std::stol(currentToken.Literal);
        } catch (const std::invalid_argument&) {
            errors.push_back("could not parse " + currentToken.Literal + " as integer");
            return nullptr;
        } catch (const std::out_of_range&) {
            errors.push_back("integer " + currentToken.Literal + " is out of range");
            return nullptr;
        }

        return lit;
    }

    std::unique_ptr<ast::Expression> parseStringLiteral() {
        auto s = std::make_unique<ast::StringLiteral>();
        s->token_ = currentToken;
        s->value_ = currentToken.Literal;
        return s;
    }

    std::unique_ptr<ast::Expression> parsePrefixExpression() {
        auto expr = std::make_unique<ast::PrefixExpression>();
        expr->token_ = currentToken;
        expr->operator_ = currentToken.Literal;

        nextToken();
        expr->right_ = parseExpression(PREFIX);

        return expr;
    }

    std::unique_ptr<ast::Expression> parseInfixExpression(std::unique_ptr<ast::Expression> left) {
        auto expr = std::make_unique<ast::InfixExpression>();
        expr->token_ = currentToken;
        expr->operator_ = currentToken.Literal;
        expr->left_ = std::move(left);

        auto precedence = currentPrecedence();
        nextToken();
        expr->right_ = parseExpression(precedence);

        return expr;
    }

    std::unique_ptr<ast::Expression> parseBoolean() {
        auto boolean = std::make_unique<ast::Boolean>();
        boolean->token_ = currentToken;
        boolean->value_ = currentTokenIs(TRUE);
        return boolean;
    }

    std::unique_ptr<ast::Expression> parseGroupedExpression() {
        nextToken();
        auto expr = parseExpression(LOWEST);

        if (!expectPeek(RPAREN)) {
            return nullptr;
        }

        return expr;
    }

    std::unique_ptr<ast::Expression> parseIfExpression() {
        auto expr = std::make_unique<ast::IfExpression>();
        expr->token_ = currentToken;

        if (!expectPeek(LPAREN)) {
            return nullptr;
        }

        nextToken();
        expr->condition_ = parseExpression(LOWEST);

        if (!expectPeek(RPAREN)) {
            return nullptr;
        }

        if (!expectPeek(LBRACE)) {
            return nullptr;
        }

        expr->consequence_ = parseBlockStatement();

        if (peekTokenIs(ELSE)) {
            nextToken();

            if (!expectPeek(LBRACE)) {
                return nullptr;
            }

            expr->alternative_ = parseBlockStatement();
        }

        return expr;
    }

    std::unique_ptr<ast::BlockStatement> parseBlockStatement() {
        auto block = std::make_unique<ast::BlockStatement>();
        block->token_ = currentToken;

        nextToken();

        while (!currentTokenIs(RBRACE) && !currentTokenIs(MEOF)) {
            auto stmt = parseStatement();
            if (stmt) {
                block->statements_.push_back(std::move(stmt));
            }
            nextToken();
        }

        return block;
    }

    std::unique_ptr<ast::Expression> parseFunctionLiteral() {
        auto lit = std::make_unique<ast::FunctionLiteral>();
        lit->token_ = currentToken;

        if (!expectPeek(LPAREN)) {
            return nullptr;
        }

        lit->parameters_ = parseFunctionParameters();

        if (!expectPeek(LBRACE)) {
            return nullptr;
        }

        lit->body_ = parseBlockStatement();

        return lit;
    }

    std::unique_ptr<ast::Expression> parseArrayLiteral() {
        std::unique_ptr<ast::ArrayLiteral>  al = std::make_unique<ast::ArrayLiteral>();
        al->token_ = currentToken;
        al->elements_ = parseExpressionList(RBRACKET);
        return al;
    }

    std::unique_ptr<ast::Expression> parseHashLiteral() {
        std::unique_ptr<ast::HashLiteral> hl = std::make_unique<ast::HashLiteral>();
        hl->token_ = currentToken;
        while (!peekTokenIs(RBRACE)) { // 没有取到右括号 )，则已知进行解析
            nextToken();
            std::shared_ptr<ast::Expression> key = parseExpression(LOWEST);
            // 跳过 :
            if (!expectPeek(COLON)) {
                return nullptr;
            }
            nextToken();
            std::shared_ptr<ast::Expression> value = parseExpression(LOWEST);

            hl->pairs_[key] = value;

            if (!peekTokenIs(RBRACE) && !expectPeek(COMMA)) {
                return nullptr;
            }
        }

        if (!expectPeek(RBRACE)) {
            return nullptr;
        }
        return hl;
    }

    std::vector<std::shared_ptr<ast::Identifier>> parseFunctionParameters() {
        std::vector<std::shared_ptr<ast::Identifier>> params;

        if (peekTokenIs(RPAREN)) {
            nextToken();
            return params;
        }

        nextToken();

        auto ident = std::make_unique<ast::Identifier>();
        ident->token_ = currentToken;
        ident->value_ = currentToken.Literal;
        params.push_back(std::move(ident));

        while (peekTokenIs(COMMA)) {
            nextToken();
            nextToken();

            auto identi = std::make_unique<ast::Identifier>();
            identi->token_ = currentToken;
            identi->value_ = currentToken.Literal;
            params.push_back(std::move(identi));
        }

        if (!expectPeek(RPAREN)) {
            return {};
        }

        return params;
    }

    std::unique_ptr<ast::Expression> parseCallExpression(std::unique_ptr<ast::Expression> function) {
        auto expr = std::make_unique<ast::CallExpression>();
        expr->token_ = currentToken;
        expr->function_ = std::move(function);
        expr->arguments_ = parseCallArguments();
        return expr;
    }

    std::unique_ptr<ast::Expression> parseIndexExpression(std::unique_ptr<ast::Expression> left) {
        auto expr = std::make_unique<ast::IndexExpression>();
        expr->token_ = currentToken;
        expr->left_ = std::move(left);
        nextToken();
        expr->index_ = parseExpression(LOWEST);
        if (!expectPeek(RBRACKET)) {
            return nullptr;
        }

        return expr;
    }
    std::vector<std::shared_ptr<ast::Expression>> parseExpressionList(token::TokenType end) {
        std::vector<std::shared_ptr<ast::Expression>> eles;
        if (peekTokenIs(end)) {
            nextToken();
            return eles;
        }

        nextToken();

        eles.push_back(parseExpression(LOWEST));

        while (peekTokenIs(COMMA)) {
            nextToken();
            nextToken();
            eles.push_back(parseExpression(LOWEST));
        }

        if (!expectPeek(end)) {
            return {}; // 直接返回空
        }

        return eles;
    }

    std::vector<std::shared_ptr<ast::Expression>> parseCallArguments() {
        std::vector<std::shared_ptr<ast::Expression>> args;

        if (peekTokenIs(RPAREN)) {
            nextToken();
            return args;
        }

        nextToken();
        args.push_back(parseExpression(LOWEST));

        while (peekTokenIs(COMMA)) {
            nextToken();
            nextToken();
            args.push_back(parseExpression(LOWEST));
        }

        if (!expectPeek(RPAREN)) {
            return {};
        }

        return args;
    }



    void registerPrefix(token::TokenType type, PrefixParseFn fn) {
        prefixParseFns[type] = fn;
    }

    void registerInfix(token::TokenType type, InfixParseFn fn) {
        infixParseFns[type] = fn;
    }
};
} // namespace parser
#endif //MYPROJECT_PARSER_H
