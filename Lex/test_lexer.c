/* test_lexer.c - Minimal driver for generated lex.yy.c */
#include <stdio.h>

/* Build tip: compile with -DASTNode=void if needed by c99.tab.hpp. */
#ifndef ASTNode
typedef void ASTNode;
#endif

#include "c99.tab.hpp"
#include "include/symbol.h"

extern int yylex(void);
extern FILE *yyin;
extern char yytext[];

#ifndef TEST_LEXER_USE_REAL_YYLVAL
YYSTYPE yylval;
#endif

#ifndef TEST_LEXER_USE_REAL_SYMBOL
void symbol_table_init(void) {}
void symbol_insert(Symbol *symbol) { (void)symbol; }
Symbol* symbol_lookup(const char *name) { (void)name; return NULL; }
void symbol_enter_scope(void) {}
void symbol_exit_scope(void) {}
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
