# Dragon

Dragon 是一个用 C++ 实现的编程语言解释器，支持词法分析、语法分析、抽象语法树构建、求值以及 REPL（Read-Eval-Print Loop）功能。该项目旨在提供一个简单且可扩展的编程语言实现示例。

## 主要特性

- **词法分析**：将源代码转换为 token 流。
- **语法分析**：将 token 流解析为抽象语法树（AST）。
- **抽象语法树**：构建并操作 AST。
- **求值器**：对 AST 进行求值，支持基本运算和内置函数。
- **REPL**：提供交互式命令行界面，方便用户输入和测试代码。
- **测试框架**：包含单元测试和集成测试，确保代码质量。

## 目录结构

```
.
├── .git/
├── .idea/
├── codes/
│   ├── cmake-build-debug/
│   ├── src/
│   │   ├── ast/
│   │   ├── dtest/
│   │   ├── evaluator/
│   │   ├── lexer/
│   │   ├── object/
│   │   ├── parser/
│   │   ├── repl/
│   │   ├── token/
│   │   ├── util/
│   │   └── main.cpp
│   ├── CMakeLists.txt
│   └── README.md
└── LICENSE
```

## 构建与运行

### 依赖

- CMake (>= 3.10)
- C++ 编译器 (支持 C++14 或更高版本)

### 构建步骤

1. 进入 `codes` 目录：
   ```bash
   cd codes
   ```

2. 创建并进入 `build` 目录：
   ```bash
   mkdir build && cd build
   ```

3. 生成构建文件并编译：
   ```bash
   cmake .. && make
   ```

4. 运行程序：
   ```bash
   ./dragon
   ```

## 测试

项目包含多个测试模块，可以通过以下命令运行测试：

```bash
./dragon -t
```

## 贡献

欢迎贡献代码、报告问题或提出改进建议。请遵循以下步骤：

1. Fork 项目。
2. 创建你的特性分支 (`git checkout -b feature/amazing-feature`)。
3. 提交你的更改 (`git commit -m 'Add some amazing feature'`)。
4. 推送到分支 (`git push origin feature/amazing-feature`)。
5. 打开一个 Pull Request。

## 许可证

本项目采用 MIT 许可证。详情请参阅 [LICENSE](LICENSE) 文件。

## 联系方式

- 作者：Jesson.Deng
- 邮箱：jesson3264@163.com 
