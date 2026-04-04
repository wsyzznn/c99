%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c99.tab.hpp"
#include "symbol.h"

int column = 0;
int line_num = 1;

static void count(void);
static void comment(void);
static int check_type(void);
%}

O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\f]

%%

"/*"                                { comment(); }
"//".*                              { count(); }

"char"                              { count(); return CHAR; }
"int"                               { count(); return INT; }
"float"                             { count(); return FLOAT; }
"double"                            { count(); return DOUBLE; }
"void"                              { count(); return VOID; }
"if"                                { count(); return IF; }
"else"                              { count(); return ELSE; }
"while"                             { count(); return WHILE; }
"do"                                { count(); return DO; }
"for"                               { count(); return FOR; }
"return"                            { count(); return RETURN; }
"break"                             { count(); return BREAK; }
"continue"                          { count(); return CONTINUE; }

{L}{A}*                              { count(); yylval.str_val = strdup(yytext); return check_type(); }

{HP}{H}+{IS}?                        { count(); yylval.intval = (int)strtoll(yytext, NULL, 0); return I_CONSTANT; }
{NZ}{D}*{IS}?                        { count(); yylval.intval = (int)strtoll(yytext, NULL, 10); return I_CONSTANT; }
"0"{O}*{IS}?                        { count(); yylval.intval = (int)strtoll(yytext, NULL, 0); return I_CONSTANT; }
{CP}?\'([^'\\\n]|{ES})+\'          { count(); yylval.intval = (int)yytext[1]; return I_CONSTANT; }

{D}+{E}{FS}?                         { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?                { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }
{D}+"."{E}?{FS}?                    { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }
{HP}{H}+{P}{FS}?                     { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?             { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?                 { count(); yylval.fval = strtod(yytext, NULL); return F_CONSTANT; }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+ { count(); yylval.str_val = strdup(yytext); return STRING_LITERAL; }

"<="                                { count(); return LE_OP; }
">="                                { count(); return GE_OP; }
"=="                                { count(); return EQ_OP; }
"!="                                { count(); return NE_OP; }
"&&"                                { count(); return AND_OP; }
"||"                                { count(); return OR_OP; }

";"                                 { count(); return ';'; }
"{"                                 { count(); return '{'; }
"}"                                 { count(); return '}'; }
","                                 { count(); return ','; }
":"                                 { count(); return ':'; }
"="                                 { count(); return '='; }
"("                                 { count(); return '('; }
")"                                 { count(); return ')'; }
"["                                 { count(); return '['; }
"]"                                 { count(); return ']'; }
"."                                 { count(); return '.'; }
"&"                                 { count(); return '&'; }
"!"                                 { count(); return '!'; }
"~"                                 { count(); return '~'; }
"-"                                 { count(); return '-'; }
"+"                                 { count(); return '+'; }
"*"                                 { count(); return '*'; }
"/"                                 { count(); return '/'; }
"%"                                 { count(); return '%'; }
"<"                                 { count(); return '<'; }
">"                                 { count(); return '>'; }
"^"                                 { count(); return '^'; }
"|"                                 { count(); return '|'; }
"?"                                 { count(); return '?'; }

{WS}+                                { count(); }
\n+                                  { count(); }
.                                    { count(); }

%%

int yywrap(void) {
    return 1;
}

static void comment(void) {
    int c;

    while ((c = input()) != 0) {
        if (c == '\n') {
            line_num++;
            column = 0;
            continue;
        }

        if (c == '\t') {
            column += 8 - (column % 8);
        } else {
            column++;
        }

        if (c == '*') {
            while ((c = input()) == '*') {
                column++;
            }
            if (c == '/') {
                column++;
                return;
            }
            if (c == 0) {
                break;
            }
            if (c == '\n') {
                line_num++;
                column = 0;
            } else if (c == '\t') {
                column += 8 - (column % 8);
            } else {
                column++;
            }
        }
    }
}

static void count(void) {
    int i;

    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            line_num++;
            column = 0;
        } else if (yytext[i] == '\t') {
            column += 8 - (column % 8);
        } else {
            column++;
        }
    }
}

static int check_type(void) {
    Symbol *symbol = symbol_lookup(yytext);

#ifdef TYPE_NAME
    if (symbol != NULL && symbol->type == TYPE_NAME) {
        return TYPE_NAME;
    }
#endif

    if (symbol != NULL && symbol->type > 0) {
        return symbol->type;
    }

    return IDENTIFIER;
}
