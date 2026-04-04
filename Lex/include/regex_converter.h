/**
 * regex_converter.h - Convert Lex extended RE to postfix RegexToken stream
 *
 * Pipeline: Lex RE string
 *   -> expand named definitions
 *   -> expand {n,m} repetitions
 *   -> tokenize to RegexToken infix stream
 *   -> insert explicit CONCAT operators
 *   -> shunting-yard to postfix
 */
#ifndef REGEX_CONVERTER_H
#define REGEX_CONVERTER_H

#include "common.h"

/**
 * Pre-expand all named definitions so that each definition is self-contained.
 * Must be called once before processing any rule patterns.
 */
void expand_all_definitions(LexFile& lf);

/**
 * Convert a single Lex RE pattern to a postfix RegexToken stream,
 * ready for Thompson NFA construction.
 */
std::vector<RegexToken> convert_lex_re(
    const std::string& pattern,
    const std::unordered_map<std::string, std::string>& defs);

#endif /* REGEX_CONVERTER_H */
