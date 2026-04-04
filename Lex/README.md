<<<<<<< HEAD
# SeuLex 使用说明

本项目提供了一个简化版 Lex 生成器 `seulex`。
它读取 `.l` 规则文件（例如 `c99.l`），生成词法分析器源码 `lex.yy.c`。

## 1. 环境要求

- Windows + MinGW64（可用 `g++/gcc`）
- 可选：CMake（如果希望使用 CMake 构建流程）

## 2. 编译生成器（seulex）

### 方案 A：g++（当前工作区推荐）

在项目根目录（Lex 文件夹）执行：

```powershell
g++ -std=c++17 -Iinclude src/main.cpp src/lex_parser.cpp src/regex_converter.cpp src/nfa.cpp src/dfa.cpp src/code_generator.cpp -o seulex.exe
```

### 方案 B：CMake

```powershell
cmake -S . -B build
cmake --build build --config Release
```

使用 CMake 时，可执行文件通常位于 `build/Release/seulex.exe`。

## 3. 从 c99.l 生成 lex.yy.c

```powershell
./seulex.exe c99.l lex.yy.c
```

期望输出包含：

- Parsed: ... definitions, ... rules
- NFA built: ... states
- DFA constructed: ... states
- DFA minimized: ... states
- Generated lex.yy.c
- Done.

## 4. 编译并运行词法测试驱动

测试驱动为 `test_lexer.c`。
将生成的词法器与测试驱动一起编译：

```powershell
gcc -fdiagnostics-color=always -g -DASTNode=void -I. -Iinclude lex.yy.c test_lexer.c -o test_lexer.exe
```

使用样例输入运行：

```powershell
./test_lexer.exe test_input.c
```

你应看到类似输出：

- 263    int
- 258    main
- 260    42

这说明词法切分流程正常工作。

## 5. 日常使用流程

1. 修改 `c99.l`
2. 重新生成词法器：`./seulex.exe c99.l lex.yy.c`
3. 重新编译测试程序：`gcc ... lex.yy.c test_lexer.c ...`
4. 用 `test_input.c` 或自定义输入运行 `test_lexer.exe`

## 6. 备注

- `test_lexer.c` 是面向当前 `c99.tab.hpp + symbol.h` 接口的最小测试驱动。
- 若你接入真实符号表实现，请定义宏：
  - `TEST_LEXER_USE_REAL_SYMBOL`
- 若 `yylval` 已在其他编译单元定义，请定义宏：
  - `TEST_LEXER_USE_REAL_YYLVAL`
=======
# c99
compiler
>>>>>>> origin/main
