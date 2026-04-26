# C99 Compiler Backend - 

本项目是一个基于 C/C++ 实现的 C99 子集编译器。本模块主要包含了编译器的**中后端**部分，采用现代编译器的**多遍编译（Multi-pass）** 架构设计。它接收前端（Lex/Yacc）构建的抽象语法树（AST），执行严格的静态语义检查，并支持生成三种不同的底层目标代码。

## 架构设计 (Architecture)

本编译器后端摒弃了传统的单遍（Single-pass）边解析边生成的模式，将编译过程严格解耦：
1. **Pass 1 (Frontend)**: 语法制导翻译构建 AST (`ast.cpp`)。
2. **Pass 2 (Semantic)**: 遍历 AST，构建作用域敏感的符号表，执行类型检查 (`semantic.cpp`, `symbol_table.cpp`)。
3. **Pass 3 (Codegen)**: 遍历校验后的 AST，执行控制流的拉链回填，生成目标代码。

## 文件目录说明

### 1. 核心数据结构
* **`ast.h` / `ast.cpp`**: 抽象语法树（AST）的核心定义与工厂模式构造函数。包含了所有语法节点的联合体定义，以及用于可视化的 `print_ast` 函数。
* **`symbol.h` / `symbol_table.cpp`**: 符号表管理器。采用 `vector<unordered_map>` 实现栈式符号表，支持 C 语言的多层块级作用域（Block Scope）嵌套、变量遮蔽及内存偏移量计算。

### 2. 语义分析
* **`semantic.cpp`**: 语义分析引擎。负责对 AST 进行深度优先遍历，完成以下核心任务：
  * **符号注册**：将变量、函数存入符号表并查重（Redefinition）。
  * **合法性校验**：检查未声明变量的使用（Undeclared identifier）。
  * **类型推导与检查**：推导表达式结果类型，检查赋值类型匹配。

### 3. 代码生成器组
本编译器支持将 AST 翻译为三种不同维度的底层代码：
* **`ir.cpp` (核心)**: 三地址码（TAC / 四元式）生成器。严格实现了基于布尔表达式的**拉链回填（Backpatching）** 算法，生成精准的控制流跳转逻辑。
* **`llvm_codegen.cpp`**: LLVM IR 汇编生成器。生成符合静态单赋值（SSA）形式的底层中间代码，包含内存分配 (`alloca`)、读写 (`load/store`) 及条件分支 (`br`)。
* **`c_codegen.cpp`**: C语言元编译生成器。一种特殊的目标代码生成，将 AST 结构翻译为能够动态输出三地址码的 C 语言源码（`output_temp.c`）。

## 编译与运行指南

### 环境依赖
* `g++` (支持 C++11 或以上)
* `flex` 和 `bison` (用于前端编译/小组自研编译器)

### 中间代码编译步骤
1. **准备测试基座**：
使用项目中的 src/test_backend.cpp。该文件通过手动调用 ast_xxx 接口，模拟构造了以下 C 逻辑的语法树：
    ```bash
    a = b + c * 2;
    if (a < 100) { a = a + 1; } else { a = a - 1; }
    while (b > 0) { b = b - 1; }
或者test_backend_v2.cpp：

    int global_var;
    int main() {
    int a, b;
    a = 5;
    b = a + 10 * 2;  // 测试优先级：应该先算 10*2

    // 测试控制流与拉链回填
    if (a < b) {
        a = a + 1;
    } else {
        b = b - 1;
    }

    // 测试循环控制流
    while (a < 20) {
        a = a + 2;
    }

    return a;
}

2. **编译后端测试程序**
    ```bash
    g++ src/test_backend.cpp src/semantic/ast.cpp src/semantic/symbol_table.cpp src/semantic/semantic.cpp src/ir/ir.cpp src/ir/c_codegen.cpp src/ir/llvm_codegen.cpp -o test_backend -std=c++11 -fexec-charset=GBK
3. **运行并查看结果**
    ```bash
    ./test_backend
预期输出：控制台将打印 AST 结构和带行号回填的三地址码。

生成产物：同时在当前目录下生成 output_temp.c 和 output.ll。


### 前后端联调步骤
1. **生成前端解析器（用小组自研编译器替换）**：
   ```bash
   bison -d c99.y
   flex c99.l
2. **联合编译全模块**：
将前端生成的 C 代码与后端所有模块及主控程序联合编译：
    ```bash
    g++ -o my_compiler lex.yy.c c99.tab.c src/semantic/ast.cpp src/semantic/symbol_table.cpp src/semantic/semantic.cpp src/ir/ir.cpp src/ir/c_codegen.cpp src/ir/llvm_codegen.cpp src/main.cpp -std=c++11
由于ast.h和symbol.h文件路径问题，可在结尾强行加上/include/src调用这两个文件，以及在所有文件执行之前访问ast.h文件，防止因为文件顺序问题报错：

    g++ -include include/ast.h lex.yy.c c99.tab.cpp src/main.cpp src/semantic/ast.cpp src/semantic/symbol_table.cpp src/semantic/semantic.cpp src/ir/ir.cpp src/ir/c_codegen.cpp src/ir/llvm_codegen.cpp -o compiler.exe -std=c++11 -static -fexec-charset=GBK -Iinclude -Isrc -I.
3. 运行编译器测试 C 文件(test.c)
    ```bash
    ./my_compiler < test.c