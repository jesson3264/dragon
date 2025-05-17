//
// Created by Jesson.Deng on 2025/4/24.
// Email: jesson3264@163.com
//

#ifndef MYPROJECT_REPL_H
#define MYPROJECT_REPL_H
#include <istream>
#include <ostream>
namespace repl {
class Repl {
public:
    Repl();
    void Start(std::istream& in, std::ostream& out);
};

} // namespace repl
#endif //MYPROJECT_REPL_H
