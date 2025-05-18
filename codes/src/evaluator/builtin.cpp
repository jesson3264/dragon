//
// Created by Jesson on 2025/5/18.
//

#include "builtin.h"
#include "object.h"
#include "evaluator.h"

#include <map>
#include <string>
#include <iostream>

std::shared_ptr<dragon::object::Object> len(const std::vector<std::shared_ptr<dragon::object::Object>> &args)
{
    // 参数个数必须是1
    if (args.size() != 1) {
        return dragon::evaluator::Evaluator::newError("wrong number of arguments. got=%d, want=1",
                        args.size());
    }

    if (args[0]->Type() == dragon::object::Object::ObjectType::ARRAY_OBJ) {
        auto array = std::dynamic_pointer_cast<dragon::object::Array>(args[0]);
        if (array) {
            return std::make_shared<dragon::object::Integer>(array->elements_.size());
        }
    } else if (args[0]->Type() == dragon::object::Object::ObjectType::STRING_OBJ) {
        auto str = std::dynamic_pointer_cast<dragon::object::String>(args[0]);
        if (str) {
            return std::make_shared<dragon::object::Integer>(str->Value.length());
        }
    }

    return dragon::evaluator::Evaluator::newError("unsupported type");
}

std::shared_ptr<dragon::object::Object> print(const std::vector<std::shared_ptr<dragon::object::Object>> &args)
{
    for (const auto& arg : args) {
        std::cout << arg->Inspect();
    }
    return nullptr;
}

std::map<std::string, std::shared_ptr<dragon::object::Builtin>> builtins  = {
        {"len", std::make_shared<dragon::object::Builtin>(len)},
        {"print", std::make_shared<dragon::object::Builtin>(print)},

};

std::shared_ptr<dragon::object::Builtin> FindBuiltInFunc(const std::string &name)
{
    if (builtins.find(name) != builtins.end()) {
        return builtins[name];
    }

    return nullptr;
}