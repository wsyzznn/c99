#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <string>

// 编译器支持的基础数据类型
typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_VOID,
    TYPE_ERROR // 用于标记语义错误，防止连锁报错
} CType;

/* 符号结构  */
typedef struct Symbol {
    std::string name;
    int type;
    CType c_type; // 存储真实的数据类型
    int scope;  
    int offset;
    int size;
} Symbol;

/* 符号表接口   */
void symbol_table_init();
void symbol_insert(Symbol *symbol);
Symbol* symbol_lookup(const char *name);
void symbol_enter_scope();
void symbol_exit_scope();

Symbol* symbol_lookup_current_scope(const char *name);


#endif