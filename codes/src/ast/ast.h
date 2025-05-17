//
// Created by Jesson on 2025/4/25.
//

#ifndef MYPROJECT_AST_H
#define MYPROJECT_AST_H
#include "token.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>
#include <map>

using std::string;
using std::vector;
namespace ast {
class Node {
public:
    virtual ~Node() = default;
    virtual string TokenLiteral() const = 0;
    virtual string String() const = 0;
};

class Statement : public Node {
public:
    virtual void statementNode() = 0;
};

class Expression : public Node {
public:
    Expression() {};
    virtual void expressionNode() = 0;
};

class Program :public Node {
public:
    Program() = default;

    string TokenLiteral() const{
        if (statements_.size()) {
            return statements_[0]->TokenLiteral();
        } else {
            return "";
        }
    }

    string String() const
    {
        std::stringstream out;
        for (auto &v : statements_)
        {
            out << v->String();
        }
        return out.str();
    }
public:
    vector<std::shared_ptr<Statement>> statements_;
};

class Identifier : public Expression {
public:
    Identifier() = default;
    Identifier(const token::Token& t, const string& v) {
        token_ = t;
        value_ = v;
    }
    void expressionNode() override {}
    std::string TokenLiteral() const override { return token_.Literal;}
    std::string String() const override {return value_;}
public:
    token::Token token_;
    string value_;
};


class LetStatement :public Statement {
public:
//    LetStatement() {}
    LetStatement() {};
    void statementNode() override {}
    std::string TokenLiteral() const override {
        return token_.Literal;
    }

    std::string String() const override {
        std::stringstream out;
        out << TokenLiteral() << " " << name_->String() << " = ";
        if (value_) {
            out << value_->String();
        }
        out << ";";
        return out.str();
    }
    ~LetStatement() override = default;
public:
    token::Token token_;
    std::shared_ptr<Identifier> name_;
    std::shared_ptr<Expression> value_;
};


class ReturnStatement: public Statement
{
public:
    token::Token token_;
    std::shared_ptr<Expression> returnValue_;

    void statementNode() override {}
    string TokenLiteral() const override {return token_.Literal;}
    string String() const override {
        std::stringstream out;
        out << TokenLiteral() << " ";
        if (returnValue_) {
            out << returnValue_->String();
        }
        out << ";";
        return out.str();
    }
};

class ExpressionStatement: public Statement {
public:
    token::Token token_;
    std::shared_ptr<Expression> expression_;

    void statementNode() override{}
    std::string TokenLiteral() const override {return token_.Literal;}
    std::string String() const override {
        if (expression_) {
            return expression_->String();
        }
        return "";
    }
};

class BlockStatement: public Statement {
public:
    token::Token token_;

    std::vector<std::shared_ptr<Statement>> statements_;
    void statementNode() override {}
    std::string TokenLiteral()const override{return token_.Literal;}

    std::string String() const override {
        std::stringstream out;
        for (const auto & s : statements_)
        {
            out << s->String();
        }

        return out.str();
    }
};


class Boolean : public Expression {
public:
    token::Token token_;
    bool value_;

    void expressionNode() override {}
    std::string TokenLiteral() const override {return token_.Literal;}
    std::string String() const override {return token_.Literal;}
};

class IntegerLiteral : public Expression {
public:
    token::Token token_;
    int64_t value_;

    void expressionNode() override {}

    std::string TokenLiteral() const override { return token_.Literal;}

    std::string String() const override {return token_.Literal;}
};

// 字符串
class StringLiteral : public Expression {
public:
    token::Token token_;
    string value_;
    void expressionNode() override {}
    std::string TokenLiteral() const override {
        return token_.Literal;
    }

    std::string String() const override {
        return token_.Literal;
    }
};

// 数组
class ArrayLiteral : public Expression {
public:
    ArrayLiteral(){}
    ArrayLiteral(const std::vector<std::shared_ptr<Expression>> & eles) :elements_(eles) {

    }
    token::Token token_;
    std::vector<std::shared_ptr<Expression>> elements_;
    void expressionNode() override {}
    std::string TokenLiteral() const override {
        return token_.Literal;
    }

    std::string String() const override {
        std::stringstream out;
        std::vector<string> all;
        // Sample : [1, 2]
        out << "[";
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (i != 0) {
                out << ", ";
            }
            out << elements_[i]->String();
        }
        out << "]";
        return out.str();
    }
};

// 索引表达式
class IndexExpression : public Expression {
public:
    token::Token token_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> index_;

    void expressionNode() override {}
    std::string TokenLiteral()const override {
        return token_.Literal;
    }
    std::string String() const override {
        std::stringstream out;
        out << "(";
        out << left_->String();
        out << "[";
        out << index_->String();
        out << "])";
        return out.str();
    }
};

class HashLiteral : public Expression {
public:
    token::Token token_;
    std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs_;

    void expressionNode() override {}
    std::string TokenLiteral()const override {
        return token_.Literal;
    }

    std::string String() const override {
        std::stringstream out;
        out << "{";
        bool first = true;
        for (const auto & pair : pairs_) {
            if (!first) {
                out << ", ";
            }
            first = false;
            out << pair.first->String() << ":" << pair.second->String();
        }

        out << "}";
        return out.str();
    }
};

// 前缀表达式
class PrefixExpression : public Expression {
public:
    token::Token token_;
    std::string operator_;
    std::shared_ptr<Expression> right_;

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token_.Literal;}
    std::string String() const override {
        std::stringstream out;
        out << "(" << operator_ << right_->String() << ")";
        return out.str();
    }
};

class InfixExpression : public Expression {
public:
    token::Token token_;
    std::shared_ptr<Expression> left_;
    std::string operator_;
    std::shared_ptr<Expression> right_;
    void expressionNode() override {}
    std::string TokenLiteral() const override { return token_.Literal;}

    std::string String() const override {
        std::stringstream out;
        out << "(" << left_->String() << " " << operator_ << " " << right_->String() << ")";
        return out.str();
    }
};

class IfExpression : public Expression {
public:
    token::Token token_;  // The 'if' token
    std::shared_ptr<Expression> condition_;
    std::shared_ptr<BlockStatement> consequence_;
    std::shared_ptr<BlockStatement> alternative_;

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token_.Literal; }
    std::string String() const override {
        std::stringstream out;
        out << "if" << condition_->String() << " " << consequence_->String();
        if (alternative_) {
            out << "else " << alternative_->String();
        }
        return out.str();
    }
};

class FunctionLiteral : public Expression {
public:
    token::Token token_;
    std::vector<std::shared_ptr<Identifier>> parameters_;
    std::shared_ptr<BlockStatement> body_;

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token_.Literal;}
    std::string String() const override {
        std::stringstream out;
        std::vector<std::string> params;
        for (const auto & p : parameters_) {
            params.push_back(p->String());
        }
        return out.str();
    }
};

class CallExpression : public Expression {
public:
    token::Token token_;
    std::shared_ptr<Expression> function_;
    std::vector<std::shared_ptr<Expression>> arguments_;

public:
    void expressionNode() override {}
    std::string TokenLiteral() const override {return token_.Literal;}
    std::string String() const override {
        std::stringstream out;
        std::vector<std::string> args;
        for (const auto& a : arguments_) {
            args.push_back(a->String());
        }

        out << function_->String() << "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) out << ", ";
            out << args[i];
        }
        out << ")";
        return out.str();
    }
};
} // namespace node
#endif //MYPROJECT_AST_H
