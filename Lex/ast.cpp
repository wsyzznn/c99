#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 确保在 C++ 环境下使用 C 风格链接 */
extern "C" {

/* 辅助函数：创建基础节点并初始化 */
/* 注意：在 C++ 中 malloc 返回 void* 必须强转 */
static ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    memset(node, 0, sizeof(ASTNode)); 
    node->type = type;
    return node;
}

/* --- 构造函数实现 --- */

ASTNode* ast_int(int val) {
    ASTNode* node = create_node(AST_INT);
    node->intval = val;
    return node;
}

ASTNode* ast_double(double val) {
    ASTNode* node = create_node(AST_DOUBLE);
    node->fval = val;
    return node;
}

ASTNode* ast_var(char* name) {
    ASTNode* node = create_node(AST_VAR);
    node->varname = name ? strdup(name) : NULL;
    return node;
}

ASTNode* ast_binop(char op, ASTNode* lhs, ASTNode* rhs) {
    ASTNode* node = create_node(AST_BINOP);
    node->bin.op = op;
    node->bin.lhs = lhs;
    node->bin.rhs = rhs;
    return node;
}

ASTNode* ast_assign(ASTNode* l, ASTNode* r) {
    ASTNode* node = create_node(AST_ASSIGN);
    node->assign.l = l;
    node->assign.r = r;
    return node;
}

ASTNode* ast_call(char* name, ASTNode** arguments, int arg_count) {
    ASTNode* node = create_node(AST_CALL);
    node->call.fname = name ? strdup(name) : NULL;
    node->call.args = arguments;
    node->call.argc = arg_count;
    return node;
}

ASTNode* ast_if(ASTNode* cond, ASTNode* then_s, ASTNode* else_s) {
    ASTNode* node = create_node(AST_IF);
    node->sif.cond = cond;
    node->sif.then_s = then_s;
    node->sif.else_s = else_s;
    return node;
}

ASTNode* ast_while(ASTNode* cond, ASTNode* body) {
    ASTNode* node = create_node(AST_WHILE);
    node->sw.cond = cond;
    node->sw.body = body;
    return node;
}

ASTNode* ast_for(ASTNode* init, ASTNode* cond, ASTNode* post, ASTNode* body) {
    ASTNode* node = create_node(AST_FOR);
    node->sf.init = init;
    node->sf.cond = cond;
    node->sf.post = post;
    node->sf.body = body;
    return node;
}

ASTNode* ast_compound(ASTNode** list, int count) {
    ASTNode* node = create_node(AST_COMPOUND_STMT);
    node->seq.list = list;
    node->seq.count = count;
    return node;
}

/* --- 内存释放 --- */
void ast_free(ASTNode* node) {
    if (!node) return;
    
    if (node->type == AST_VAR) free(node->varname);
    if (node->type == AST_CALL) {
        if (node->call.fname) free(node->call.fname);
        if (node->call.args) free(node->call.args);
    }
    // 递归释放建议在稳定后再添加，防止因 AST 结构未闭合导致段错误
    free(node);
}

} // end extern "C"