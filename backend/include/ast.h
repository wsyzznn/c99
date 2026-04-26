#ifndef AST_H
#define AST_H

#include <stdlib.h>

/* AST 节点种类 (严格对应前端的定义) [cite: 734-740] */
typedef enum {
    /* 基本表达式 */
    AST_INT, AST_DOUBLE, AST_STRING, AST_VAR, 
    AST_BINOP, AST_UNOP, AST_ASSIGN, AST_CALL,
    /* 复合语句、块 */
    AST_COMPOUND_STMT, AST_EXPR_STMT, AST_IF,
    AST_WHILE, AST_DO_WHILE, AST_FOR, AST_BREAK, AST_CONTINUE, AST_RETURN,
    /* 类型与声明 */
    AST_TRANSLATION_UNIT, AST_EXTERNAL_DECL, AST_DECLARATION, 
    AST_FUNC_DEF, AST_DECL_SPEC, AST_INIT_DECL, AST_POINTER_TYPE, 
    AST_ARRAY_TYPE, AST_FUNC_TYPE, AST_PARAM_LIST, AST_PARAM, 
    AST_INIT_LIST, AST_TYPE_NAME
} ASTNodeType;

/* 核心 AST 节点结构 (联合体设计) [cite: 743-787] */
struct ASTNode {
    ASTNodeType type;
    int line; // 记录行号，方便报错

    union {
        /* AST_INT */
        int intval;
        /* AST_DOUBLE */
        double fval;
        /* AST_STRING */
        char *str;
        /* AST_VAR */
        char *varname;

        /* AST_BINOP, AST_UNOP */
        struct { char op; struct ASTNode *lhs, *rhs; } bin, un;
        
        /* AST_ASSIGN */
        struct { struct ASTNode *l, *r; } assign;
        
        /* AST_CALL */
        struct { char *fname; struct ASTNode **args; int argc; } call;
        
        /* AST_COMPOUND_STMT, AST_TRANSLATION_UNIT, AST_EXTERNAL_DECL, AST_INIT_LIST */
        struct { struct ASTNode **list; int count; } seq;
        
        /* AST_EXPR_STMT, AST_RETURN */
        struct ASTNode *expr;
        
        /* AST_IF */
        struct { struct ASTNode *cond, *then_s, *else_s; } sif;
        
        /* AST_WHILE */
        struct { struct ASTNode *cond, *body; } sw;
        
        /* AST_DO_WHILE */
        struct { struct ASTNode *body, *cond; } sdw;
        
        /* AST_FOR */
        struct { struct ASTNode *init, *cond, *post, *body; } sf;
        
        /* --- 以下为声明和类型相关的专用字段 --- */
        /* AST_DECL_SPEC */
        struct { struct ASTNode **specs; int scount; } ds;
        /* AST_INIT_DECL */
        struct { struct ASTNode *declr, *init; } id;
        /* AST_POINTER_TYPE */
        struct ASTNode *ptr_to;
        /* AST_ARRAY_TYPE */
        struct { struct ASTNode *base; struct ASTNode *size; } at;
        /* AST_FUNC_TYPE */
        struct { char *name; struct ASTNode *ret_type; struct ASTNode *params; struct ASTNode *body; } ft;
        /* AST_PARAM_LIST */
        struct { struct ASTNode **params; int pcount; } pl;
        /* AST_PARAM */
        struct { struct ASTNode **dspecs; int dcount; struct ASTNode *declr; } param;
        /* AST_DECLARATION */
        struct { struct ASTNode **specs; int scount; struct ASTNode **inits; int icount; } declaration;
    };
};

typedef struct ASTNode ASTNode;

/* ================== 接口函数声明 ================== */
#ifdef __cplusplus
extern "C" {
#endif

// 基础节点构造
ASTNode* ast_int(int val);
ASTNode* ast_double(double val);
ASTNode* ast_var(char* name);

// 表达式与赋值构造
ASTNode* ast_binop(char op, ASTNode* lhs, ASTNode* rhs);
ASTNode* ast_assign(ASTNode* l, ASTNode* r);
// 函数调用构造
ASTNode* ast_call(char* fname, ASTNode** args, int argc);

// 控制流构造
ASTNode* ast_if(ASTNode* cond, ASTNode* then_s, ASTNode* else_s);
ASTNode* ast_while(ASTNode* cond, ASTNode* body);
ASTNode* ast_for(ASTNode* init, ASTNode* cond, ASTNode* post, ASTNode* body);
ASTNode* ast_compound(ASTNode** list, int count);

// 内存释放
void ast_free(ASTNode* node);

// 4.15新增：打印语法树函数
void print_ast(ASTNode* node, int depth);

#ifdef __cplusplus
}
#endif

#endif // AST_H