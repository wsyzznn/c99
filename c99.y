%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* 声明外部接口 */
extern int yylex();
extern int yylineno;
extern char* yytext;

/* 错误处理函数 */
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}

/* 全局根节点，供语义分析和中间代码组使用 */
ASTNode *g_ast_root = NULL;
%}

/* 1. 语义值类型定义：必须与 ast.h 匹配 */
%union {
    int intval;
    double fval;
    char *str_val;
    ASTNode *ast;  /* 统一使用 ASTNode* */
    struct {       /* 专门处理列表的临时结构 */
        ASTNode **items;
        int count;
    } list;
}

/* 2. Token 声明 (需与词法组 c99.l 严格一致) */
%token <str_val> IDENTIFIER STRING_LITERAL
%token <intval> I_CONSTANT
%token <fval> F_CONSTANT
%token CHAR INT FLOAT DOUBLE VOID
%token IF ELSE WHILE DO FOR RETURN BREAK CONTINUE
%token LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP

/* 3. 非终结符类型声明 (全量覆盖 C 语言结构) */
%type <ast> primary_expression postfix_expression unary_expression 
%type <ast> cast_expression multiplicative_expression additive_expression
%type <ast> relational_expression equality_expression logical_and_expression logical_or_expression
%type <ast> assignment_expression expression
%type <ast> declaration declaration_specifiers type_specifier declarator direct_declarator
%type <ast> statement compound_statement expression_statement selection_statement 
%type <ast> iteration_statement jump_statement block_item
%type <ast> function_definition external_declaration translation_unit

/* 列表类型：对应你的 ast.h 中需要数组的地方 */
%type <list> argument_expression_list init_declarator_list block_item_list parameter_list

/* 4. 优先级处理 (解决 Shift/Reduce 冲突) */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right '='
%left OR_OP
%left AND_OP
%left EQ_OP NE_OP
%left '<' '>' LE_OP GE_OP
%left '+' '-'
%left '*' '/' '%'

%start translation_unit

%%

/* ========================== 1. 表达式逻辑 ========================== */

primary_expression
    : IDENTIFIER { $$ = ast_var($1); }
    | I_CONSTANT { $$ = ast_int($1); }
    | F_CONSTANT { $$ = ast_double($1); }
    | '(' expression ')' { $$ = $2; }
    ;

postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression '(' ')' { $$ = ast_call($1->varname, NULL, 0); }
    | postfix_expression '(' argument_expression_list ')' { $$ = ast_call($1->varname, $3.items, $3.count); }
    ;

argument_expression_list
    : assignment_expression { 
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); $$.items[0] = $1; $$.count = 1; 
    }
    | argument_expression_list ',' assignment_expression {
        $1.items = (ASTNode**)realloc($1.items, sizeof(ASTNode*) * ($1.count + 1));
        $1.items[$1.count++] = $3; $$ = $1;
    }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
    | '-' cast_expression { $$ = ast_binop('-', ast_int(0), $2); /* 简易处理负数 */ }
    ;

cast_expression
    : unary_expression { $$ = $1; }
    ;

multiplicative_expression
    : cast_expression { $$ = $1; }
    | multiplicative_expression '*' cast_expression { $$ = ast_binop('*', $1, $3); }
    | multiplicative_expression '/' cast_expression { $$ = ast_binop('/', $1, $3); }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression '+' multiplicative_expression { $$ = ast_binop('+', $1, $3); }
    | additive_expression '-' multiplicative_expression { $$ = ast_binop('-', $1, $3); }
    ;

relational_expression
    : additive_expression { $$ = $1; }
    | relational_expression '<' additive_expression { $$ = ast_binop('<', $1, $3); }
    | relational_expression LE_OP additive_expression { $$ = ast_binop('l', $1, $3); }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ_OP relational_expression { $$ = ast_binop('e', $1, $3); }
    ;

logical_and_expression
    : equality_expression { $$ = $1; }
    | logical_and_expression AND_OP equality_expression { $$ = ast_binop('&', $1, $3); }
    ;

logical_or_expression
    : logical_and_expression { $$ = $1; } /* 改成这一行 */
    | logical_or_expression OR_OP logical_and_expression { $$ = ast_binop('|', $1, $3); }
    ;

assignment_expression
    : logical_or_expression { $$ = $1; }
    | unary_expression '=' assignment_expression { $$ = ast_assign($1, $3); }
    ;

expression
    : assignment_expression { $$ = $1; }
    ;

/* ========================== 2. 声明逻辑 ========================== */

declaration
    : declaration_specifiers init_declarator_list ';' {
        /* 对应你的 AST_DECLARATION: specs, scount, inits, icount */
        $$ = (ASTNode*)malloc(sizeof(ASTNode));
        $$->type = AST_DECLARATION;
        $$->declaration.specs = $1->ds.specs;
        $$->declaration.scount = $1->ds.scount;
        $$->declaration.inits = $2.items;
        $$->declaration.icount = $2.count;
    }
    ;

declaration_specifiers
    : type_specifier { 
        ASTNode **a = (ASTNode**)malloc(sizeof(ASTNode*)); a[0] = $1;
        $$ = (ASTNode*)malloc(sizeof(ASTNode));
        $$->type = AST_DECL_SPEC;
        $$->ds.specs = a; $$->ds.scount = 1;
    }
    ;

type_specifier
    : VOID { $$ = ast_var("void"); }
    | INT  { $$ = ast_var("int"); }
    | FLOAT { $$ = ast_var("float"); }
    ;

init_declarator_list
    : declarator { 
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); 
        $$.items[0] = (ASTNode*)malloc(sizeof(ASTNode));
        $$.items[0]->type = AST_INIT_DECL;
        $$.items[0]->id.declr = $1; $$.items[0]->id.init = NULL;
        $$.count = 1; 
    }
    ;

declarator
    : direct_declarator { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER { $$ = ast_var($1); }
    | direct_declarator '(' parameter_list ')' { 
        $$ = (ASTNode*)malloc(sizeof(ASTNode));
        $$->type = AST_FUNC_TYPE;
        $$->ft.name = $1->varname;
        $$->ft.params = (ASTNode*)malloc(sizeof(ASTNode));
        $$->ft.params->type = AST_PARAM_LIST;
        $$->ft.params->pl.params = $3.items;
        $$->ft.params->pl.pcount = $3.count;
    }
    ;

parameter_list
    : /* empty */ { $$.items = NULL; $$.count = 0; }
    | IDENTIFIER { /* 简易处理参数 */
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); 
        $$.items[0] = ast_var($1); $$.count = 1;
    }
    ;

/* ========================== 3. 语句逻辑 ========================== */

statement
    : compound_statement { $$ = $1; }
    | expression_statement { $$ = $1; }
    | selection_statement { $$ = $1; }
    | iteration_statement { $$ = $1; }
    | jump_statement { $$ = $1; }
    ;

compound_statement
    : '{' '}' { $$ = ast_compound(NULL, 0); }
    | '{' block_item_list '}' { $$ = ast_compound($2.items, $2.count); }
    ;

block_item_list
    : block_item { 
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); $$.items[0] = $1; $$.count = 1; 
    }
    | block_item_list block_item {
        $1.items = (ASTNode**)realloc($1.items, sizeof(ASTNode*) * ($1.count + 1));
        $1.items[$1.count++] = $2; $$ = $1;
    }
    ;

block_item
    : declaration { $$ = $1; }
    | statement { $$ = $1; }
    ;

expression_statement
    : ';' { $$ = NULL; }
    | expression ';' { $$ = $1; }
    ;

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE { $$ = ast_if($3, $5, NULL); }
    | IF '(' expression ')' statement ELSE statement { $$ = ast_if($3, $5, $7); }
    ;

iteration_statement
    : WHILE '(' expression ')' statement { $$ = ast_while($3, $5); }
    ;

jump_statement
    : RETURN expression ';' { 
        $$ = (ASTNode*)malloc(sizeof(ASTNode));
        $$->type = AST_RETURN; $$->expr = $2;
    }
    | BREAK ';' { $$ = (ASTNode*)malloc(sizeof(ASTNode)); $$->type = AST_BREAK; }
    ;

/* ========================== 4. 根单元 ========================== */

translation_unit
    : external_declaration { 
        g_ast_root = (ASTNode*)malloc(sizeof(ASTNode)); // 必须先给它 malloc！
        g_ast_root->type = AST_TRANSLATION_UNIT;
        ASTNode **a = (ASTNode**)malloc(sizeof(ASTNode*)); 
        a[0] = $1;
        g_ast_root->seq.list = a; 
        g_ast_root->seq.count = 1;
    }

external_declaration
    : function_definition { $$ = $1; }
    | declaration { $$ = $1; }
    ;

function_definition
    : declaration_specifiers declarator compound_statement {
        $$ = $2; // 从 direct_declarator 传来的 AST_FUNC_TYPE
        $$->ft.ret_type = $1->ds.specs[0];
        $$->ft.body = $3;
    }
    ;

%%