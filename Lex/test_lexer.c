/* test_lexer.c - Minimal driver for generated lex.yy.c */
#include <stdio.h>

#include "y.tab.h"

extern int yylex(void);
extern FILE *yyin;
extern char yytext[];

#ifndef TEST_LEXER_USE_REAL_YYLVAL
/* 默认使用 ytab_runtime.c 中的定义；单文件调试可定义 TEST_LEXER_USE_REAL_YYLVAL 并自行提供 */
#endif

int main(int argc, char *argv[]) {
    const char *input_path = (argc > 1) ? argv[1] : "test_input.c";
    int tok;

    yyin = fopen(input_path, "r");
    if (!yyin) {
        fprintf(stderr, "Cannot open %s\n", input_path);
        return 1;
    }

    while ((tok = yylex()) != 0) {
        printf("%d\t%s\n", tok, yytext);
    }

    fclose(yyin);
    return 0;
}
