#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

// 引用来自 Flex 和 Bison 的全局变量/函数
extern int yyparse();
extern FILE* yyin;
extern int yylineno;
ASTNode* g_ast_root = NULL; // 语法树根节点

// 简单的可视化函数：打印语法树结构
void print_ast(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");

    switch (node->type) {
        case AST_INT:    printf("INT: %d\n", node->intval); break;
        case AST_VAR:    printf("VAR: %s\n", node->varname); break;
        case AST_BINOP:  printf("BINOP: %c\n", node->bin.op); 
                         print_ast(node->bin.lhs, level + 1);
                         print_ast(node->bin.rhs, level + 1);
                         break;
        case AST_IF:     printf("IF_STMT\n"); break;
        case AST_FUNC_DEF: printf("FUNC_DEF\n"); break;
        case AST_TRANSLATION_UNIT: 
                         printf("TRANSLATION_UNIT (Root)\n");
                         for(int i=0; i<node->seq.count; i++) print_ast(node->seq.list[i], level+1);
                         break;
        default:         printf("NODE_TYPE: %d\n", node->type); break;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror("File Error");
        return 1;
    }

    yyin = fp; // 将 Flex 的输入指向文件
    printf("--- Starting C99 Parser ---\n");

    if (yyparse() == 0 && g_ast_root != NULL) {
        printf("\n[Success] Syntax tree built successfully!\n\n");
        printf("AST Structure:\n");
        print_ast(g_ast_root, 0);
    } else {
        printf("\n[Error] Syntax analysis failed at line %d\n", yylineno);
    }

    fclose(fp);
    return 0;
}