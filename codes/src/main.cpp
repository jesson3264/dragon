//
// Author: Jesson
// Email: jesson3264@163.com
//
#include <iostream>
#include "repl.h"
#include "lexer_test.h"
#include "ast_test.h"
#include "parser_test.h"
#include "evaluator_test.h"

using namespace std;
#define Version "1.0.0"

void Test() {
    TestNextToken();
//    cout << "Version:" << Version << endl;
//    cout << "Author: Jesson.Deng" << endl;
//	cout << "Email: jesson3264@163.com" << endl;
//    Repl r;
//    r.Start();
//
//    TestAst();
//     ParserTest();
    TestEvals();

//    repl::Repl r;
//    r.Start(std::cin, std::cout);
}
int main(int argc, char **argv)
{
    if (argc > 1) {
        if (string(argv[1]) == "-t") {
            Test();
        }

        if (string(argv[1]) == "-v") {
            cout << Version << endl;
        }
    }


	return 0;
}