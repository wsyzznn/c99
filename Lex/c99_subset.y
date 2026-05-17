/* c99_subset.y — 子集：记号与 y.tab.h 一致；完整产生式在 parser/grammar.cpp（手写） */
%{
#include <stdio.h>
void yyerror(const char *s);
int yylex(void);
%}
%token IDENTIFIER I_CONSTANT STRING_LITERAL F_CONSTANT
%token INT VOID FLOAT DOUBLE CHAR IF ELSE WHILE DO FOR RETURN BREAK CONTINUE
%token LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%%
/* 占位一条产生式，便于 flex/bison 语法检查；真实文法见 grammar.cpp */
subset_start: %empty ;
%%
void yyerror(const char *s) { (void)fprintf(stderr, "%s\n", s); }
