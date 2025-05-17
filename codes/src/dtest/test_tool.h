#ifndef __TEST_TOOL_H__
#define __TEST_TOOL_H__

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>

// 断言失败处理器
inline void AssertionFailure(const char* file, int line, const std::string& msg) {
    std::cerr << "\033[31m[FAIL]\033[0m " << file << ":" << line << ": " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

// 支持流式输出的失败宏
#define FAIL() \
    AssertionFailure(__FILE__, __LINE__, std::string())

#define FAIL_MSG() \
    std::cerr, AssertionFailure(__FILE__, __LINE__, ""), std::ostringstream() \
    ).ostream()


class FailMessage {
public:
    FailMessage(const char* file, int line) : file(file), line(line) {}

    ~FailMessage() {
        if (!ss.str().empty()) {
            AssertionFailure(file, line, ss.str());
        }
    }

    std::ostream& ostream() { return ss; }

private:
    std::ostringstream ss;
    const char* file;
    int line;
};
#define FAIL_STREAM() \
    FailMessage(__FILE__, __LINE__).ostream()

// 布尔断言（带自定义消息）
#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        FAIL_STREAM() << "Assertion failed: " #expr << " is false"; \
    }

// 相等断言（带自定义消息）
#define ASSERT_EQ(actual, expected) \
    do { \
        auto&& a = (actual); \
        auto&& e = (expected); \
        if (a != e) { \
            FAIL_STREAM() << "Assertion failed: " #actual " != " #expected \
                         << " (" << a << " != " << e << ")"; \
        } \
    } while (0)

// 相等断言（带自定义消息）
#define ASSERT_NE(actual, expected) \
    do { \
        auto&& a = (actual); \
        auto&& e = (expected); \
        if (a == e) { \
            FAIL_STREAM() << "Assertion failed: " #actual " != " #expected \
                         << " (" << a << " != " << e << ")"; \
        } \
    } while (0)
// 直接输出失败信息
#define ADD_FAILURE() \
    FAIL_STREAM()


#include <iostream>
#include <string>
#include <sstream>
#include <exception>

class Value {
public:
    enum Type { VINT, VBOOL, VSTRING, VNULL};
    Value():type(VNULL) {
        ;
    }
    Value(int x) : type(VINT) {
        this->i = x;
    }
    Value(bool b) : type(VBOOL) {
        this->b = b;
    }
    Value(const std::string& s) : type(VSTRING) {
        this->s = s;
    }

    Value(const char* cstr) : type(VSTRING) {
        s = string(cstr);
    }
    ~Value() {
        if (type == VSTRING) {
            // reinterpret_cast<std::string*>(&storage)->~basic_string();
        }
    }

    // 添加拷贝/移动构造函数（略）...

    Type getType() const { return type; }

    int getInt() const {
        assert(type == VINT);
        return i;
    }

    // 其他getter类似...
    bool getBool() const {
        assert(type == VBOOL);
        return b;
    }

    std::string getString()const{
        assert(type == VSTRING);
        return s;
    }
private:
    Type type;
    std::string s;
    int i;
    bool b;

//    alignas(alignof(std::string)) char storage[sizeof(std::string)];
};

class TestingT {
public:
    // 模拟 Go 的 testing.T 的 Fatalf 方法
    void Fatalf(const std::string& format) {
        std::ostringstream oss;
        oss << format;
        this->FailNow(oss.str());
    }

    template<typename T, typename... Args>
    void Fatalf(const std::string& format, T first, Args... args) {
        std::ostringstream oss;
        FormatHelper(oss, format, first, args...);
        this->FailNow(oss.str());
    }

    // 其他类似 testing.T 的方法
    void Errorf(const std::string& format) {
        std::cerr << "ERROR: " << format << std::endl;
        failed_ = true;
    }

    void Logf(const std::string& format) {
        std::cout << "LOG: " << format << std::endl;
    }

    bool Failed() const {
        return failed_;
    }

private:
    bool failed_ = false;

    void FailNow(const std::string& message) {
        std::cerr << "FAIL: " << message << std::endl;
        failed_ = true;
        throw TestFailureException(message);
    }

    // 辅助函数用于格式化字符串
    template<typename T, typename... Args>
    void FormatHelper(std::ostringstream& oss, const std::string& format, T first, Args... args) {
        size_t pos = format.find("%v");
        if (pos == std::string::npos) {
            oss << format;
            return;
        }
        oss << format.substr(0, pos);
        oss << first;
        FormatHelper(oss, format.substr(pos + 2), args...);
    }

    void FormatHelper(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }

    // 测试失败异常
    class TestFailureException : public std::exception {
    public:
        TestFailureException(const std::string& message) : message_(message) {}
        const char* what() const noexcept override { return message_.c_str(); }
    private:
        std::string message_;
    };
};
#endif