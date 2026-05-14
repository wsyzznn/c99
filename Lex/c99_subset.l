/* c99_subset.l — 中间代码子集对应的词法规范（与全集 c99.l 对齐记号；可删减规则以缩小子集） */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "symbol.h"
int column = 0;
extern int line_num;
#ifdef __cplusplus
extern "C" {
#endif
static void count(void);
#ifdef __cplusplus
}
#endif
%}
%option noyywrap
%option yylineno
%%
/* 子集：保留关键字、标识符、整数、运算符与分隔符；其余可逐步删除 */
"int"     { count(); return INT; }
"void"    { count(); return VOID; }
"if"      { count(); return IF; }
"else"    { count(); return ELSE; }
"while"   { count(); return WHILE; }
"return"  { count(); return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]* { count(); yylval.str_val = strdup(yytext); return IDENTIFIER; }
[0-9]+    { count(); yylval.intval = atoi(yytext); return I_CONSTANT; }
";"       { count(); return ';'; }
"{"       { count(); return '{'; }
"}"       { count(); return '}'; }
"("       { count(); return '('; }
")"       { count(); return ')'; }
"="       { count(); return '='; }
"+"       { count(); return '+'; }
"-"       { count(); return '-'; }
"*"       { count(); return '*'; }
"<"       { count(); return '<'; }
">"       { count(); return '>'; }
[ \t\r]+  { count(); }
\n        { count(); line_num++; column = 0; }
%%
void count(void) { (void)column; }
