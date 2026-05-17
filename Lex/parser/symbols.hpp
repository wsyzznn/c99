#pragma once
#include <cstdint>

/* 终结符：与词法返回值一致；单字符记号用其 ASCII；$ 单独编码 */
namespace seulr {

inline constexpr int T_DOLLAR = 282; /* 不与 y.tab.h 中记号冲突 */
inline constexpr int NT_BASE = 1000;

enum NT : int {
    NT_accept = NT_BASE,
    NT_translation_unit,
    NT_external_declaration,
    NT_function_definition,
    NT_declaration,
    NT_declaration_specifiers,
    NT_type_specifier,
    NT_init_declarator_list,
    NT_init_declarator,
    NT_declarator,
    NT_direct_declarator,
    NT_parameter_list,
    NT_identifier_list,
    NT_compound_statement,
    NT_block_item_list,
    NT_block_item,
    NT_statement,
    NT_expression_statement,
    NT_expression_opt,
    NT_selection_statement,
    NT_iteration_statement,
    NT_jump_statement,
    NT_expression,
    NT_assignment_expression,
    NT_logical_or_expression,
    NT_logical_and_expression,
    NT_equality_expression,
    NT_relational_expression,
    NT_additive_expression,
    NT_multiplicative_expression,
    NT_unary_expression,
    NT_postfix_expression,
    NT_primary_expression,
    NT_argument_expression_list
};

inline constexpr int NT_NUM =
    static_cast<int>(NT_argument_expression_list) - NT_BASE + 1;

inline bool is_terminal(int s) { return s < NT_BASE; }

/* 当前词法记号 -> ACTION 列（0..511），未用槽为 -1 */
inline int term_col(int lex) {
    if (lex < 0 || lex > 511)
        return -1;
    return lex;
}

} // namespace seulr
