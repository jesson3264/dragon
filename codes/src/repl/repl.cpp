//
// Created by Jesson on 2025/4/24.
//

#include "repl.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "evaluator.h"
#include "environment.hpp"

#include <iostream>
#include <string>
#include <iomanip>  // 必须包含此头文件
#include <memory>

namespace repl {
Repl::Repl()
{

}
const std::string PROMPT = ">> ";
const std::string MONKEY_FACE = R"(
                       ZZ    ZZZ     Z Z     ZZ   ZZ
                    Z ZZZ  ZZZ  ZZZ ZZZ     ZZZ
                  ZZZZZZZZZZZZZZZZZZZZ     ZZ  Z
                  ZZZZZZZZZZZZZZZZZZZZZZZZZZ
                 ZZZZZZZZZZZZZ    ZZZZZZZZZZZZ
                 ZZZZZZZZZZZ         ZZZZZZZ  Z
               ZZZZZ ZZZZZZZ           ZZZZZZ
              ZZZZZZZZZZZZ              ZZZZZZZ
           ZZZZZZZZZZZZZZZZZ             ZZZZZZZ
           ZZZZZZ  ZZZZZZZ                ZZZZZZZ
                   ZZZZ                   ZZZZZZZ
                ZZZZZ                     ZZZZ ZZ
                 ZZ                       ZZZZ  Z
                                          ZZZZZ
                       ZZZZZZZZ          ZZZZZZ
                    ZZZZZZZZZZZZZZZZZZZZZZZZZZZ
ZZZZ             ZZZZZZZZZZZZZZZZZZZZZZZZZZZ ZZ
   ZZZZZZ      ZZZZZZZZZZZZZZZZZZZZZZZZZZZZ  Z
 ZZZZZZZZZZZZZZZZZZZZZZZZZ    ZZZZZ  ZZ  Z
Z     ZZ   ZZZZZZZZZZZZ        ZZZZ   Z
Z  ZZ  ZZ   ZZZZZZZZZZZ      ZZZ  Z
 Z  Z       ZZZZZZZZZ       ZZZ               ZZZZZZZZ
     ZZ    ZZZZZZZZZ   Z   ZZZ            ZZZZZZZZZZZZZZZ
           ZZZZZZZZZ  ZZZZZZZZZZ        ZZZZZZZZZZZZZZZZZZ
          ZZZZZZZZZ  Z ZZZZ   Z      ZZZZZZZZZZZZZZZZZZZZZZ
           ZZZZZZZZZZ   ZZZZZ        ZZZZZZZZZZZZZZZZZZZZZZZ
           ZZZZZZZZZ   ZZ  Z        ZZZZZZZZZZZZZZZZZZZZZZZZ
           ZZZZZZZZZZ  Z   ZZ      ZZZZZZZZZZZZZZZZZZZZZZZZZ
           ZZZZZZZZZZZ            ZZZZZZZZZZZ         ZZZZZZ
           ZZZZZZZZZZZZ          ZZZZZZZZZ            ZZZZZZ
           ZZZZZZZZZZZZZZZ      ZZZZZZZZZ             ZZZZZZ
           ZZZZZZZZZZZZZZZZZ  ZZZZZZZZZ   Z           ZZZZZ
            Z  ZZZZZZZZZZZZZZZZZZZZZZZZZZZ            ZZZZZ
               ZZZZZZZZZZZZZZZZZZZZ                   ZZZZ
                Z  ZZZZZZZZZZZZZZZZZ  Z              ZZZZ
                 Z  ZZZZZZZZZZZZ  ZZZZ               ZZZZ
                     ZZZ    ZZZ                      ZZZ
                       ZZZ    ZZ                    ZZZZ
                                                    ZZZ
                                                   ZZZ
                                                   ZZZ
                                                    ZZZZ
                                                       ZZZZ    Z
                                                           ZZZZZZ
                                                             ZZZZZ
                                                             ZZZ
)";
void printParserErrors(std::ostream& out, const std::vector<std::string>& errors) {
        out << MONKEY_FACE;
        out << "Woops! We ran into some dragon business here!\n";
        out << " parser errors:\n";
        for (const auto& msg : errors) {
            out << "\t" << msg << "\n";
        }
}

void Repl::Start(std::istream& in, std::ostream& out) {
    std::string line;
    auto env = std::make_shared<dragon::Environment>();

    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return;
        }

        lexer::Lexer l(line);
        parser::Parser p(l);

        auto program = p.parseProgram();
        if (p.errors.size()) {
            printParserErrors(out, p.errors);
            continue;
        }

        auto evaluated = dragon::evaluator::Evaluator::eval(program, env);
        if (evaluated != nullptr) {
            out << evaluated->Inspect() << "\n";
        }
    }
}
}