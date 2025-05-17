
#include "ast.h"
#include "object.h"

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <map>
using std::map;


namespace dragon {
    namespace object{
        map<Object::ObjectType, string> m {
                {Object::ObjectType::NULL_OBJ, "NULL"},
                {Object::ObjectType::ERROR_OBJ, "ERROR"},

                {Object::ObjectType::INTEGER_OBJ, "INTEGER"},
                {Object::ObjectType::BOOLEAN_OBJ, "BOOLEAN"},
                {Object::ObjectType::STRING_OBJ, "STRING"},

                {Object::ObjectType::RETURN_VALUE_OBJ, "RETURN_VALUE"},

                {Object::ObjectType::FUNCTION_OBJ, "FUNCTION"},
                {Object::ObjectType::BUILTIN_OBJ, "BUILTIN"},

                {Object::ObjectType::ARRAY_OBJ, "ARRAY"},
                {Object::ObjectType::HASH_OBJ, "HASH"},
        };
        string GetTypeString(const Object::ObjectType &ot) {
            if (m.find(ot) != m.end()) {
                return m[ot];
            }
            return "";
        }
    }
} // namespace dragon

