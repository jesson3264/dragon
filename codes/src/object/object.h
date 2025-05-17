#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "ast.h"
#include "hash.h"
#include <string>

namespace dragon {
namespace object {  
class Object {
public:
    enum class ObjectType {
        NULL_OBJ,
        ERROR_OBJ,

        INTEGER_OBJ,
        BOOLEAN_OBJ,
        STRING_OBJ,

        RETURN_VALUE_OBJ,

        FUNCTION_OBJ,
        BUILTIN_OBJ,

        ARRAY_OBJ,
        HASH_OBJ, // "HASH
    };
    virtual ~Object() = default;
    virtual ObjectType Type() const = 0;
    virtual std::string Inspect() const = 0;
};

class HashKey {
public:
    HashKey(const Object::ObjectType& t, u_int64_t v) : Type(t), Value(v){}
    bool operator < (const HashKey &other) const {
        return Value < other.Value;
    }

    bool operator==(const HashKey& other) const {
        return Type == other.Type && Value == other.Value;
    }
    Object::ObjectType Type;
    uint64_t Value;
};

class HashPair {
public:
    std::shared_ptr<Object> key_;
    std::shared_ptr<Object> value_;
};

class Hash : public Object{
public:
    Hash(std::map<HashKey, HashPair> m) : pairs_(m){}
    std::map<HashKey, HashPair> pairs_;
public:
    ObjectType Type() const override {
        return ObjectType::HASH_OBJ;
    }

    string Inspect() const override {
        std::stringstream out;
        bool first = true;
        for (const auto& pair : pairs_) {
            if (!first) {
                out << ", ";
            }
            first = false;
            out << pair.second.value_->Inspect();
        }

        return out.str();
    }
};
// hash 接口
class Hashable {
public:
    virtual HashKey Hashkey() const = 0;

    virtual bool lessThan(const Hashable& other) const = 0;
};

class Integer : public Object, public Hashable {
public:
    int64_t Value;
    Integer(int64_t v) : Value(v) {}
    bool operator == (Integer &i) const  {
        return i.Value == Value ? true: false;
    }

    bool lessThan (const Hashable& other) const override{
        const auto* p = dynamic_cast<const Integer*>(&other);
        return Value < p->Value;
    }
    ObjectType Type() const override { return ObjectType::INTEGER_OBJ; }
    std::string Inspect() const override { return std::to_string(Value); }

    HashKey Hashkey() const override {
        return HashKey(Type(), Value);
    }
};

class Boolean : public Object, public Hashable {
public:
    bool Value;
    Boolean(bool v) : Value(v) {}
    bool operator == (Boolean& b) const{
        if (Value == b.Value) {
            return true;
        }
        return false;
    }

    bool operator != (Boolean& b) const{
        if (Value != b.Value) {
            return true;
        }
        return false;
    }

    bool lessThan (const Hashable& other) const override{
        const auto* p = dynamic_cast<const Boolean*>(&other);
        return Value < p->Value;
    }

    ObjectType Type() const override { return ObjectType::BOOLEAN_OBJ; }
    std::string Inspect() const override { return Value ? "true" : "false"; }

    HashKey Hashkey() const override {
        return HashKey(Type(), Value ? 1 : 0);
    }
};

class String : public Object, public Hashable {
public:
    string Value;
    String(const string &s):Value(s){}
    bool lessThan (const Hashable& other) const override{
        const auto* p = dynamic_cast<const String*>(&other);
        return Value < p->Value;
    }
    ObjectType Type() const override {return ObjectType::STRING_OBJ;}
    std::string Inspect() const override {
        return Value;
    }

    HashKey Hashkey() const override {
        return HashKey(Type(), util::times33_hash(Value.c_str()));
    }
};

// 内置函数

// 数组对象
class Array :public Object {
public:
    std::vector<std::shared_ptr<Object>> elements_;
public:
    Array(vector<std::shared_ptr<Object>> &es): elements_(es){}
    ObjectType Type()const override {
        return ObjectType::ARRAY_OBJ;
    }

    std::string Inspect() const override {
        std::stringstream out;
        out << "[";
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (i != 0) {
                out << ", ";
            }
            out << elements_[i]->Inspect();
        }
        out << "]";
        return out.str();
    }
};


class Null : public Object {
public:
    ObjectType Type() const override { return ObjectType::NULL_OBJ; }
    std::string Inspect() const override { return "null"; }
};

class ReturnValue : public Object {
public:
    std::shared_ptr<Object> value_;
    ReturnValue(std::shared_ptr<Object> v) : value_(v) {}
    ObjectType Type() const override { return ObjectType::RETURN_VALUE_OBJ; }
    std::string Inspect() const override { return value_ ? value_->Inspect() : "null"; }
};

class Error : public Object {
public:
    std::string Message;
    Error(const std::string& msg) : Message(msg) {}
    ObjectType Type() const override { return ObjectType::ERROR_OBJ; }
    std::string Inspect() const override { return "ERROR: " + Message; }
};

string GetTypeString(const Object::ObjectType &ot);
} // namespace object
} // namespace dragon

#endif