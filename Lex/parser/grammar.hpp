#pragma once
#include <vector>

struct Production {
    int lhs{};
    std::vector<int> rhs; /* 终结符用词法返回值；非终结符 >= NT_BASE；空产生式 rhs 为空 */
};

extern std::vector<Production> g_prods;

/** 填充 g_prods（与 c99.y 子集一致） */
void grammar_init();
