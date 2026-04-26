#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "../include/ast.h"
#include "../include/symbol.h"

// 【关键】：伪造词法分析器的行号变量，防止与 ast.cpp 链接时报错
int yylineno = 1;

// 声明外部核心函数（确保这些函数在你的 .cpp 文件中已实现）
extern void print_ast(ASTNode *node, int depth);
extern void generate_three_address(ASTNode *root);
extern CType check_semantic_internal(ASTNode *node);
extern void generate_c_from_ast(ASTNode *root);
extern void generate_llvm_ir(ASTNode *root); // 新增：LLVM 生成函数声明

int main()
{
    std::cout << "========== [综合测试] 模拟复杂 AST 构造、元编译与 LLVM 生成 ==========" << std::endl;

    /*
     * 手工构造以下逻辑的 AST:
     * {
     * a = b + c * 2;
     * if (a < 100) { a = a + 1; } else { a = a - 1; }
     * while (b > 0) { b = b - 1; }
     * }
     */

    // 1. 构造表达式: a = b + c * 2
    ASTNode *var_a = ast_var((char *)"a");
    ASTNode *var_b = ast_var((char *)"b");
    ASTNode *var_c = ast_var((char *)"c");
    ASTNode *expr_mul = ast_binop('*', var_c, ast_int(2));
    ASTNode *expr_add = ast_binop('+', var_b, expr_mul);
    ASTNode *stmt_assign1 = ast_assign(var_a, expr_add);

    // 2. 构造 If-Else 语句
    ASTNode *cond_if = ast_binop('<', var_a, ast_int(100));
    ASTNode *then_s = ast_assign(var_a, ast_binop('+', var_a, ast_int(1)));
    ASTNode *else_s = ast_assign(var_a, ast_binop('-', var_a, ast_int(1)));
    ASTNode *stmt_if = ast_if(cond_if, then_s, else_s);

    // 3. 构造 While 语句
    ASTNode *cond_while = ast_binop('>', var_b, ast_int(0));
    ASTNode *while_body = ast_assign(var_b, ast_binop('-', var_b, ast_int(1)));
    ASTNode *stmt_while = ast_while(cond_while, while_body);

    // 4. 打包进复合语句块
    ASTNode **block_list = (ASTNode **)malloc(3 * sizeof(ASTNode *));
    block_list[0] = stmt_assign1;
    block_list[1] = stmt_if;
    block_list[2] = stmt_while;
    ASTNode *root_compound = ast_compound(block_list, 3);

    // --- 阶段 1: 打印 AST ---
    std::cout << "\n[1/5] 正在打印 AST 结构..." << std::endl;
    print_ast(root_compound, 0);

    // --- 阶段 2: 符号表与语义分析 ---
    std::cout << "\n[2/5] 正在执行语义分析..." << std::endl;
    symbol_table_init();
    Symbol sym_a;
    sym_a.name = "a";
    sym_a.c_type = TYPE_INT;
    sym_a.scope = 0;
    Symbol sym_b;
    sym_b.name = "b";
    sym_b.c_type = TYPE_INT;
    sym_b.scope = 0;
    Symbol sym_c;
    sym_c.name = "c";
    sym_c.c_type = TYPE_INT;
    sym_c.scope = 0;
    symbol_insert(&sym_a);
    symbol_insert(&sym_b);
    symbol_insert(&sym_c);
    check_semantic_internal(root_compound);
    std::cout << "语义检查完成。" << std::endl;

    // --- 阶段 3: 控制台生成三地址码 (TAC) ---
    std::cout << "\n[3/5] 正在控制台输出三地址码 (TAC)..." << std::endl;
    generate_three_address(root_compound);

    // --- 阶段 4: 元编译 (生成 output_temp.c) ---
    std::cout << "\n[4/5] 正在元编译生成 output_temp.c ..." << std::endl;
    if (freopen("output_temp.c", "w", stdout) == NULL)
    {
        fprintf(stderr, "无法创建 output_temp.c 文件！\n");
        return -1;
    }
    generate_c_from_ast(root_compound);

// 恢复标准输出
#ifdef _WIN32
    freopen("CON", "w", stdout);
#else
    freopen("/dev/tty", "w", stdout);
#endif

    // --- 阶段 5: 生成 LLVM IR (output.ll) ---
    std::cout << "\n[5/5] 正在生成 LLVM IR (output.ll) ..." << std::endl;
    if (freopen("output.ll", "w", stdout) == NULL)
    {
        fprintf(stderr, "无法创建 output.ll 文件！\n");
        return -1;
    }

    // 调用你修改后的 llvm_codegen.cpp 中的入口
    generate_llvm_ir(root_compound);

// 再次恢复标准输出
#ifdef _WIN32
    freopen("CON", "w", stdout);
#else
    freopen("/dev/tty", "w", stdout);
#endif

    std::cout << "\n全部测试完成！" << std::endl;
    std::cout << "- C 代码已输出至: output_temp.c" << std::endl;
    std::cout << "- LLVM IR 已输出至: output.ll" << std::endl;

    return 0;
}