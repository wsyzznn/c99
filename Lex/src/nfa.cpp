/**
 * nfa.cpp - Thompson NFA construction and merging
 */
#include "nfa.h"

/* ---------- Thompson construction from postfix ---------- */

NFAFragment build_nfa_from_postfix(NFA& nfa, const std::vector<RegexToken>& postfix) {
    std::stack<NFAFragment> frags;

    for (auto& tok : postfix) {
        switch (tok.type) {

        case RegexToken::LITERAL: {
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            nfa.add_transition(s0, tok.char_code, s1);
            frags.push({s0, s1});
            break;
        }

        case RegexToken::CHAR_CLASS: {
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            for (int ch : tok.char_set) {
                nfa.add_transition(s0, ch, s1);
            }
            frags.push({s0, s1});
            break;
        }

        case RegexToken::OP_CONCAT: {
            NFAFragment b = frags.top(); frags.pop();
            NFAFragment a = frags.top(); frags.pop();
            /* merge a.accept and b.start: add epsilon from a.accept to b.start */
            nfa.add_transition(a.accept, EPSILON, b.start);
            frags.push({a.start, b.accept});
            break;
        }

        case RegexToken::OP_OR: {
            NFAFragment b = frags.top(); frags.pop();
            NFAFragment a = frags.top(); frags.pop();
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            nfa.add_transition(s0, EPSILON, a.start);
            nfa.add_transition(s0, EPSILON, b.start);
            nfa.add_transition(a.accept, EPSILON, s1);
            nfa.add_transition(b.accept, EPSILON, s1);
            frags.push({s0, s1});
            break;
        }

        case RegexToken::OP_STAR: {
            NFAFragment a = frags.top(); frags.pop();
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            nfa.add_transition(s0, EPSILON, a.start);
            nfa.add_transition(s0, EPSILON, s1);
            nfa.add_transition(a.accept, EPSILON, a.start);
            nfa.add_transition(a.accept, EPSILON, s1);
            frags.push({s0, s1});
            break;
        }

        case RegexToken::OP_PLUS: {
            /* a+ = a a* */
            NFAFragment a = frags.top(); frags.pop();
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            nfa.add_transition(s0, EPSILON, a.start);
            nfa.add_transition(a.accept, EPSILON, a.start);
            nfa.add_transition(a.accept, EPSILON, s1);
            frags.push({s0, s1});
            break;
        }

        case RegexToken::OP_QUESTION: {
            /* a? = (a | epsilon) */
            NFAFragment a = frags.top(); frags.pop();
            int s0 = nfa.add_state();
            int s1 = nfa.add_state();
            nfa.add_transition(s0, EPSILON, a.start);
            nfa.add_transition(s0, EPSILON, s1);
            nfa.add_transition(a.accept, EPSILON, s1);
            frags.push({s0, s1});
            break;
        }

        default:
            break;
        }
    }

    if (frags.empty()) {
        /* Empty pattern: just accept anything (epsilon) */
        int s0 = nfa.add_state();
        int s1 = nfa.add_state();
        nfa.add_transition(s0, EPSILON, s1);
        return {s0, s1};
    }
    return frags.top();
}

/* ---------- build combined NFA from all rules ---------- */

NFA build_combined_nfa(const LexFile& lf) {
    NFA nfa;

    /* Create new global start state */
    int global_start = nfa.add_state();
    nfa.start_state = global_start;

    for (size_t i = 0; i < lf.rules.size(); i++) {
        const LexRule& rule = lf.rules[i];

        /* Convert pattern to postfix */
        std::vector<RegexToken> postfix = convert_lex_re(rule.pattern, lf.defs);

        /* Build NFA fragment for this rule */
        NFAFragment frag = build_nfa_from_postfix(nfa, postfix);

        /* Mark accept state with rule index */
        nfa.states[frag.accept].is_accept = true;
        nfa.states[frag.accept].rule_index = rule.rule_index;

        /* Connect global start to this fragment's start via epsilon */
        nfa.add_transition(global_start, EPSILON, frag.start);
    }

    std::cerr << "[SeuLex] NFA built: " << nfa.states.size() << " states, "
              << lf.rules.size() << " rules" << std::endl;

    return nfa;
}
