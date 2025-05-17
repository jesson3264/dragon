#include "evaluator_test.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "evaluator.h"

#include "test_tool.h"

std::shared_ptr<dragon::object::Object> testEval(const std::string& input)
{
    lexer::Lexer lexer(input);
    parser::Parser parser(lexer);
    auto program = parser.parseProgram();
    auto env = std::make_shared<dragon::Environment>();
    return dragon::evaluator::Evaluator::eval(program, env);
}

// 测试整数计算
void TestEvalIntegerExpression()
{
    struct Test {
        std::string input;
        int64_t expected;
    };
    std::vector<Test> tests = {
       {"5", 5},
		{"10", 10},
		{"-5", -5},
		{"-10", -10},
		{"5 + 6 + 7 + 8 - 10", 16},
		{"2 * 2 * 2 * 2 * 2", 32},
		{"-50 + 100 + -50", 0},
		{"5 * 2 + 10", 20},
		{"5 + 2 * 10", 25},
		{"20 + 2 * -10", 0},
		{"50 / 2 * 2 + 10", 60},
		{"2 * (5 + 10)", 30},
		{"3 * 3 * 3 + 10", 37},
		{"3 * (3 * 3) + 10", 37},
		{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for (const auto& tt : tests) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tt.input);
        int64_t result = dynamic_cast<dragon::object::Integer*>(evaluated.get())->Value;
        ASSERT_EQ(result, tt.expected);
    }
}

bool testBooleanObject(TestingT& t, dragon::object::Object *object,  bool expected)  {
	auto *obj = dynamic_cast<dragon::object::Boolean*>(object);
	if (!obj)	{
        t.Fatalf("");
        return false;
    }

	if (obj->Value != expected) {
		return false;
	}
	return true;
}

void TestEvalBooleanExpression(TestingT& t) {
	struct Test {
		std::string input;
		bool expected;
	};

	std::vector<Test> tests = {
		{"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
	};

	for (const auto& tc : tests) {
		std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        if (!testBooleanObject(t, evaluated.get(), tc.expected)) {
            t.Fatalf("input:%s", tc.input.c_str());
        }
	}
}

void TestBangOperator(TestingT &t) {
    struct TestCase {
        string intput;
        bool expected;
    };

    std::vector<TestCase> testcases {
            {"!true", false},
            {"!false", true},
            {"!5", false},
            {"!!true", false},
            {"!!false", false},
            {"!!5", true},
    };

    for(auto & tc: testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.intput);
        testBooleanObject(t, evaluated.get(), tc.expected);
    }
}

bool testIntegerObject(TestingT& t, dragon::object::Object * o, int expected) {
    auto* oi = dynamic_cast<dragon::object::Integer*> (o);
    if (!oi) {
        t.Fatalf("");
        return false;
    }

    if (oi->Value != expected) {
        t.Fatalf("");
        return false;
    }
    return true;
}

bool testNullObject(TestingT& t, dragon::object::Object *o) {

    if (!o) {
        t.Fatalf("");
        return false;
    }

    auto *n = dynamic_cast<dragon::object::Null*>(o);
    if (!n) {
        t.Fatalf("");
        return false;
    }

    return true;
}


void TestIfElseExpressions(TestingT &t) {
    struct TestCase {
        string input;
        Value expected;
    };

    vector<TestCase> testcases {
            {"if (true) { 10 }", 10},
            {"if (false) { 10 }", Value()},//NULL
            {"if (1) { 10 }", 10},
            {"if (1 < 2) { 10 }", 10},
            {"if (1 > 2) { 10 }", Value()},
            {"if (1 > 2) { 10 } else { 20 }", 20},
            {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    for( auto & tc : testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        if (evaluated->Type() == dragon::object::Object::ObjectType::INTEGER_OBJ) {
            if (!testIntegerObject(t, evaluated.get(), tc.expected.getInt())) {
                t.Fatalf("");
            }
        } else {
            testNullObject(t, evaluated.get());
        }
    }
}

void TestReturnStatements(TestingT& t) {
    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> testcases {
            {"return 10;", 10},
            {"return 10; 9;", 10},
            {"return 2 * 5; 9;", 10},
            {"9; return 2 * 5; 9;", 10},
            {"if (10 > 1) { return 10; }", 10},
            {
                    R"(
                    if (10 > 1) {
                        if (10 > 1) {
                            return 10;
                        }

                        return 1;
                    }
                    )",
                    10,
            },
            {
                    R"(
                    let f = fn(x) {
                        return x;
                        x + 10;
                    };
                    f(10);)",
                    10,
            },
            {
                    R"(
                    let f = fn(x) {
                        let result = x + 10;
                        return result;
                        return 10;
                    };
                    f(10);)",
                    20,
            },
    };

    for (auto &tc : testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        if (!testIntegerObject(t, evaluated.get(), tc.expected)) {
            t.Fatalf("");
        }
    }
}

void TestErrorHandling(TestingT &t) {
    struct TestCase {
        string input;
        string expectedMessage;
    };

    vector<TestCase> testcases {
            {
                    "5 + true;",
                    "type mismatch: INTEGER + BOOLEAN",
            },
            {
                    "5 + true; 5;",
                    "type mismatch: INTEGER + BOOLEAN",
            },
            {
                    "-true",
                    "unknown operator: -BOOLEAN",
            },
            {
                    "true + false;",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "true + false + true + false;",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "5; true + false; 5",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "if (10 > 1) { true + false; }",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    R"(
                    if (10 > 1) {
                        if (10 > 1) {
                            return true + false;
                        }

                        return 1;
                    }
                    )",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "foobar",
                    "identifier not found: foobar",
            },
    };

    for (auto &tc: testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        auto* err = dynamic_cast<dragon::object::Error*>(evaluated.get());
        if (!err) {
            t.Fatalf("");
        }
        if (err->Message != tc.expectedMessage) {
            t.Fatalf("%s - %s", err->Inspect().c_str(), tc.expectedMessage.c_str());
        }
    }
}

void TestLetStatements(TestingT& t)
{
    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> testcases {
            {"let a = 5; a;", 5},
            {"let a = 5 * 5; a;", 25},
            {"let a = 5; let b = a; b;", 5},
            {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
    for (auto & tc : testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        if (!evaluated.get()) {
            t.Fatalf("");
            if (!testIntegerObject(t, evaluated.get(), tc.expected)) {
                t.Fatalf("");
            }
        }
    }
}

void TestFunctionObject(TestingT & t) {
    string input = "fn(x) { x + 2; };";
    std::shared_ptr<dragon::object::Object> evaluated = testEval(input);
    if (!evaluated.get()) {
        t.Fatalf("");
    }

    auto* func = dynamic_cast<dragon::object::Function*>(evaluated.get());
    if (!func) {
        t.Fatalf("");
    }

    if (func->parameters_.size() != 1) {
        t.Fatalf("");
    }

    if (func->parameters_[0]->String() != "x") {
        t.Fatalf("xx");
    }

    string expectedBody = "(x + 2)";
    if (func->body_->String() != expectedBody) {
        t.Fatalf("TODO string");
    }



}

void TestFunctionApplication(TestingT& t) {
    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> testcases{
            {"let identity = fn(x) { x; }; identity(5);", 5},
            {"let identity = fn(x) { return x; }; identity(5);", 5},
            {"let double = fn(x) { x * 2; }; double(5);", 10},
            {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
            {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
            {"fn(x) { x; }(5)", 5},
    };

    for (auto &tc : testcases) {
        std::shared_ptr<dragon::object::Object> evaluated = testEval(tc.input);
        if (!testIntegerObject(t, evaluated.get(), tc.expected)) {
            t.Fatalf("todo");
        }
    }

}

void TestEnclosingEnvironments(TestingT& t)
{
    string input = R"(
        let first = 10;
        let second = 10;
        let third = 10;

        let ourFunction = fn(first) {
          let second = 20;

          first + second + third;
        };

    ourFunction(20) + first + second;
    )";
    std::shared_ptr<dragon::object::Object> evaluated = testEval(input);
    testIntegerObject(t, evaluated.get(), 70);
}

void TestClosures(TestingT &t) {
    string intput = R"(let newAdder = fn(x) {
  fn(y) { x + y };
};

let addTwo = newAdder(2);
addTwo(2);)";

    std::shared_ptr<dragon::object::Object> evaluated = testEval(intput);
    testIntegerObject(t, evaluated.get(), 4);
}

void TestStringLiteral(TestingT &t) {
    string input = R"("www.jesson32.cn")";
    std::shared_ptr<dragon::object::Object> evaluated = testEval(input);
    auto* str = dynamic_cast<dragon::object::String*>(evaluated.get());
    if (!str) {
        t.Fatalf("str should not be null");
    }

    if (str->Value != "www.jesson32.cn") {
        t.Fatalf("%s should equal %s", str->Value.c_str(), input.c_str());
    }
}

void TestEvals()
{
	TestingT t;
    TestEvalIntegerExpression();
	TestEvalBooleanExpression(t);
    TestBangOperator(t);
    TestIfElseExpressions(t);
    TestReturnStatements(t);
    TestErrorHandling(t);
    TestLetStatements(t);
    TestFunctionObject(t);
    TestFunctionApplication(t);
    TestEnclosingEnvironments(t);
    TestClosures(t);
    TestStringLiteral(t);
}