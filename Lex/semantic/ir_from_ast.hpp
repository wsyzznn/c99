#pragma once
#include <cstdio>

struct ASTNode;

#ifdef __cplusplus
extern "C" {
#endif
/** 将语法树打印为教材风格三地址码（子集：算术、赋值、顺序块；控制流为占位标签） */
void emit_ir_from_ast(FILE *out, ASTNode *root);
#ifdef __cplusplus
}
#endif
