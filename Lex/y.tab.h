/**
 * 与 c99.l 首部 #include 一致；记号名、编号与 c99.y 中 %token 一致（手写维护，供词法/自写语法分析共用）。
 * 风格对齐 Flex/Bison：yylex 返回 int；属性在全局 yylval(YYSTYPE)；原文在 yytext；位置可用 yylloc(YYLTYPE)。
 */
#ifndef Y_TAB_H
#define Y_TAB_H

#include <stddef.h>

struct ASTNode;
typedef struct ASTNode ASTNode;

#ifndef YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED 1
typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#endif

#ifndef YYTOKENTYPE
#define YYTOKENTYPE
enum yytokentype {
    YYEMPTY = -2,
    YYEOF = 0,
    YYerror = 256,
    YYUNDEF = 257,
    IDENTIFIER = 258,
    STRING_LITERAL = 259,
    I_CONSTANT = 260,
    F_CONSTANT = 261,
    CHAR = 262,
    INT = 263,
    FLOAT = 264,
    DOUBLE = 265,
    VOID = 266,
    IF = 267,
    ELSE = 268,
    WHILE = 269,
    DO = 270,
    FOR = 271,
    RETURN = 272,
    BREAK = 273,
    CONTINUE = 274,
    LE_OP = 275,
    GE_OP = 276,
    EQ_OP = 277,
    NE_OP = 278,
    AND_OP = 279,
    OR_OP = 280,
    LOWER_THAN_ELSE = 281
};
#endif

#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE {
    int intval;
    double fval;
    char *str_val; /* 标识符/字符串字面量等 */
    ASTNode *ast;
    struct {
        ASTNode **items;
        int count;
    } list;
} YYSTYPE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

/* SeuLex 生成为 char yytext[]；Flex 生成为 char*，联调时与扫描器定义一致即可 */
extern char yytext[];

#ifdef __cplusplus
}
#endif

#endif /* Y_TAB_H */
