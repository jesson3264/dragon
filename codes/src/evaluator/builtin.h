//
// Created by Jesson on 2025/5/18.
//
// 内置函数
#ifndef DRAGON_BUILTIN_H
#define DRAGON_BUILTIN_H

#include "object.h"
#include <string>

std::shared_ptr<dragon::object::Builtin> FindBuiltInFunc(const std::string &name);

#endif //DRAGON_BUILTIN_H
