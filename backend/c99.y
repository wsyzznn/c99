%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* 声明外部接口 */
extern int yylex();
extern int yylineno;
extern char* yytext;

/* 【新增】：声明后端的语义分析与代码生成接口 */
extern void semantic_analysis(ASTNode *root);
extern void generate_three_address(ASTNode *root);
extern void generate_llvm_ir(ASTNode *root);
extern void generate_c_from_ast(ASTNode* root);

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
    ASTNode *ast;  /* 统一使用 ASTNode*作为综合属性 */
    struct {       /* 专门处理列表的临时结构（传递继承/综合属性 */
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

/* 列表类型：对应 ast.h 中需要数组的地方 */
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

/* ========================== 1. 表达式逻辑 ========================== */

primary_expression
    : IDENTIFIER { 
        /* 语义规则: PrimaryExpr.node = mkleaf(id, id.entry) */
        $$ = ast_var($1); }
    | I_CONSTANT { 
        /* 语义规则: PrimaryExpr.node = mkleaf(num, num.val) */
        $$ = ast_int($1); }
    | F_CONSTANT { 
        /* 语义规则: PrimaryExpr.node = mkleaf(num, num.fval) */
        $$ = ast_double($1); }
    | '(' expression ')' { 
        /* 语义规则: PrimaryExpr.node = Expr.node */
        $$ = $2; }
    ;

postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression '(' ')' { 
        /* 语义规则: PostfixExpr.node = mkcall(PostfixExpr1.name, null) */
        $$ = ast_call($1->varname, NULL, 0); }
    | postfix_expression '(' argument_expression_list ')' { 
        /* 语义规则: PostfixExpr.node = mkcall(PostfixExpr1.name, ArgList.items) */
        $$ = ast_call($1->varname, $3.items, $3.count); }
    ;

argument_expression_list
    : assignment_expression { 
        /* * 语义规则:
         * ArgList.items = alloc(1)
         * ArgList.items[0] = AssignExpr.node
         * ArgList.count = 1
         */
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); $$.items[0] = $1; $$.count = 1; 
    }
    | argument_expression_list ',' assignment_expression {
        /* * 语义规则:
         * ArgList.items = realloc(ArgList1.items, ArgList1.count + 1)
         * ArgList.items[ArgList1.count] = AssignExpr.node
         * ArgList.count = ArgList1.count + 1
         */
        $1.items = (ASTNode**)realloc($1.items, sizeof(ASTNode*) * ($1.count + 1));
        $1.items[$1.count++] = $3; $$ = $1;
    }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
    | '-' cast_expression { 
        /* 语义规则: UnaryExpr.node = mknode('-', 0, CastExpr.node) */
        $$ = ast_binop('-', ast_int(0), $2); /* 简易处理负数 */ }
    ;

cast_expression
    : unary_expression { $$ = $1; }
    ;

multiplicative_expression
    : cast_expression { $$ = $1; }
    | multiplicative_expression '*' cast_expression { 
        /* 语义规则: Expr.node = mknode('*', Expr1.node, CastExpr.node) */
        $$ = ast_binop('*', $1, $3); }
    | multiplicative_expression '/' cast_expression { 
        /* 语义规则: Expr.node = mknode('/', Expr1.node, CastExpr.node) */
        $$ = ast_binop('/', $1, $3); }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression '+' multiplicative_expression { 
        /* 【translation scheme】：在匹配加法后，将子节点属性综合到父节点 */
        /* 语义规则: Expr.node = mknode('+', Expr1.node, Expr2.node) */
        $$ = ast_binop('+', $1, $3); }
    | additive_expression '-' multiplicative_expression { 
        /* 语义规则: Expr.node = mknode('-', Expr1.node, Expr2.node) */
        $$ = ast_binop('-', $1, $3); }
    ;

relational_expression
    : additive_expression { $$ = $1; }
    | relational_expression '<' additive_expression { 
        /* 语义规则: Expr.node = mknode('<', Expr1.node, Expr2.node) */
        $$ = ast_binop('<', $1, $3); }
    | relational_expression LE_OP additive_expression { 
        /* 语义规则: Expr.node = mknode('<=', Expr1.node, Expr2.node) */
        $$ = ast_binop('l', $1, $3); }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ_OP relational_expression { 
        /* 语义规则: Expr.node = mknode('==', Expr1.node, Expr2.node) */
        $$ = ast_binop('e', $1, $3); }
    ;

logical_and_expression
    : equality_expression { $$ = $1; }
    | logical_and_expression AND_OP equality_expression { 
        /* 语义规则: Expr.node = mknode('&&', Expr1.node, Expr2.node) */
        $$ = ast_binop('&', $1, $3); }
    ;

logical_or_expression
    : logical_and_expression { $$ = $1; } /* 改成这一行 */
    | logical_or_expression OR_OP logical_and_expression { 
        /* 语义规则: Expr.node = mknode('||', Expr1.node, Expr2.node) */
        $$ = ast_binop('|', $1, $3); }
    ;

assignment_expression
    : logical_or_expression { $$ = $1; }
    | unary_expression '=' assignment_expression { 
        /* 语义规则: AssignExpr.node = mkassign(UnaryExpr.node, AssignExpr1.node) */
        $$ = ast_assign($1, $3); }
    ;

expression
    : assignment_expression { $$ = $1; }
    ;

/* ========================== 2. 声明逻辑 ========================== */

declaration
    : declaration_specifiers init_declarator_list ';' {
        /* 【语义规则】：组装声明节点的综合属性：specs, scount, inits, icount */
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
        /* 语义规则: DeclSpecs.type = TypeSpec.node */
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
    | DOUBLE { $$ = ast_var((char*)"double"); }/*【新增】double */
    ;

init_declarator_list
    : declarator { 
        /* * 语义规则:
         * List.items = alloc(1)
         * List.items[0].declr = Declarator.node
         * List.items[0].init = null
         */
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); 
        $$.items[0] = (ASTNode*)malloc(sizeof(ASTNode));
        $$.items[0]->type = AST_INIT_DECL;
        $$.items[0]->id.declr = $1; $$.items[0]->id.init = NULL;
        $$.count = 1; 
    }
    /* 【修复】：新增逗号支持，允许 int a, b; 这种多变量声明 */
    | init_declarator_list ',' declarator {
        /* 语义规则: List.items = append(List1.items, Declarator.node) */
        $1.items = (ASTNode**)realloc($1.items, sizeof(ASTNode*) * ($1.count + 1));
        ASTNode *new_decl = (ASTNode*)malloc(sizeof(ASTNode));
        new_decl->type = AST_INIT_DECL;
        new_decl->id.declr = $3; new_decl->id.init = NULL;
        $1.items[$1.count++] = new_decl; 
        $$ = $1;
    }
    ;

declarator
    : direct_declarator { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER { 
        /* 语义规则: Decl.node = mkleaf(id.lexeme) */
        $$ = ast_var($1); 
    }
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
    : /* empty */ { 
        /* 语义规则: ParamList.count = 0 */
        $$.items = NULL; $$.count = 0; 
    }
    | IDENTIFIER { /* 简易处理参数 */
    /* 语义规则: ParamList.items[0] = mkleaf(id.lexeme); ParamList.count = 1 */
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
    : '{' '}' { 
        /* 语义规则: CompStmt.node = mkblock(null) */
        $$ = ast_compound(NULL, 0); 
    }
    | '{' block_item_list '}' { 
        /* 语义规则: CompStmt.node = mkblock(BlockItemList.items) */
        $$ = ast_compound($2.items, $2.count); 
    }
    ;

block_item_list
    : block_item { 
        /* 语义规则: BlockList.items = append(null, BlockItem.node) */
        $$.items = (ASTNode**)malloc(sizeof(ASTNode*)); $$.items[0] = $1; $$.count = 1; 
    }
    | block_item_list block_item {
        /* 语义规则: BlockList.items = append(BlockList1.items, BlockItem.node) */
        $1.items = (ASTNode**)realloc($1.items, sizeof(ASTNode*) * ($1.count + 1));
        $1.items[$1.count++] = $2; $$ = $1;
    }
    ;

block_item
    : declaration { $$ = $1; }
    | statement { $$ = $1; }
    ;

expression_statement
    : ';' { 
        /* 语义规则: ExprStmt.node = null */
        $$ = NULL; 
    }
    | expression ';' { 
        /* 语义规则: ExprStmt.node = Expr.node */
        $$ = $1; 
    }
    ;

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE { 
        /* * 语义规则:
         * S.node = mkif(Expr.node, Stmt.node, null)
         */
        $$ = ast_if($3, $5, NULL); 
    }
    | IF '(' expression ')' statement ELSE statement { 
        /* * 语义规则 (对应龙书条件跳转结构):
         * S.node = mkif(Expr.node, Stmt1.node, Stmt2.node)
         */
        $$ = ast_if($3, $5, $7); 
    }
    ;

iteration_statement
    : WHILE '(' expression ')' statement { 
        /* * 语义规则 (对应龙书循环流图):
         * S.node = mkwhile(Expr.node, Stmt.node)
         */
        $$ = ast_while($3, $5); 
    }
    ;

jump_statement
    : RETURN expression ';' { 
        /* 语义规则: ReturnStmt.node = mkreturn(Expr.node) */
        $$ = (ASTNode*)malloc(sizeof(ASTNode));
        $$->type = AST_RETURN; $$->expr = $2;
    }
    | BREAK ';' { 
        /* 语义规则: BreakStmt.node = mkbreak() */
        $$ = (ASTNode*)malloc(sizeof(ASTNode)); $$->type = AST_BREAK; 
        }
    ;

/* ========================== 4. 根单元 ========================== */

translation_unit
    : external_declaration { 
        /* * 语义规则 (初始化全局序列节点):
         * TransUnit.node = new_Seq_Node()
         * TransUnit.node.list[0] = ExtDecl.node
         */
        g_ast_root = (ASTNode*)malloc(sizeof(ASTNode)); // 必须先给它 malloc！
        g_ast_root->type = AST_TRANSLATION_UNIT;
        ASTNode **a = (ASTNode**)malloc(sizeof(ASTNode*)); 
        a[0] = $1;
        g_ast_root->seq.list = a; 
        g_ast_root->seq.count = 1;
    }
    /* 【核心修复】：加上这一段，编译器才能解析超过一行的函数和全局变量！ */
    | translation_unit external_declaration {
        /* * 语义规则 (扩展全局序列节点):
         * TransUnit.node.list = append(TransUnit1.node.list, ExtDecl.node)
         */
        int old_count = g_ast_root->seq.count;
        g_ast_root->seq.list = (ASTNode**)realloc(g_ast_root->seq.list, sizeof(ASTNode*) * (old_count + 1));
        g_ast_root->seq.list[old_count] = $2;
        g_ast_root->seq.count++;
        $$ = g_ast_root;
    }

external_declaration
    : function_definition { $$ = $1; }
    | declaration { $$ = $1; }
    ;

function_definition
    : declaration_specifiers declarator compound_statement {
        /* * 语义规则 (绑定函数体):
         * FuncDef.node = Declarator.node
         * FuncDef.node.return_type = DeclSpecs.type
         * FuncDef.node.body = CompStmt.node
         */
        $$ = $2; // 从 direct_declarator 传来的 AST_FUNC_TYPE
        $$->ft.ret_type = $1->ds.specs[0];
        $$->ft.body = $3;
    }
    ;

%%