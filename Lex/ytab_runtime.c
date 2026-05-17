/* 全局语义值与位置（与 Flex/Bison 约定一致），单独编译单元避免重复定义 */
#include "y.tab.h"
#include "symbol.h"

YYSTYPE yylval;
YYLTYPE yylloc = {1, 0, 1, 0};
int line_num = 1;

/* lex.yy.c 中 check_type() 依赖符号表接口；无真实符号表时的占位实现 */
void symbol_table_init(void) {}

void symbol_insert(Symbol *symbol) { (void)symbol; }

Symbol *symbol_lookup(const char *name) {
    (void)name;
    return NULL;
}

void symbol_enter_scope(void) {}

void symbol_exit_scope(void) {}
