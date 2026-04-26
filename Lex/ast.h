#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 提前定义，解决递归引用 */
typedef struct ASTNode ASTNode;

typedef enum {
    AST_INT, AST_DOUBLE, AST_STRING, AST_VAR,
    AST_BINOP, AST_UNOP, AST_ASSIGN, AST_CALL,
    AST_COMPOUND_STMT, AST_EXPR_STMT, AST_IF,
    AST_WHILE, AST_DO_WHILE, AST_FOR, AST_BREAK, AST_CONTINUE, AST_RETURN,
    AST_TRANSLATION_UNIT, AST_EXTERNAL_DECL, AST_DECLARATION,
    AST_FUNC_DEF, AST_DECL_SPEC, AST_INIT_DECL, AST_FUNC_TYPE, 
    AST_PARAM_LIST, AST_PARAM
} ASTNodeType;

struct ASTNode {
    ASTNodeType type;
    int line; 
    union {
        int intval;
        double fval;
        char *str;
        char *varname;
        struct { char op; struct ASTNode *lhs, *rhs; } bin;
        struct { struct ASTNode *l, *r; } assign;
        struct { char *fname; struct ASTNode **args; int argc; } call;
        struct { struct ASTNode **list; int count; } seq;
        struct { struct ASTNode *cond, *then_s, *else_s; } sif;
        struct { struct ASTNode *cond, *body; } sw;
        struct { struct ASTNode *init, *cond, *post, *body; } sf;
        struct { struct ASTNode **specs; int scount; } ds; 
        struct { struct ASTNode *declr, *init; } id;      
        struct { struct ASTNode **specs; int scount; struct ASTNode **inits; int icount; } declaration;
        struct { char *name; struct ASTNode *ret_type; struct ASTNode *params; struct ASTNode *body; } ft;
        struct ASTNode *expr;
    };
};

/* 接口函数声明 */
ASTNode* ast_int(int val);
ASTNode* ast_double(double val);
ASTNode* ast_var(char* name);
ASTNode* ast_binop(char op, ASTNode* lhs, ASTNode* rhs);
ASTNode* ast_assign(ASTNode* l, ASTNode* r);
ASTNode* ast_call(char* name, ASTNode** arguments, int arg_count);
ASTNode* ast_if(ASTNode* cond, ASTNode* then_s, ASTNode* else_s);
ASTNode* ast_while(ASTNode* cond, ASTNode* body);
ASTNode* ast_for(ASTNode* init, ASTNode* cond, ASTNode* post, ASTNode* body);
ASTNode* ast_compound(ASTNode** list, int count);
void ast_free(ASTNode* node);

#ifdef __cplusplus
}
#endif

#endif