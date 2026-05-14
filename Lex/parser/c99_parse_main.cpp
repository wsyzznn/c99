/**
 * 词法 lex.yy.c + LR(1) 语法分析 入口
 */
#include "seu_lr1_parser.hpp"
#include "../ast.h"

#include <cstdio>
#include <cstdlib>

extern "C" {
extern FILE *yyin;
}

static void print_ast(ASTNode *node, int depth) {
    if (!node)
        return;
    for (int i = 0; i < depth; ++i)
        printf("  ");
    switch (node->type) {
        case AST_INT:
            printf("INT %d\n", node->intval);
            break;
        case AST_DOUBLE:
            printf("DOUBLE %g\n", node->fval);
            break;
        case AST_STRING:
            printf("STRING %s\n", node->str ? node->str : "");
            break;
        case AST_VAR:
            printf("VAR %s\n", node->varname ? node->varname : "?");
            break;
        case AST_BINOP:
            printf("BINOP '%c'\n", node->bin.op);
            print_ast(node->bin.lhs, depth + 1);
            print_ast(node->bin.rhs, depth + 1);
            break;
        case AST_ASSIGN:
            printf("ASSIGN\n");
            print_ast(node->assign.l, depth + 1);
            print_ast(node->assign.r, depth + 1);
            break;
        case AST_CALL:
            printf("CALL %s\n", node->call.fname ? node->call.fname : "?");
            for (int i = 0; i < node->call.argc; ++i)
                print_ast(node->call.args[i], depth + 1);
            break;
        case AST_COMPOUND_STMT:
            printf("BLOCK (%d)\n", node->seq.count);
            for (int i = 0; i < node->seq.count; ++i)
                print_ast(node->seq.list[i], depth + 1);
            break;
        case AST_IF:
            printf("IF\n");
            print_ast(node->sif.cond, depth + 1);
            print_ast(node->sif.then_s, depth + 1);
            print_ast(node->sif.else_s, depth + 1);
            break;
        case AST_WHILE:
            printf("WHILE\n");
            print_ast(node->sw.cond, depth + 1);
            print_ast(node->sw.body, depth + 1);
            break;
        case AST_DO_WHILE:
            printf("DO_WHILE\n");
            print_ast(node->sw.body, depth + 1);
            print_ast(node->sw.cond, depth + 1);
            break;
        case AST_FOR:
            printf("FOR\n");
            print_ast(node->sf.init, depth + 1);
            print_ast(node->sf.cond, depth + 1);
            print_ast(node->sf.post, depth + 1);
            print_ast(node->sf.body, depth + 1);
            break;
        case AST_RETURN:
            printf("RETURN\n");
            print_ast(node->expr, depth + 1);
            break;
        case AST_BREAK:
            printf("BREAK\n");
            break;
        case AST_CONTINUE:
            printf("CONTINUE\n");
            break;
        case AST_DECLARATION:
            printf("DECL (%d inits)\n", node->declaration.icount);
            for (int i = 0; i < node->declaration.icount; ++i)
                print_ast(node->declaration.inits[i], depth + 1);
            break;
        case AST_INIT_DECL:
            printf("INIT_DECL\n");
            print_ast(node->id.declr, depth + 1);
            print_ast(node->id.init, depth + 1);
            break;
        case AST_FUNC_TYPE:
            printf("FUNC %s\n", node->ft.name ? node->ft.name : "?");
            print_ast(node->ft.body, depth + 1);
            break;
        case AST_PARAM_LIST:
            printf("PARAMS (%d)\n", node->seq.count);
            for (int i = 0; i < node->seq.count; ++i)
                print_ast(node->seq.list[i], depth + 1);
            break;
        case AST_TRANSLATION_UNIT:
            printf("TU (%d)\n", node->seq.count);
            for (int i = 0; i < node->seq.count; ++i)
                print_ast(node->seq.list[i], depth + 1);
            break;
        default:
            printf("NODE %d\n", (int)node->type);
            break;
    }
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "parser/test_syntax_ok.c";
    FILE *fp = std::fopen(path, "r");
    if (!fp) {
        std::perror(path);
        return 1;
    }
    yyin = fp;
    ASTNode *root = nullptr;
    int rc = seulr_parse(fp, &root);
    std::fclose(fp);
    if (rc != 0 || !root) {
        printf("PARSE_FAIL rc=%d\n", rc);
        return 1;
    }
    printf("=== PARSE OK ===\n");
    print_ast(root, 0);
    ast_free(root);
    return 0;
}
