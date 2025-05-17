#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#include "object.h"
#include "ast.h"
#include "environment.hpp"
#include "function.h"
#include <memory>
#include <vector>

#define NULL_OBJ_INS  std::make_shared<object::Null>()
namespace dragon {
namespace evaluator {
class Evaluator {
public:
    static std::shared_ptr<object::Object> eval(const std::shared_ptr<ast::Node>& node, 
                                      const std::shared_ptr<Environment>& env);

private:
    static std::shared_ptr<object::Object> evalProgram(const std::shared_ptr<ast::Program>& program,
                                             const std::shared_ptr<Environment>& env);
    
    static std::shared_ptr<object::Object> evalBlockStatement(const std::shared_ptr<ast::BlockStatement>& block,
                                                    const std::shared_ptr<Environment>& env);
    
    static std::shared_ptr<object::Object> evalPrefixExpression(const std::string& op,
                                                      const std::shared_ptr<object::Object>& right);
    
    static std::shared_ptr<object::Object> evalInfixExpression(const std::string& op,
                                                     const std::shared_ptr<object::Object>& left,
                                                     const std::shared_ptr<object::Object>& right);
    
    static std::shared_ptr<object::Object> evalIntegerInfixExpression(const std::string& op,
                                                            const std::shared_ptr<object::Object>& left,
                                                            const std::shared_ptr<object::Object>& right);
    static std::shared_ptr<object::Object> evalStringInfixExpression(const string & oper,
                                                                     const std::shared_ptr<object::Object> &left,
                                                                     const std::shared_ptr<object::Object> &right);
    static std::shared_ptr<object::Object> evalIfExpression(const std::shared_ptr<ast::IfExpression>& ie,
                                                  const std::shared_ptr<Environment>& env);
    
    static std::shared_ptr<object::Object> evalIdentifier(const std::shared_ptr<ast::Identifier>& node,
                                                const std::shared_ptr<Environment>& env);

    static std::shared_ptr<object::Object> evalArrayIndexExpression(const std::shared_ptr<object::Object> &array,
                                                                    const std::shared_ptr<object::Object> &index);
    static std::shared_ptr<object::Object> evalHashIndexExpression(const std::shared_ptr<object::Object> &hash,
                                                                   const std::shared_ptr<object::Object> &index);
    static std::shared_ptr<object::Object> evalIndexExpression(std::shared_ptr<object::Object>  &obj,
                                                                   const std::shared_ptr<object::Object>&node);

    static std::shared_ptr<object::Object> evalHashLiteral(const std::shared_ptr<ast::HashLiteral>& ie,
                                                            const std::shared_ptr<Environment>& env);
    static std::vector<std::shared_ptr<object::Object>> evalExpressions(
        const std::vector<std::shared_ptr<ast::Expression>>& exps,
        const std::shared_ptr<Environment>& env);

    static std::shared_ptr<object::Object> applyFunction(const std::shared_ptr<object::Object>& fn,
                                               const std::vector<std::shared_ptr<object::Object>>& args);
    
    static std::shared_ptr<Environment> extendFunctionEnv(
        const std::shared_ptr<object::Function>& fn,
        const std::vector<std::shared_ptr<object::Object>>& args);
    
    static std::shared_ptr<object::Object> unwrapReturnValue(const std::shared_ptr<object::Object>& obj);
    
    static bool isTruthy(const std::shared_ptr<object::Object>& obj);
    static bool isError(const std::shared_ptr<object::Object>& obj);
    static std::shared_ptr<object::Error> newError(const char* format, ...);
};
} // namespace evaluator
} // namespace dragon

#endif
