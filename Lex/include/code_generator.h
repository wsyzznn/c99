/**
 * code_generator.h - Generate lex.yy.c from minimized DFA
 */
#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "common.h"

/**
 * Generate lex.yy.c from a minimized DFA and the parsed .l file.
 * The generated code contains:
 *   - User preamble code (from %{ ... %})
 *   - State transition table
 *   - Accept state table (mapping state -> action)
 *   - yylex() driver with longest-match + backtracking
 *   - Global variables: yytext, yyleng, yyin, yyout, yylval
 *   - User subroutine code
 */
void generate_lex_yy_c(
    const std::string& output_path,
    const DFA& dfa,
    const LexFile& lf);

#endif /* CODE_GENERATOR_H */
