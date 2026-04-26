#include "../../include/symbol.h"
#include <vector>
#include <unordered_map>
#include <cstring>

// 内部静态变量，不对外暴露
static std::vector<std::unordered_map<std::string, Symbol>> scope_stack;
static int current_level = 0;

static int current_offset = 0; // 【新增】：全局偏移量计数器

void symbol_table_init() {
    scope_stack.clear();
    current_level = 0;
    current_offset = 0; // 初始化
    scope_stack.push_back({}); // 全局作用域
}

void symbol_enter_scope() {
    current_level++;
    scope_stack.push_back({});
}

void symbol_exit_scope() {
    if (scope_stack.size() > 1) {
        scope_stack.pop_back();
        current_level--;
    }
}

void symbol_insert(Symbol *symbol) {
    if (scope_stack.empty()) return;
    
    // 拷贝一份符号信息存入当前作用域
    symbol->scope = current_level;
    symbol->offset = current_offset;

    // 计算当前变量需要占用多少字节
    int alloc_size = 0;
    
    if (symbol->size > 0) {
        // 如果在 semantic.cpp 里已经算好了大小（比如数组算出了 40 字节），优先用它
        alloc_size = symbol->size;
    } else {
        // 兜底逻辑：普通的基础类型
        if (symbol->c_type == TYPE_INT) alloc_size = 4;
        else if (symbol->c_type == TYPE_DOUBLE) alloc_size = 8;
        else alloc_size = 4; // 默认 4 字节
    }

    // 更新全局偏移量，并把计算好的大小存回符号表
    symbol->size = alloc_size;
    current_offset += alloc_size;
    
    // 插入当前作用域的哈希表中
    scope_stack.back()[symbol->name] = *symbol;
}

Symbol* symbol_lookup(const char *name) {
    std::string search_name(name);
    // 从内层向外层查找
    for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); ++it) {
        auto found = it->find(search_name); // 算一次哈希
        if (found != it->end()) {
            return &(found->second);        // 直接返回内存地址
        }
    }
    return nullptr;
}

Symbol* symbol_lookup_current_scope(const char *name) {
    if (scope_stack.empty()) return nullptr;
    
    auto& current_scope = scope_stack.back();
    std::string search_name(name);
    
    auto it = current_scope.find(search_name);
    if (it != current_scope.end()) {
        return &(it->second);
    }
    return nullptr;
}