#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

/* 1. 符号节点结构体 */
typedef struct Symbol {
    char *name;   /* 符号名称，例如 "size_t" 或 "my_variable" */
    int type;     /* 符号类型，建议后续与 c99.tab.hpp 中的 Token ID 对应，比如 TYPE_NAME */
    int scope;    /* 作用域层级 */
} Symbol;

/* 2. 符号表核心操作接口 */
void symbol_table_init();
void symbol_insert(Symbol *symbol);
Symbol* symbol_lookup(const char *name);

/* 3. 作用域管理接口 */
void symbol_enter_scope();
void symbol_exit_scope();

#endif /* COMPILER_SYMBOL_H */