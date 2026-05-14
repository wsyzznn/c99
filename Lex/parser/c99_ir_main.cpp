/**
 * 词法 + LALR(1) 语法分析 + 三地址码骨架输出（中间代码生成演示入口）
 */
#include "seu_lr1_parser.hpp"
#include "../ast.h"
#include "semantic/ir_from_ast.hpp"

#include <cstdio>
#include <cstdlib>

extern "C" {
extern FILE *yyin;
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
    if (seulr_parse(fp, &root) != 0 || !root) {
        std::fputs("PARSE_FAIL\n", stderr);
        std::fclose(fp);
        return 1;
    }
    std::fclose(fp);
    emit_ir_from_ast(stdout, root);
    ast_free(root);
    return 0;
}
