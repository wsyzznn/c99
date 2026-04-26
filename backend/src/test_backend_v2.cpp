#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "../include/ast.h"
#include "../include/symbol.h"

// 伪造词法分析器的行号变量
int yylineno = 1;

// 声明后端接口
extern void print_ast(ASTNode *node, int depth);
extern void semantic_analysis(ASTNode *root);
extern void generate_three_address(ASTNode *root);
extern void generate_llvm_ir(ASTNode *root);

int main() {
    std::cout << "========== [模拟 test.c] 手工构造 AST 与全流程测试 ==========" << std::endl;

    // ---------------------------------------------------------
    // 1. 构造全局变量声明: int global_var;
    // ---------------------------------------------------------
    ASTNode* g_var_node = (ASTNode*)calloc(1, sizeof(ASTNode));
    g_var_node->type = AST_DECLARATION;
    // Type specifier: int
    g_var_node->declaration.specs = (ASTNode**)malloc(sizeof(ASTNode*));
    g_var_node->declaration.specs[0] = ast_var((char*)"int");
    g_var_node->declaration.scount = 1;
    // Init declarator: global_var
    ASTNode* g_init = (ASTNode*)calloc(1, sizeof(ASTNode));
    g_init->type = AST_INIT_DECL;
    g_init->id.declr = ast_var((char*)"global_var");
    g_var_node->declaration.inits = (ASTNode**)malloc(sizeof(ASTNode*));
    g_var_node->declaration.inits[0] = g_init;
    g_var_node->declaration.icount = 1;

    // ---------------------------------------------------------
    // 2. 构造 main 函数内部逻辑
    // ---------------------------------------------------------
    
    // (1) 局部变量声明: int a, b;
    ASTNode* local_decl = (ASTNode*)calloc(1, sizeof(ASTNode));
    local_decl->type = AST_DECLARATION;
    local_decl->declaration.specs = (ASTNode**)malloc(sizeof(ASTNode*));
    local_decl->declaration.specs[0] = ast_var((char*)"int");
    local_decl->declaration.scount = 1;
    
    ASTNode* init_a = (ASTNode*)calloc(1, sizeof(ASTNode));
    init_a->type = AST_INIT_DECL; init_a->id.declr = ast_var((char*)"a");
    ASTNode* init_b = (ASTNode*)calloc(1, sizeof(ASTNode));
    init_b->type = AST_INIT_DECL; init_b->id.declr = ast_var((char*)"b");
    
    local_decl->declaration.inits = (ASTNode**)malloc(2 * sizeof(ASTNode*));
    local_decl->declaration.inits[0] = init_a;
    local_decl->declaration.inits[1] = init_b;
    local_decl->declaration.icount = 2;

    // (2) a = 5;
    ASTNode* assign_a = ast_assign(ast_var((char*)"a"), ast_int(5));

    // (3) b = a + 10 * 2; (测试优先级：先乘后加)
    ASTNode* mul_expr = ast_binop('*', ast_int(10), ast_int(2));
    ASTNode* add_expr = ast_binop('+', ast_var((char*)"a"), mul_expr);
    ASTNode* assign_b = ast_assign(ast_var((char*)"b"), add_expr);

    // (4) if (a < b) { a = a + 1; } else { b = b - 1; }
    ASTNode* if_cond = ast_binop('<', ast_var((char*)"a"), ast_var((char*)"b"));
    ASTNode* then_stmt = ast_assign(ast_var((char*)"a"), ast_binop('+', ast_var((char*)"a"), ast_int(1)));
    ASTNode* else_stmt = ast_assign(ast_var((char*)"b"), ast_binop('-', ast_var((char*)"b"), ast_int(1)));
    ASTNode* if_node = ast_if(if_cond, then_stmt, else_stmt);

    // (5) while (a < 20) { a = a + 2; }
    ASTNode* while_cond = ast_binop('<', ast_var((char*)"a"), ast_int(20));
    ASTNode* while_body = ast_assign(ast_var((char*)"a"), ast_binop('+', ast_var((char*)"a"), ast_int(2)));
    ASTNode* while_node = ast_while(while_cond, while_body);

    // (6) return a;
    ASTNode* ret_node = (ASTNode*)calloc(1, sizeof(ASTNode));
    ret_node->type = AST_RETURN;
    ret_node->expr = ast_var((char*)"a");

    // ---------------------------------------------------------
    // 3. 组装 main 函数
    // ---------------------------------------------------------
    ASTNode* main_body_list[] = {local_decl, assign_a, assign_b, if_node, while_node, ret_node};
    ASTNode* main_body = ast_compound(main_body_list, 6);

    ASTNode* main_func = (ASTNode*)calloc(1, sizeof(ASTNode));
    main_func->type = AST_FUNC_TYPE;
    main_func->ft.name = (char*)"main";
    main_func->ft.ret_type = ast_var((char*)"int");
    main_func->ft.body = main_body;

    // ---------------------------------------------------------
    // 4. 组装根节点 (Translation Unit)
    // ---------------------------------------------------------
    ASTNode* root = (ASTNode*)calloc(1, sizeof(ASTNode));
    root->type = AST_TRANSLATION_UNIT;
    root->seq.list = (ASTNode**)malloc(2 * sizeof(ASTNode*));
    root->seq.list[0] = g_var_node; // 全局变量
    root->seq.list[1] = main_func;  // main 函数
    root->seq.count = 2;

    // ---------------------------------------------------------
    // 5. 执行后端流程
    // ---------------------------------------------------------
    std::cout << "\n[1/4] 打印 AST 结构..." << std::endl;
    print_ast(root, 0);

    std::cout << "\n[2/4] 执行语义分析 (建立符号表)..." << std::endl;
    semantic_analysis(root);

    std::cout << "\n[3/4] 控制台生成三地址码 (TAC)..." << std::endl;
    generate_three_address(root);

    std::cout << "\n[4/4] 生成 LLVM IR..." << std::endl;
    generate_llvm_ir(root);

    return 0;
}