#pragma once
#include <cstdio>

struct ASTNode;

#ifdef __cplusplus
extern "C" {
#endif

/** 初始化文法、求 First、构造 LR(1) 项集族后压缩为 LALR(1)，生成 ACTION/GOTO 表 */
int seulr_init(void);

/**
 * 词法 yylex + LALR(1) 总控（由 LR(1) 同心项合并得到分析表），成功返回 0，*root 为 AST_TRANSLATION_UNIT。
 * 调用前设置 yyin；链接 lex.yy.c、ast、ytab_runtime。
 */
int seulr_parse(FILE *in, ASTNode **root);

#ifdef __cplusplus
}
#endif
