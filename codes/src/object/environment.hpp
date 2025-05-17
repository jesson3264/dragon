#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__


#include <memory>
#include <unordered_map>
#include "object.h"

namespace dragon {

class Environment {
public:
    Environment() : outer_(nullptr) {}
    
    static std::shared_ptr<Environment> newEnvironment() {
        return std::make_shared<Environment>();
    }
    
    static std::shared_ptr<Environment> newEnclosedEnvironment(std::shared_ptr<Environment> outer) {
        auto env = newEnvironment();
        env->outer_ = std::move(outer);
        return env;
    }
    
    std::pair<std::shared_ptr<object::Object>, bool> get(const std::string& name) const {
        auto it = store_.find(name);
        if (it != store_.end()) {
            return {it->second, true};
        }
        
        if (outer_) {
            return outer_->get(name);
        }
        
        return {nullptr, false};
    }
    
    std::shared_ptr<object::Object> set(const std::string& name, std::shared_ptr<object::Object> val) {
        store_[name] = std::move(val);
        return store_[name];
    }

private:
    std::unordered_map<std::string, std::shared_ptr<object::Object>> store_;
    std::shared_ptr<Environment> outer_;
};

} // namespace dragon


#endif