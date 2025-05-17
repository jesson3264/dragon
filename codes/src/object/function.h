
#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "object.h"
#include "ast.h"
#include "environment.hpp"
namespace dragon {
namespace object {
class Function : public Object {
public:
    Function(std::vector<std::shared_ptr<ast::Identifier>> parameters, 
        std::shared_ptr<ast::BlockStatement> body, 
        std::shared_ptr<Environment> env)
        : parameters_(parameters), body_(body), env_(env) {}
    ObjectType Type() const override { return ObjectType::FUNCTION_OBJ; }
    std::string Inspect() const override {
        std::ostringstream out;
        out << "fn(";
        for (size_t i = 0; i < parameters_.size(); ++i) {
            if (i > 0) out << ", ";
            out << parameters_[i]->String();
        }
        out << ") {\n";
        out << body_->String() << "\n}";
        return out.str();
    }
    
public:
    std::vector<std::shared_ptr<ast::Identifier>>  parameters_;
    std::shared_ptr<ast::BlockStatement> body_;
    std::shared_ptr<Environment> env_;
};
} // namespace object
} // namespace dragon
#endif  