%{
#include "ast.h"
%}

%union {
    int intval;
    double fval;
    char *str_val;
    ASTNode *ast;
    struct {
        ASTNode **items;
        int count;
    } list;
}

%token <str_val> IDENTIFIER STRING_LITERAL
%token <intval> I_CONSTANT
%token <fval> F_CONSTANT
%token CHAR INT FLOAT DOUBLE VOID
%token IF ELSE WHILE DO FOR RETURN BREAK CONTINUE
%token LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token LOWER_THAN_ELSE

%start translation_unit

%%

/* 实际产生式在自写 LR(1) 的 grammar.cpp / seu_lr1_parser.cpp 等处实现；此处仅占位以满足 .y 文件结构 */
translation_unit: /* empty */ ;

%%

void yyerror(const char *s) { (void)s; }
