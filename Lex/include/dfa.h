/**
 * dfa.h - Subset construction (NFA->DFA) and Hopcroft DFA minimization
 */
#ifndef DFA_H
#define DFA_H

#include "common.h"

/**
 * Convert an NFA to a DFA using the subset construction algorithm.
 * Handles epsilon-closure, and resolves accept-state conflicts by
 * choosing the rule with the lowest index (earliest in .l file).
 */
DFA nfa_to_dfa(const NFA& nfa);

/**
 * Minimize a DFA using Hopcroft's partition-refinement algorithm.
 * Accept states bound to different actions are initially in different groups.
 * Dead states (non-accept, no useful outgoing transitions) are removed.
 */
DFA minimize_dfa(const DFA& dfa);

#endif /* DFA_H */
