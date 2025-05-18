#include "evaluator.h"
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include "builtin.h"
namespace dragon {
namespace evaluator {

#define TRUE_OBJ  std::make_shared<object::Boolean>(true)
#define FALSE_OBJ std::make_shared<object::Boolean>(false)

std::shared_ptr<object::Object> Evaluator::eval(const std::shared_ptr<ast::Node>& node,
                                      const std::shared_ptr<Environment>& env) {
    if (auto program = std::dynamic_pointer_cast<ast::Program>(node)) {
        return evalProgram(program, env);
    }
    if (auto block = std::dynamic_pointer_cast<ast::BlockStatement>(node)) {
        return evalBlockStatement(block, env);
    }
    if (auto expr = std::dynamic_pointer_cast<ast::ExpressionStatement>(node)) {
        return eval(expr->expression_, env);
    }
    if (auto ret = std::dynamic_pointer_cast<ast::ReturnStatement>(node)) {
        auto val = eval(ret->returnValue_, env);
        if (isError(val)) return val;
        return std::make_shared<object::ReturnValue>(val);
    }
    if (auto let = std::dynamic_pointer_cast<ast::LetStatement>(node)) {
        auto val = eval(let->value_, env);
        if (isError(val)) return val;
        env->set(let->name_->value_, val);
        return nullptr;
    }
    if (auto integer = std::dynamic_pointer_cast<ast::IntegerLiteral>(node)) {
        return std::make_shared<object::Integer>(integer->value_);
    }

    if (auto str = std::dynamic_pointer_cast<ast::StringLiteral>(node)) {
        return std::make_shared<object::String>(str->value_);
    }
    if (auto boolean = std::dynamic_pointer_cast<ast::Boolean>(node)) {
        if (boolean->value_) return std::make_shared<object::Boolean>(true);
        else return std::make_shared<object::Boolean>(false);
    }
    if (auto prefix = std::dynamic_pointer_cast<ast::PrefixExpression>(node)) {
        auto right = eval(prefix->right_, env);
        if (isError(right)) return right;
        return evalPrefixExpression(prefix->operator_, right);
    }
    if (auto infix = std::dynamic_pointer_cast<ast::InfixExpression>(node)) {
        auto left = eval(infix->left_, env);
        if (isError(left)) return left;
        
        auto right = eval(infix->right_, env);
        if (isError(right)) return right;
        
        return evalInfixExpression(infix->operator_, left, right);
    }
    if (auto ifExpr = std::dynamic_pointer_cast<ast::IfExpression>(node)) {
        return evalIfExpression(ifExpr, env);
    }

    if (auto hash = std::dynamic_pointer_cast<ast::HashLiteral>(node)) {
        return evalHashLiteral(hash, env);
    }
    if (auto ident = std::dynamic_pointer_cast<ast::Identifier>(node)) {
        return evalIdentifier(ident, env);
    }
    if (auto func = std::dynamic_pointer_cast<ast::FunctionLiteral>(node)) {
        std::vector<std::string> params;
        for (const auto& param : func->parameters_) {
            params.push_back(param->value_);
        }
        return std::make_shared<object::Function>(func->parameters_, func->body_, env);
    }

    if (auto call = std::dynamic_pointer_cast<ast::CallExpression>(node)) {
        auto function = eval(call->function_, env);
        if (isError(function)) return function;
        
        auto args = evalExpressions(call->arguments_, env);
        if (args.size() == 1 && isError(args[0])) return args[0];
        
        return applyFunction(function, args);
    }

    if (auto al = std::dynamic_pointer_cast<ast::ArrayLiteral>(node)) {
        auto eles = evalExpressions(al->elements_, env);
        if (eles.size() == 1 && isError(eles[0])) {
            return eles[0];
        }

        return std::make_shared<object::Array>(eles);
    }
    // index
    if (auto index = std::dynamic_pointer_cast<ast::IndexExpression>(node)) {
        auto left = eval(index->left_, env);
        if (isError(left)) {
            return left;
        }

        auto idx = eval(index->index_, env);
        if (isError(idx)) {
            return idx;
        }

        return evalIndexExpression(left, idx);
    }
    return nullptr;
}

std::shared_ptr<object::Object> Evaluator::evalIndexExpression(std::shared_ptr<object::Object>  &left,
                                                               const std::shared_ptr<object::Object> &index)
{
    if (left->Type() == object::Object::ObjectType::ARRAY_OBJ &&
        index->Type() == object::Object::ObjectType::INTEGER_OBJ) {
        return evalArrayIndexExpression(left, index);
    }
    else if (left->Type() == object::Object::ObjectType::HASH_OBJ) {
        evalHashIndexExpression(left, index);
    }
    else {
        return newError("index operator not supported: %s", left->Type());
    }
    return newError("index operator not supported: %s", left->Type());
}

std::shared_ptr<object::Object> Evaluator::evalArrayIndexExpression(const std::shared_ptr<object::Object> &array,
                                                                        const std::shared_ptr<object::Object> &index)
{
    auto arrayObject = std::dynamic_pointer_cast<object::Array>(array);
    auto idx = std::dynamic_pointer_cast<object::Integer>(index);
    auto m = arrayObject->elements_.size();
    decltype(m) i = idx->Value;
    if (i < 0 || i > m) {
        return nullptr;
    }

    return arrayObject->elements_[i];
}

std::shared_ptr<object::Object> Evaluator::evalHashIndexExpression(const std::shared_ptr<object::Object> &hash,
                                                                       const std::shared_ptr<object::Object> &index)
{
    auto hashObject = std::dynamic_pointer_cast<object::Hash>(hash);
    if (!hashObject) {
        return nullptr;
    }

    auto key = std::dynamic_pointer_cast<object::Hashable>(index);
    if (!key) {
        return newError("unusable as hash key: %d", index->Type());
    }
    auto hashKey = key->Hashkey();
    auto pair = hashObject->pairs_[hashKey].value_;
    return pair;
}

std::shared_ptr<object::Object> Evaluator::evalProgram(const std::shared_ptr<ast::Program>& program,
                                             const std::shared_ptr<Environment>& env) {
    std::shared_ptr<object::Object> result;
    
    for (const auto& statement : program->statements_) {
        result = eval(statement, env);
        
        if (auto returnValue = std::dynamic_pointer_cast<object::ReturnValue>(result)) {
            return returnValue->value_;
        }
        if (auto error = std::dynamic_pointer_cast<object::Error>(result)) {
            return error;
        }
    }
    
    return result;
}

std::shared_ptr<object::Object> Evaluator::evalBlockStatement(const std::shared_ptr<ast::BlockStatement>& block,
                                                    const std::shared_ptr<Environment>& env) {
    std::shared_ptr<object::Object> result;
    
    for (const auto& statement : block->statements_) {
        result = eval(statement, env);
        
        if (result) {
            auto rt = result->Type();
            if (rt == object::Object::ObjectType::RETURN_VALUE_OBJ || rt == object::Object::ObjectType::ERROR_OBJ) {
                return result;
            }
        }
    }
    
    return result;
}

std::shared_ptr<object::Object> Evaluator::evalPrefixExpression(const std::string& op,
                                                      const std::shared_ptr<object::Object>& right) {
    if (op == "!") {
        if (right->Type() == object::Object::ObjectType::NULL_OBJ) {
            return std::make_shared<object::Boolean>(true);
        } else if (right->Type() == object::Object::ObjectType::BOOLEAN_OBJ) {
            auto* o = dynamic_cast<object::Boolean*>(right.get());
            if (o->Value == true) return std::make_shared<object::Boolean>(false);
            if (o->Value == false) return std::make_shared<object::Boolean>(true);
        }
        return FALSE_OBJ;
    }
    if (op == "-") {
        if (right->Type() != object::Object::ObjectType::INTEGER_OBJ) {
            return newError("unknown operator: -%s", dragon::object::GetTypeString(right->Type()).c_str());
        }
        auto value = std::dynamic_pointer_cast<object::Integer>(right)->Value;
        return std::make_shared<object::Integer>(-value);
    }
    
    return newError("unknown operator: %s%s", op.c_str(), dragon::object::GetTypeString(right->Type()).c_str());
}

std::shared_ptr<object::Object> Evaluator::evalInfixExpression(const std::string& op,
                                                     const std::shared_ptr<object::Object>& left,
                                                     const std::shared_ptr<object::Object>& right) {
    if (left->Type() == object::Object::ObjectType::INTEGER_OBJ &&
        right->Type() == object::Object::ObjectType::INTEGER_OBJ) {
        return evalIntegerInfixExpression(op, left, right);
    }
    if (left->Type() == object::Object::ObjectType::STRING_OBJ &&
        right->Type() == object::Object::ObjectType::STRING_OBJ)
    {
        return evalStringInfixExpression(op, left, right);
    }
    
    if (op == "==") {
        auto b = TRUE_OBJ;
        if (*(static_cast<object::Boolean*>((left.get()))) == *(static_cast<object::Boolean*>((right.get())))) {
            b = TRUE_OBJ;
        }
        else {
            b = FALSE_OBJ;
        }
        return b;
    }
    if (op == "!=") {
        auto b = TRUE_OBJ;
        if (*(static_cast<object::Boolean*>((left.get()))) != *(static_cast<object::Boolean*>((right.get())))) {
            b = TRUE_OBJ;
        }
        else {
            b = FALSE_OBJ;
        }
        return b;
    }
    
    if (left->Type() != right->Type()) {
        return newError("type mismatch: %s %s %s",
                        dragon::object::GetTypeString( left->Type()).c_str(), op.c_str(),
                        dragon::object::GetTypeString(right->Type()).c_str());
    }
    
    return newError("unknown operator: %s %s %s",
                    dragon::object::GetTypeString(left->Type()).c_str(), op.c_str(),
                    dragon::object::GetTypeString(right->Type()).c_str());
}

std::shared_ptr<object::Object> Evaluator::evalIntegerInfixExpression(const std::string& op,
                                                            const std::shared_ptr<object::Object>& left,
                                                            const std::shared_ptr<object::Object>& right) {
    auto leftVal = std::dynamic_pointer_cast<object::Integer>(left)->Value;
    auto rightVal = std::dynamic_pointer_cast<object::Integer>(right)->Value;
    
    if (op == "+") return std::make_shared<object::Integer>(leftVal + rightVal);
    if (op == "-") return std::make_shared<object::Integer>(leftVal - rightVal);
    if (op == "*") return std::make_shared<object::Integer>(leftVal * rightVal);
    if (op == "/") return std::make_shared<object::Integer>(leftVal / rightVal);
    if (op == "<") return leftVal < rightVal ? TRUE_OBJ : FALSE_OBJ;
    if (op == ">") return leftVal > rightVal ? TRUE_OBJ : FALSE_OBJ;
    if (op == "==") return leftVal == rightVal ? TRUE_OBJ : FALSE_OBJ;
    if (op == "!=") return leftVal != rightVal ? TRUE_OBJ : FALSE_OBJ;
    
    return newError("unknown operator: %s %s %s",
                    dragon::object::GetTypeString(left->Type()).c_str(), op.c_str(), dragon::object::GetTypeString(right->Type()).c_str());
}

std::shared_ptr<object::Object> Evaluator::evalStringInfixExpression(const string & oper,
                                                                         const std::shared_ptr<object::Object> &left,
                                                                         const std::shared_ptr<object::Object> &right)
{
    // 暂时只支持字符串的 + 运算
    if (oper != "+") {
        return newError("unknown operator: %d %s %d",
                        left->Type(), oper.c_str(), right->Type());
    }

    auto leftValue = std::dynamic_pointer_cast<object::String>(left);
    auto rightValue = std::dynamic_pointer_cast<object::String>(right);

    return std::make_shared<object::String>(leftValue->Value + rightValue->Value);
}

std::shared_ptr<object::Object> Evaluator::evalIfExpression(const std::shared_ptr<ast::IfExpression>& ie,
                                                  const std::shared_ptr<Environment>& env) {
    auto condition = eval(ie->condition_, env);
    if (isError(condition)) return condition;
    
    if (isTruthy(condition)) {
        return eval(ie->consequence_, env);
    } else if (ie->alternative_) {
        return eval(ie->alternative_, env);
    } else {
        return NULL_OBJ_INS;
    }
}

std::shared_ptr<object::Object> Evaluator::evalHashLiteral(const std::shared_ptr<ast::HashLiteral>& hashliteral,
                                                           const std::shared_ptr<Environment>& env)
{
    std::map<object::HashKey, object::HashPair> pairs_;
    // std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs_;
    for (const auto & pair : hashliteral->pairs_) {
        auto key = eval(pair.first, env);
        if (isError(key)) {
            return key;
        }

        auto keyType = std::dynamic_pointer_cast<object::Hashable>(key);
        if (!keyType) {
            return newError("unusable as hash key: %s", key->Type());
        }

        auto value = eval(pair.second, env);
        if (isError(value)) {
            return value;
        }

        auto hashed = keyType->Hashkey();
        pairs_[hashed] = object::HashPair{key, value};
    }
    return std::make_shared<object::Hash>(pairs_);
}

std::shared_ptr<object::Object> Evaluator::evalIdentifier(const std::shared_ptr<ast::Identifier>& node,
                                                const std::shared_ptr<Environment>& env) {
    auto val = env->get(node->value_);
    if (val.first) {
        return val.first;
    }

    auto b = FindBuiltInFunc(node->value_);
    if (b != nullptr) {
        return  b;
    }
    // 查找确认是否是内置类型
    return newError(string("identifier not found: " + node->value_).c_str());
}

std::vector<std::shared_ptr<object::Object>> Evaluator::evalExpressions(
    const std::vector<std::shared_ptr<ast::Expression>>& exps,
    const std::shared_ptr<Environment>& env) {
    std::vector<std::shared_ptr<object::Object>> result;
    
    for (const auto& e : exps) {
        auto evaluated = eval(e, env);
        if (isError(evaluated)) {
            return {evaluated};
        }
        result.push_back(evaluated);
    }
    
    return result;
}

std::shared_ptr<object::Object> Evaluator::applyFunction(const std::shared_ptr<object::Object>& fn,
                                               const std::vector<std::shared_ptr<object::Object>>& args) {
    auto function = std::dynamic_pointer_cast<object::Function>(fn);

    if (function) {
        auto extendedEnv = extendFunctionEnv(function, args);
        auto evaluated = eval(function->body_, extendedEnv);
        return unwrapReturnValue(evaluated);
    }
    else { // 判断是不是内置的函数
        auto builtin = std::dynamic_pointer_cast<object::Builtin>(fn);
        return builtin->fn_(args);
    }

    return newError("not a function: %s",dragon::object::GetTypeString(fn->Type()).c_str() );
}

std::shared_ptr<Environment> Evaluator::extendFunctionEnv(
    const std::shared_ptr<object::Function>& fn,
    const std::vector<std::shared_ptr<object::Object>>& args) {
    auto env = Environment::newEnclosedEnvironment(fn->env_);
    
    for (size_t i = 0; i < fn->parameters_.size(); ++i) {
        env->set(fn->parameters_[i]->value_, args[i]); //
    }
    
    return env;
}

std::shared_ptr<object::Object> Evaluator::unwrapReturnValue(const std::shared_ptr<object::Object>& obj) {
    if (auto returnValue = std::dynamic_pointer_cast<object::ReturnValue>(obj)) {
        return returnValue->value_;
    }
    return obj;
}

bool Evaluator::isTruthy(const std::shared_ptr<object::Object>& obj) {
    // 通过 type 进行判断
    if (obj->Type() == dragon::object::Object::ObjectType::NULL_OBJ) {
        return false;
    } else if (obj->Type() == dragon::object::Object::ObjectType::BOOLEAN_OBJ) {
        auto* b = dynamic_cast<object::Boolean*>(obj.get());
        if (b->Value == true) {
            return true;
        } else
        {
            return false;
        }
    }

    return false;
}

bool Evaluator::isError(const std::shared_ptr<object::Object>& obj) {
    return obj && obj->Type() == object::Object::ObjectType::ERROR_OBJ;
}

std::shared_ptr<object::Error> Evaluator::newError(const char* format, ...) {
    char buffer[1024] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::make_shared<object::Error>(buffer);
}
} // namespace evaluator
} // namespace dragon