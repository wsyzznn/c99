#include <iostream>
#include <cstdio>
#include <string>
#include "include/ast.h"
#include "include/symbol.h"

// ================= 外部接口声明 =================
// 1. 词法/语法分析接口
extern int yyparse();
extern FILE *yyin;

// 2. 全局语法树根节点 (确保在 c99.y 中有定义并赋值)
extern ASTNode* g_ast_root; 

// 3. 后端模块入口函数声明
extern void semantic_analysis(ASTNode *root);      // 来自 semantic.cpp
extern void generate_three_address(ASTNode *root); // 来自 ir.cpp
extern void generate_c_from_ast(ASTNode* root);    // 来自 c_codegen.cpp
extern void generate_llvm_ir(ASTNode *root);       // 来自 llvm_codegen.cpp 入口

// 辅助函数：保存原始 stdout 并重定向到文件
void redirect_output(const char* filename) {
    if (freopen(filename, "w", stdout) == NULL) {
        std::cerr << "致命错误: 无法创建文件 " << filename << std::endl;
        exit(1);
    }
}

// 辅助函数：恢复 stdout 到控制台
void restore_stdout() {
#ifdef _WIN32
    freopen("CON", "w", stdout);
#else
    freopen("/dev/tty", "w", stdout);
#endif
}

int main(int argc, char **argv) {
    // 0. 准备输入文件
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            std::cerr << "致命错误: 无法打开源文件 " << argv[1] << std::endl;
            return 1;
        }
        std::cout << ">> 成功加载源文件: " << argv[1] << std::endl;
    } else {
        std::cout << ">> 未提供输入文件，请在控制台输入代码 (Ctrl+D 结束)..." << std::endl;
    }

    // ================== 编译流水线 ==================

    // 阶段 1：词法与语法分析
    std::cout << "\n[1/6] 正在执行词法与语法分析..." << std::endl;
    if (yyparse() != 0) {
        std::cerr << "[Error] 语法分析失败，编译终止！" << std::endl;
        return 1;
    }
    if (!g_ast_root) {
        std::cerr << "[Error] 语法树根节点为空，请检查 c99.y 配置！" << std::endl;
        return 1;
    }
    std::cout << "      -> 语法分析成功！" << std::endl;

    // 阶段 2：打印 AST 结构
    std::cout << "\n[2/6] 正在打印抽象语法树 (AST)..." << std::endl;
    std::cout << "================ AST ================" << std::endl;
    print_ast(g_ast_root, 0); 
    std::cout << "=====================================" << std::endl;

    // 阶段 3：语义分析与符号表构建
    std::cout << "\n[3/6] 正在执行语义分析..." << std::endl;
    semantic_analysis(g_ast_root);
    std::cout << "      -> 语义分析完成！" << std::endl;

    // 阶段 4：生成三地址码 (TAC) 并在控制台显示
    std::cout << "\n[4/6] 正在生成三地址码 (TAC)..." << std::endl;
    std::cout << "================ TAC ================" << std::endl;
    generate_three_address(g_ast_root);
    std::cout << "=====================================" << std::endl;

    // 阶段 5：生成 output_temp.c (元编译代码)
    std::cout << "\n[5/6] 正在生成 output_temp.c..." << std::endl;
    redirect_output("output_temp.c");
    generate_c_from_ast(g_ast_root);
    restore_stdout();
    std::cout << "      -> 文件 output_temp.c 已保存。" << std::endl;

    // 阶段 6：生成 output.ll (LLVM IR)
    std::cout << "\n[6/6] 正在生成 output.ll..." << std::endl;
    redirect_output("output.ll");
    // 注意：确保 llvm_codegen.cpp 中有这个包装好的入口
    generate_llvm_ir(g_ast_root); 
    restore_stdout();
    std::cout << "      -> 文件 output.ll 已保存。" << std::endl;

    std::cout << "\n>> 编译流程全部顺利完成！" << std::endl;

    return 0;
}