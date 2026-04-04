/**
 * main.cpp - SeuLex entry point
 *
 * Usage: seulex <input.l> [output.c]
 *
 * Reads a .l file, builds the full NFA->DFA->minimized DFA pipeline,
 * and generates lex.yy.c.
 */
#include "lex_parser.h"
#include "regex_converter.h"
#include "nfa.h"
#include "dfa.h"
#include "code_generator.h"

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file = "lex.yy.c";

    if (argc < 2) {
        std::cerr << "Usage: seulex <input.l> [output.c]" << std::endl;
        return 1;
    }
    input_file = argv[1];
    if (argc >= 3) output_file = argv[2];

    std::cerr << "[SeuLex] Processing " << input_file << " ..." << std::endl;

    /* Step 1: Parse .l file */
    LexFile lf = parse_lex_file(input_file);
    std::cerr << "[SeuLex] Parsed: " << lf.defs.size() << " definitions, "
              << lf.rules.size() << " rules" << std::endl;

    /* Step 2: Expand named regex definitions */
    expand_all_definitions(lf);

    /* Step 3: Build combined NFA (Thompson construction for each rule, then merge) */
    NFA nfa = build_combined_nfa(lf);

    /* Step 4: Subset construction (NFA -> DFA) */
    DFA dfa = nfa_to_dfa(nfa);

    /* Step 5: Hopcroft minimization */
    DFA min_dfa = minimize_dfa(dfa);

    /* Step 6: Generate output C file */
    generate_lex_yy_c(output_file, min_dfa, lf);

    std::cerr << "[SeuLex] Done." << std::endl;
    return 0;
}
