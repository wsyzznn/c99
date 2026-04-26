#include "../../include/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

// 如果使用 yylloc
//  typedef struct YYLTYPE {
//      int first_line;
//      int first_column;
//      int last_line;
//      int last_column;
//  } YYLTYPE;
//  extern YYLTYPE yylloc;

// 内部通用的节点分配函数
static ASTNode *create_node(ASTNodeType type)
{
    // 用 calloc 保证所有指针初始为 NULL，防止野指针
    ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
    node->type = type;
    extern int yylineno; // 如果前端提供了 yylineno，可以在这里直接赋值
    node->line = yylineno;

    // 如果前端使用了 yylloc 结构体来跟踪位置，可以在这里赋值行号
    // node->line = yylloc.first_line;

    return node;
}

ASTNode *ast_int(int val)
{
    ASTNode *node = create_node(AST_INT);
    node->intval = val;
    return node;
}

ASTNode *ast_double(double val)
{
    ASTNode *node = create_node(AST_DOUBLE);
    node->fval = val;
    return node;
}

ASTNode *ast_var(char *name)
{
    ASTNode *node = create_node(AST_VAR);
    node->varname = strdup(name); // 深拷贝字符串
    return node;
}

ASTNode *ast_binop(char op, ASTNode *lhs, ASTNode *rhs)
{
    ASTNode *node = create_node(AST_BINOP);
    node->bin.op = op;
    node->bin.lhs = lhs;
    node->bin.rhs = rhs;
    return node;
}

ASTNode *ast_assign(ASTNode *l, ASTNode *r)
{
    ASTNode *node = create_node(AST_ASSIGN);
    node->assign.l = l;
    node->assign.r = r;
    return node;
}

ASTNode* ast_call(char* fname, ASTNode** args, int argc) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_CALL;
    // 假设 yacc 传过来的 fname 已经是分配好内存的字符串
    node->call.fname = fname; 
    node->call.args = args;
    node->call.argc = argc;
    // node->line = current_line; // 如果你有全局行号可以加上
    return node;
}

ASTNode *ast_if(ASTNode *cond, ASTNode *then_s, ASTNode *else_s)
{
    ASTNode *node = create_node(AST_IF);
    node->sif.cond = cond;
    node->sif.then_s = then_s;
    node->sif.else_s = else_s; // else_s 可以为 NULL
    return node;
}

ASTNode *ast_while(ASTNode *cond, ASTNode *body)
{
    ASTNode *node = create_node(AST_WHILE);
    node->sw.cond = cond;
    node->sw.body = body;
    return node;
}

ASTNode *ast_for(ASTNode *init, ASTNode *cond, ASTNode *post, ASTNode *body)
{
    ASTNode *node = create_node(AST_FOR);
    node->sf.init = init;
    node->sf.cond = cond;
    node->sf.post = post;
    node->sf.body = body;
    return node;
}

ASTNode *ast_compound(ASTNode **list, int count)
{
    ASTNode *node = create_node(AST_COMPOUND_STMT);
    node->seq.count = count;
    if (count > 0 && list != NULL)
    {
        node->seq.list = (ASTNode **)malloc(sizeof(ASTNode *) * count);
        memcpy(node->seq.list, list, sizeof(ASTNode *) * count);
    }
    else
    {
        node->seq.list = NULL;
    }
    return node;
}

// 递归释放整棵树的内存
void ast_free(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_VAR:
        if (node->varname)
            free(node->varname);
        break;
    case AST_BINOP:
        ast_free(node->bin.lhs);
        ast_free(node->bin.rhs);
        break;
    case AST_ASSIGN:
        ast_free(node->assign.l);
        ast_free(node->assign.r);
        break;
    case AST_IF:
        ast_free(node->sif.cond);
        ast_free(node->sif.then_s);
        ast_free(node->sif.else_s);
        break;
    case AST_WHILE:
        ast_free(node->sw.cond);
        ast_free(node->sw.body);
        break;
    case AST_FOR:
        ast_free(node->sf.init);
        ast_free(node->sf.cond);
        ast_free(node->sf.post);
        ast_free(node->sf.body);
        break;

    case AST_COMPOUND_STMT:
        for (int i = 0; i < node->seq.count; i++)
        {
            ast_free(node->seq.list[i]);
        }
        if (node->seq.list)
            free(node->seq.list);
        break;
    // 其他类型的释放逻辑可随开发进度补充
    default:
        break;
    }
    free(node); // 释放当前节点本身
}

// 4.15新增打印语法树函数
void print_ast(ASTNode *node, int depth)
{
    if (!node)
        return;

    // 打印缩进
    for (int i = 0; i < depth; ++i)
        std::cout << "  ";

    switch (node->type)
    {
    case AST_TRANSLATION_UNIT:
        std::cout << "|- TranslationUnit" << std::endl;
        break;
    case AST_FUNC_TYPE:
        std::cout << "|- Function: " << node->ft.name << std::endl;
        print_ast(node->ft.body, depth + 1);
        return;
    case AST_COMPOUND_STMT:
        std::cout << "|- CompoundStmt" << std::endl;
        break;
    case AST_DECLARATION:
        std::cout << "|- Declaration" << std::endl;
        break;
    case AST_BINOP:
        std::cout << "|- BinaryExpr: " << node->bin.op << std::endl;
        print_ast(node->bin.lhs, depth + 1);
        print_ast(node->bin.rhs, depth + 1);
        return;
    case AST_VAR:
        std::cout << "|- Identifier: " << (node->varname ? node->varname : "null") << std::endl;
        return;
    case AST_INT:
        std::cout << "|- Int: " << node->intval << std::endl;
        return;
    case AST_IF:
        std::cout << "|- IfStmt" << std::endl;
        print_ast(node->sif.cond, depth + 1);
        print_ast(node->sif.then_s, depth + 1);
        if (node->sif.else_s)
            print_ast(node->sif.else_s, depth + 1);
        return;
    case AST_RETURN:
        std::cout << "|- Return" << std::endl;
        print_ast(node->expr, depth + 1);
        return;
    case AST_ASSIGN:
        std::cout << "|- Assign" << std::endl;
        print_ast(node->assign.l, depth + 1);
        print_ast(node->assign.r, depth + 1);
        return;
    case AST_WHILE:
        std::cout << "|- WhileStmt" << std::endl;
        print_ast(node->sw.cond, depth + 1);
        print_ast(node->sw.body, depth + 1);
        return;
    // [新增: 打印 FOR 循环节点]
    case AST_FOR:
        std::cout << "|- ForStmt" << std::endl;
        if (node->sf.init) {
            for(int i=0; i<depth+1; ++i) std::cout << "  ";
            std::cout << "|- Init:" << std::endl;
            print_ast(node->sf.init, depth + 2);
        }
        if (node->sf.cond) {
            for(int i=0; i<depth+1; ++i) std::cout << "  ";
            std::cout << "|- Cond:" << std::endl;
            print_ast(node->sf.cond, depth + 2);
        }
        if (node->sf.post) {
            for(int i=0; i<depth+1; ++i) std::cout << "  ";
            std::cout << "|- Post:" << std::endl;
            print_ast(node->sf.post, depth + 2);
        }
        if (node->sf.body) {
            print_ast(node->sf.body, depth + 1);
        }
        return;

    // [新增: 打印函数调用节点]
    case AST_CALL:
        std::cout << "|- Call: " << node->call.fname << std::endl;
        for (int i = 0; i < node->call.argc; i++) {
            print_ast(node->call.args[i], depth + 1);
        }
        return;
        
    // [新增: 打印 Break/Continue]
    case AST_BREAK:
        std::cout << "|- Break" << std::endl;
        return;
    case AST_CONTINUE:
        std::cout << "|- Continue" << std::endl;
        return;
        
    default:
        std::cout << "|- Node(" << node->type << ")" << std::endl;
        break;
    }

    // 处理序列
    if (node->type == AST_TRANSLATION_UNIT || node->type == AST_COMPOUND_STMT)
    {
        for (int i = 0; i < node->seq.count; i++)
        {
            print_ast(node->seq.list[i], depth + 1);
        }
    }
}