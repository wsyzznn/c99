/**
 * lex_parser.h - Parser for .l (Lex input) files
 */
#ifndef LEX_PARSER_H
#define LEX_PARSER_H

#include "common.h"
#include <fstream>

/**
 * Parse a .l file into its three sections:
 *   1. Definition section (before first %%)
 *   2. Rules section (between first and second %%)
 *   3. User subroutine section (after second %%)
 */
LexFile parse_lex_file(const std::string& filename);

#endif /* LEX_PARSER_H */
