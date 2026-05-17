# SeuLex 与 C99 子集词法 / 记号约定

本仓库核心为 **自实现的词法生成器 `seulex`**（`src/`、`include/`），以及 **`c99.l`** 与由 seulex 生成的 **`lex.yy.c`**。

## 与词法组一致的接口（Flex/Bison 风格）

| 项目 | 约定 |
|------|------|
| 记号头文件 | **`y.tab.h`**：`enum yytokentype` 名与编号须与 **`c99.y`** 开头 `%token` 一致 |
| 词法返回值 | **`yylex()`** 返回 **`int`**（记号类型），**不用**整 token 结构体 |
| 属性 | 全局 **`YYSTYPE yylval`**（含 **`char *str_val`** 及 `intval` / `fval` / `ast` / `list`） |
| 位置 | 全局 **`YYLTYPE yylloc`**；`c99.l` 的 **`count()`** 在每次匹配后更新 `yylloc` |
| 原文 | **`yytext`**：当前实现为 **`char *yytext`**（指向扫描缓冲，见 `lex.yy.c`）；若改为定长数组，只需改生成器/lex 输出一处定义 |
| 行号 | **`line_num`**（`c99.l` 使用）与 **`ytab_runtime.c`**；Flex 的 **`yylineno`** 仍由生成器维护 |

全局定义见 **`ytab_runtime.c`**（`yylval`、`yylloc`、`line_num`），与 **`test_lexer.c`** 链接时加入该文件，避免重复定义 `yylval`。

**`c99.y`**：当前保留 **`%token` / `%union`** 与占位产生式，便于组内与 `y.tab.h` 对齐；**不要求**用 Bison 生成分析器。

## 1. 环境

- Windows + MinGW（`g++` / `gcc`）
- 可选：CMake 构建 `seulex`

## 2. 编译 seulex

```powershell
g++ -std=c++17 -Iinclude src/main.cpp src/lex_parser.cpp src/regex_converter.cpp src/nfa.cpp src/dfa.cpp src/code_generator.cpp -o seulex.exe
```

## 3. 生成 lex.yy.c

```powershell
./seulex.exe c99.l lex.yy.c
```

生成结果首部应与 **`c99.l`** 一致包含 **`#include "y.tab.h"`**。

## 4. 词法测试

```powershell
gcc -c -I. -Iinclude lex.yy.c -o lex.yy.o
gcc -c -I. -Iinclude ytab_runtime.c -o ytab_runtime.o
gcc -std=c11 -g -I. -Iinclude -o test_lexer.exe test_lexer.c ytab_runtime.o lex.yy.o
./test_lexer.exe test_input.c
```
（`test_lexer.exe` 须用 **gcc** 链接：若用 `g++` 编 `test_lexer.c`，会按 C++ 解析 `yylex`，与 **gcc** 编的 `lex.yy.o` 符号不一致。）

## 5. 语法分析（LR(1) 自写）

与词法对接时包含 **`y.tab.h`**、**`ast.h`**，总控中调用 **`yylex()`** 取记号，读 **`yylval`** / **`yytext`** / **`yylloc`** 即可。

`parser/` 下为 **LR(1)+LALR(1)** 实验代码：**`grammar.cpp`**、**`seu_lr1_parser.cpp`**（同心项压缩）、入口 **`parser/c99_parse_main.cpp`**、三地址骨架 **`semantic/ir_from_ast.cpp`** + **`parser/c99_ir_main.cpp`**。构建与交付对照见 **`build_parse.cmd`**、**`docs/交付说明.md`**；与词法组 GitHub [`lex&yacc`](https://github.com/wsyzznn/c99/tree/lex%26yacc/Lex) 差异见 **`docs/与词法lex-yacc分支对照.md`**。

## 6. 其它

- `ast.cpp`：语法树构造函数。
- `include/symbol.h`：符号表接口。
- `debug_test.cpp`：调试 seulex 解析 `c99.l`。
