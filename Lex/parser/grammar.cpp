#include "grammar.hpp"
#include "symbols.hpp"
#include "../y.tab.h"

#include <cstdio>

std::vector<Production> g_prods;

static void P(int lhs, std::initializer_list<int> r) {
    g_prods.push_back({lhs, std::vector<int>(r)});
}

void grammar_init() {
    g_prods.clear();
    using namespace seulr;

    /* 0 */
    P(NT_accept, {NT_translation_unit, T_DOLLAR});
    P(NT_translation_unit, {NT_external_declaration});
    P(NT_translation_unit, {NT_translation_unit, NT_external_declaration});
    P(NT_external_declaration, {NT_function_definition});
    P(NT_external_declaration, {NT_declaration});
    P(NT_function_definition,
      {NT_declaration_specifiers, NT_declarator, NT_compound_statement});
    P(NT_declaration, {NT_declaration_specifiers, NT_init_declarator_list, ';'});
    P(NT_declaration_specifiers, {NT_type_specifier});
    P(NT_type_specifier, {VOID});
    P(NT_type_specifier, {INT});
    P(NT_type_specifier, {FLOAT});
    P(NT_type_specifier, {DOUBLE});
    P(NT_type_specifier, {CHAR});
    P(NT_init_declarator_list, {NT_init_declarator});
    P(NT_init_declarator_list, {NT_init_declarator_list, ',', NT_init_declarator});
    P(NT_init_declarator, {NT_declarator});
    P(NT_init_declarator, {NT_declarator, '=', NT_assignment_expression});
    P(NT_declarator, {NT_direct_declarator});
    P(NT_direct_declarator, {IDENTIFIER});
    P(NT_direct_declarator, {NT_direct_declarator, '(', NT_parameter_list, ')'});
    P(NT_parameter_list, {}); /* ε */
    P(NT_parameter_list, {NT_identifier_list});
    P(NT_identifier_list, {IDENTIFIER});
    P(NT_identifier_list, {NT_identifier_list, ',', IDENTIFIER});
    P(NT_compound_statement, {'{', '}'});
    P(NT_compound_statement, {'{', NT_block_item_list, '}'});
    P(NT_block_item_list, {NT_block_item});
    P(NT_block_item_list, {NT_block_item_list, NT_block_item});
    P(NT_block_item, {NT_declaration});
    P(NT_block_item, {NT_statement});
    P(NT_statement, {NT_compound_statement});
    P(NT_statement, {NT_expression_statement});
    P(NT_statement, {NT_selection_statement});
    P(NT_statement, {NT_iteration_statement});
    P(NT_statement, {NT_jump_statement});
    P(NT_expression_statement, {';'});
    P(NT_expression_statement, {NT_expression, ';'});
    P(NT_expression_opt, {});
    P(NT_expression_opt, {NT_expression});
    P(NT_selection_statement, {IF, '(', NT_expression, ')', NT_statement});
    P(NT_selection_statement, {IF, '(', NT_expression, ')', NT_statement, ELSE, NT_statement});
    P(NT_iteration_statement, {WHILE, '(', NT_expression, ')', NT_statement});
    P(NT_iteration_statement, {DO, NT_statement, WHILE, '(', NT_expression, ')', ';'});
    P(NT_iteration_statement,
      {FOR, '(', NT_expression_statement, NT_expression_statement, ')', NT_statement});
    P(NT_iteration_statement,
      {FOR, '(', NT_expression_statement, NT_expression_statement, NT_expression, ')',
       NT_statement});
    P(NT_iteration_statement,
      {FOR, '(', NT_declaration, NT_expression_opt, ';', NT_expression_opt, ')', NT_statement});
    P(NT_jump_statement, {RETURN, NT_expression, ';'});
    P(NT_jump_statement, {BREAK, ';'});
    P(NT_jump_statement, {CONTINUE, ';'});
    P(NT_expression, {NT_assignment_expression});
    P(NT_assignment_expression, {NT_logical_or_expression});
    P(NT_assignment_expression, {NT_unary_expression, '=', NT_assignment_expression});
    P(NT_logical_or_expression, {NT_logical_and_expression});
    P(NT_logical_or_expression, {NT_logical_or_expression, OR_OP, NT_logical_and_expression});
    P(NT_logical_and_expression, {NT_equality_expression});
    P(NT_logical_and_expression, {NT_logical_and_expression, AND_OP, NT_equality_expression});
    P(NT_equality_expression, {NT_relational_expression});
    P(NT_equality_expression, {NT_equality_expression, EQ_OP, NT_relational_expression});
    P(NT_equality_expression, {NT_equality_expression, NE_OP, NT_relational_expression});
    P(NT_relational_expression, {NT_additive_expression});
    P(NT_relational_expression, {NT_relational_expression, '<', NT_additive_expression});
    P(NT_relational_expression, {NT_relational_expression, '>', NT_additive_expression});
    P(NT_relational_expression, {NT_relational_expression, LE_OP, NT_additive_expression});
    P(NT_relational_expression, {NT_relational_expression, GE_OP, NT_additive_expression});
    P(NT_additive_expression, {NT_multiplicative_expression});
    P(NT_additive_expression, {NT_additive_expression, '+', NT_multiplicative_expression});
    P(NT_additive_expression, {NT_additive_expression, '-', NT_multiplicative_expression});
    P(NT_multiplicative_expression, {NT_unary_expression});
    P(NT_multiplicative_expression, {NT_multiplicative_expression, '*', NT_unary_expression});
    P(NT_multiplicative_expression, {NT_multiplicative_expression, '/', NT_unary_expression});
    P(NT_multiplicative_expression, {NT_multiplicative_expression, '%', NT_unary_expression});
    P(NT_unary_expression, {NT_postfix_expression});
    P(NT_unary_expression, {'-', NT_unary_expression});
    P(NT_postfix_expression, {NT_primary_expression});
    P(NT_postfix_expression, {NT_postfix_expression, '(', ')'});
    P(NT_postfix_expression, {NT_postfix_expression, '(', NT_argument_expression_list, ')'});
    P(NT_argument_expression_list, {NT_assignment_expression});
    P(NT_argument_expression_list, {NT_argument_expression_list, ',', NT_assignment_expression});
    P(NT_primary_expression, {IDENTIFIER});
    P(NT_primary_expression, {I_CONSTANT});
    P(NT_primary_expression, {F_CONSTANT});
    P(NT_primary_expression, {STRING_LITERAL});
    P(NT_primary_expression, {'(', NT_expression, ')'});

    std::fprintf(stderr, "[seulr] grammar: %zu productions\n", g_prods.size());
}
