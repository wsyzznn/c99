/**
 * nfa.h - Thompson NFA construction and merging
 */
#ifndef NFA_H
#define NFA_H

#include "common.h"
#include "regex_converter.h"

/**
 * Build an NFA from a postfix RegexToken stream using Thompson's construction.
 * Returns an NFAFragment (start, accept) within the shared NFA.
 */
NFAFragment build_nfa_from_postfix(NFA& nfa, const std::vector<RegexToken>& postfix);

/**
 * Build and merge all rule NFAs into a single combined NFA.
 * Creates a new start state with epsilon transitions to each rule's NFA start.
 * Marks each rule's accept state with its rule_index (for priority).
 */
NFA build_combined_nfa(const LexFile& lf);

#endif /* NFA_H */
