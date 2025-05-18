cmake_minimum_required(VERSION 3.10)
project(dragon)

### 语法设置
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# 设置 Debug 模式的编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra")

### 编译选项设置

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|Intel")
    # 将所有警告视为错误
    add_compile_options(-Werror)
    
    # 可选：添加特定警告选项
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 递归查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

### 
message(${SOURCES})

### 设置头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/src/lexer)
include_directories(${CMAKE_SOURCE_DIR}/src/repl)
include_directories(${CMAKE_SOURCE_DIR}/src/token)
include_directories(${CMAKE_SOURCE_DIR}/src/ast)
include_directories(${CMAKE_SOURCE_DIR}/src/parser)
include_directories(${CMAKE_SOURCE_DIR}/src/dtest)
include_directories(${CMAKE_SOURCE_DIR}/src/object)
include_directories(${CMAKE_SOURCE_DIR}/src/evaluator)
include_directories(${CMAKE_SOURCE_DIR}/src/util)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} src/evaluator/builtin.cpp src/evaluator/builtin.h)

# 包含所有子目录头文件
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接系统库示例
# find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME})

