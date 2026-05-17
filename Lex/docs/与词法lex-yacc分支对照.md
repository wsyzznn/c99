# 与词法组 `lex&yacc` 分支对照说明

词法同学原版仓库：[wsyzznn/c99 · 分支 `lex&yacc` · `Lex/`](https://github.com/wsyzznn/c99/tree/lex%26yacc/Lex)（[SeuLex 使用说明](https://github.com/wsyzznn/c99/tree/lex%26yacc/Lex#seulex-%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)）

本目录（`c99last/c99-main/Lex`）在词法交付基础上，接入手写 **LALR(1)** 语法分析、三地址 IR 骨架与一键构建。**并非只改了测试命令。**

---

## 1. 相同部分（可视为「词法原版」）

| 内容 | 说明 |
|------|------|
| **`c99.l` 中 `%%` 以下规则** | 关键字、整型/浮点/字符串常量、运算符、`check_type()` 等与 `lex&yacc` 一致 |
| **SeuLex 核心源码** | `src/lex_parser.cpp`、`regex_converter.cpp`、`nfa.cpp`、`dfa.cpp` 与 GitHub 分支同源 |
| **SeuLex 使用流程** | 编 `seulex.exe` → `seulex.exe c99.l lex.yy.c` → 输出 Parsed / NFA / DFA / Generated |

词法「切什么、返回什么 token」**没有重做**。

---

## 2. 有差异的部分（不限于测试命令）

### 2.1 `c99.l` 头尾与接口

| 词法同学 `lex&yacc` | 本仓库 |
|---------------------|--------|
| `#include "c99.tab.hpp"` | `#include "y.tab.h"` |
| `int line_num = 1` 定义在 `.l` 内 | `extern int line_num`（定义在 `ytab_runtime.c`） |
| `count()` 只更新 `column` / `line_num` | `count()` 同时更新 **`yylloc`** |
| `yywrap()` 写在 `c99.l` 末尾 | 由 **SeuLex** 在 `code_generator.cpp` 中生成 |
| — | `%option noyywrap`、`%option yylineno` |

### 2.2 记号头与全局语义变量

| 词法同学 `lex&yacc` | 本仓库 |
|---------------------|--------|
| Bison 生成 **`c99.tab.c` / `c99.tab.hpp`** | 手写 **`y.tab.h`**（`enum yytokentype` 编号与 Bison 版对齐，如 `IDENTIFIER=258`） |
| `yylval` 常在 `test_lexer.c` 定义，或用 `TEST_LEXER_USE_REAL_YYLVAL` | **`ytab_runtime.c`** 统一提供 `yylval`、`yylloc`、`line_num` |
| 语法入口 **`main.cpp` + `yyparse()`** | **`seulr_parse()`**（`parser/seu_lr1_parser.cpp`），文法在 **`parser/grammar.cpp`** |

### 2.3 SeuLex 生成器

| 词法同学 `lex&yacc` | 本仓库 |
|---------------------|--------|
| `code_generator.cpp` 不生成 `yywrap` | 生成 **`int yywrap(void) { return 1; }`** |

### 2.4 词法测试与构建

| 词法同学 README §4 | 本仓库 |
|---------------------|--------|
| `gcc ... lex.yy.c test_lexer.c -o test_lexer.exe`（单步编译） | `gcc -c lex.yy.c` → `lex.yy.o`，`gcc -c ytab_runtime.c` → `ytab_runtime.o`，再链接 `test_lexer.exe` |
| 仅词法演示 | **`build_parse.cmd`**：SeuLex + `c99_parse.exe` + `c99_ir.exe` + 冒烟测试 |

### 2.5 本仓库新增、原版 `Lex/` 目录没有

- `parser/`：LR(1) 项集、**LALR 同心合并**、`reduce_prod` 建 AST  
- `semantic/`：三地址码骨架 `ir_from_ast.cpp`  
- `build_parse.cmd`、`docs/交付说明.md`  
- `c99_subset.l`、`c99_subset.y`、`tools/yacc_subset_scan.cpp`  
- 正式构建**不依赖** Flex / Bison（分支上仍可能保留 `c99.tab.c` 作对照）

### 2.6 Flex / Bison

| 项目 | 词法同学分支常见状态 | 本仓库 |
|------|----------------------|--------|
| 词法产物 | README 要求 SeuLex；仓库可能另有历史 Flex `lex.yy.c` | **`build_parse.cmd` 强制 SeuLex 生成** `lex.yy.c` |
| 语法 | Bison `c99.tab.c` + `yyparse` | 手写 LALR，**不用** `yyparse` |

---

## 3. 构建命令对照

### 3.1 词法同学原版（仅词法）

```powershell
g++ -std=c++17 -Iinclude src/main.cpp src/lex_parser.cpp src/regex_converter.cpp src/nfa.cpp src/dfa.cpp src/code_generator.cpp -o seulex.exe
./seulex.exe c99.l lex.yy.c
gcc -fdiagnostics-color=always -g -DASTNode=void -I. -Iinclude lex.yy.c test_lexer.c -o test_lexer.exe
./test_lexer.exe test_input.c
```

（记号头为 `c99.tab.hpp` 时适用；见 [GitHub README](https://github.com/wsyzznn/c99/blob/lex%26yacc/Lex/README.md)。该 README 目前含 **未解决的 git 合并冲突**，提交前应清理。）

### 3.2 本仓库（推荐）

在项目根目录（含 `build_parse.cmd` 的 `Lex` 文件夹）：

```bat
build_parse.cmd
```

仅测词法时：

```bat
seulex.exe c99.l lex.yy.c
gcc -std=c11 -Wall -c -I. -Iinclude lex.yy.c -o lex.yy.o
gcc -std=c11 -Wall -c -I. -Iinclude ytab_runtime.c -o ytab_runtime.o
gcc -std=c11 -g -I. -Iinclude -o test_lexer.exe test_lexer.c ytab_runtime.o lex.yy.o
test_lexer.exe test_input.c
```

期望输出示例：`263    int`、`258    main`（数字为 `y.tab.h` 中 token 编号）。

语法 + AST：

```bat
c99_parse.exe parser\test_syntax_ok.c
```

三地址骨架：

```bat
c99_ir.exe parser\test_syntax_ok.c
```

---

## 4. 与词法同学协作时注意

1. **改 `c99.l` 规则**：仍用 `seulex c99.l lex.yy.c`；联调请拉本仓库最新并跑 `build_parse.cmd`。  
2. **记号名/编号**：以 **`y.tab.h`** 与 **`c99.y` / `parser/grammar.cpp`** 一致为准，不要只改 `c99.tab.hpp`。  
3. **中间代码同学**：消费 **AST**（`ast.h`），入口 `seulr_parse` → `emit_ir_from_ast`；与 Flex/SeuLex 实现细节无关。  
4. **合并 `lex&yacc` 时**：保留 `%%` 规则；将 `#include "c99.tab.hpp"` 改为 **`y.tab.h`**，并链接 **`ytab_runtime.o`**。

---

## 5. 一句话总结

| 维度 | 结论 |
|------|------|
| 词法规则 | 与 [lex&yacc/Lex](https://github.com/wsyzznn/c99/tree/lex%26yacc/Lex) **基本相同** |
| SeuLex | **同源**，`code_generator` 略增 `yywrap` |
| 差异 | **接口头文件、yylloc/line_num、ytab_runtime、测试链接方式、手写 LALR、IR、build_parse** |
| 是否只改了测试命令 | **否** |

更完整的课件交付清单见同目录 **`交付说明.md`**。
