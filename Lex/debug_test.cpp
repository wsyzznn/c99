/**
 * debug_test.cpp - Quick debug to check pattern parsing
 */
#include "lex_parser.h"
#include "regex_converter.h"
#include "nfa.h"
#include "dfa.h"

int main() {
    LexFile lf = parse_lex_file("c99.l");
    
    std::cout << "=== Definitions (" << lf.defs.size() << ") ===" << std::endl;
    for (auto& name : lf.def_order) {
        std::cout << "  " << name << " = " << lf.defs[name] << std::endl;
    }
    
    std::cout << "\n=== Rules (" << lf.rules.size() << ") ===" << std::endl;
    for (size_t i = 0; i < lf.rules.size(); i++) {
        if (lf.rules[i].action.find("STRING_LITERAL") != std::string::npos ||
            (i >= 38 && i <= 55)) {
            std::cout << "  [" << lf.rules[i].rule_index << "] pattern=|" 
                      << lf.rules[i].pattern << "| action=|" 
                      << lf.rules[i].action << "|" << std::endl;
        }
    }
    
    std::cout << "\n=== Expanding definitions ===" << std::endl;
    expand_all_definitions(lf);
    for (auto& name : lf.def_order) {
        std::cout << "  " << name << " => " << lf.defs[name] << std::endl;
    }
    
    std::cout << "\n=== Converting first few rules ===" << std::endl;
    for (size_t i = 0; i < lf.rules.size() && i < 10; i++) {
        std::cout << "  Rule " << i << ": pattern=|" << lf.rules[i].pattern << "|" << std::endl;
        auto postfix = convert_lex_re(lf.rules[i].pattern, lf.defs);
        std::cout << "    postfix length=" << postfix.size() << std::endl;
    }

    std::cout << "\n=== Building NFA ===" << std::endl;
    NFA nfa = build_combined_nfa(lf);
    std::cout << "NFA states: " << nfa.states.size() << std::endl;
    
    /* Check if there are any non-epsilon transitions */
    int eps_count = 0, char_count = 0;
    for (auto& st : nfa.states) {
        for (auto& tr : st.transitions) {
            if (tr.first == EPSILON) eps_count++;
            else char_count++;
        }
    }
    std::cout << "Epsilon transitions: " << eps_count << std::endl;
    std::cout << "Character transitions: " << char_count << std::endl;
    
    /* Check accept states in NFA */
    int accept_count = 0;
    for (auto& st : nfa.states) {
        if (st.is_accept) accept_count++;
    }
    std::cout << "NFA accept states: " << accept_count << std::endl;
    
    /* Manual epsilon closure of start state */
    std::set<int> start_closure;
    start_closure.insert(nfa.start_state);
    {
        std::stack<int> work;
        work.push(nfa.start_state);
        while (!work.empty()) {
            int cur = work.top(); work.pop();
            auto range = nfa.states[cur].transitions.equal_range(EPSILON);
            for (auto it = range.first; it != range.second; ++it) {
                if (start_closure.insert(it->second).second) {
                    work.push(it->second);
                }
            }
        }
    }
    std::cout << "Epsilon closure of start: " << start_closure.size() << " states" << std::endl;
    
    /* How many accept states are in start closure? */
    int start_accept = 0;
    for (int s : start_closure) {
        if (nfa.states[s].is_accept) start_accept++;
    }
    std::cout << "Accept states in start closure: " << start_accept << std::endl;
    
    /* Check outgoing non-epsilon transitions from start closure */
    std::set<int> chars_from_start;
    for (int s : start_closure) {
        for (auto& tr : nfa.states[s].transitions) {
            if (tr.first != EPSILON) chars_from_start.insert(tr.first);
        }
    }
    std::cout << "Chars from start closure: " << chars_from_start.size() << std::endl;
    
    /* Try a specific move: 'a' from start closure */
    std::set<int> move_a;
    for (int s : start_closure) {
        auto range = nfa.states[s].transitions.equal_range('a');
        for (auto it = range.first; it != range.second; ++it) {
            move_a.insert(it->second);
        }
    }
    std::cout << "Move on 'a' from start: " << move_a.size() << " states" << std::endl;
    
    /* epsilon close that */
    {
        std::stack<int> work;
        for (int s : move_a) work.push(s);
        std::set<int> closure = move_a;
        while (!work.empty()) {
            int cur = work.top(); work.pop();
            auto range = nfa.states[cur].transitions.equal_range(EPSILON);
            for (auto it = range.first; it != range.second; ++it) {
                if (closure.insert(it->second).second) {
                    work.push(it->second);
                }
            }
        }
        std::cout << "Closure after 'a': " << closure.size() << " states" << std::endl;
    }

    std::cout << "\n=== Building DFA ===" << std::endl;
    DFA dfa = nfa_to_dfa(nfa);
    std::cout << "DFA states: " << dfa.states.size() << std::endl;
    std::cout << "Alphabet size: " << dfa.alphabet.size() << std::endl;
    
    /* Show DFA state details */
    for (size_t i = 0; i < dfa.states.size() && i < 5; i++) {
        std::cout << "DFA[" << i << "]: accept=" << dfa.states[i].is_accept
                  << " rule=" << dfa.states[i].rule_index
                  << " nfa_states=" << dfa.states[i].nfa_states.size()
                  << " transitions=" << dfa.states[i].transitions.size() << std::endl;
    }
    
    /* Debug string literal rule */
    std::cout << "\n=== String literal rule debug ===" << std::endl;
    for (auto& rule : lf.rules) {
        if (rule.pattern.find("STRING_LITERAL") != std::string::npos ||
            rule.pattern.find("\\\"") != std::string::npos ||
            rule.pattern.find("SP") != std::string::npos) {
            std::cout << "Rule " << rule.rule_index << ": |" << rule.pattern << "|" << std::endl;
            
            /* Expand defs manually */
            std::string expanded = rule.pattern;
            // try to convert
            auto postfix = convert_lex_re(rule.pattern, lf.defs);
            std::cout << "  postfix len=" << postfix.size() << std::endl;
        }
    }

    /* Check ES definition after expansion */
    std::cout << "\nES expanded: " << lf.defs["ES"] << std::endl;
    std::cout << "SP expanded: " << lf.defs["SP"] << std::endl;
    std::cout << "WS expanded: " << lf.defs["WS"] << std::endl;
    
    /* Test simple string pattern */
    std::string test_pat = "\"hello\"";
    std::cout << "\nTest pattern: |" << test_pat << "|" << std::endl;
    auto postfix2 = convert_lex_re(test_pat, lf.defs);
    std::cout << "  postfix len=" << postfix2.size() << std::endl;
    for (auto& t : postfix2) {
        if (t.type == RegexToken::LITERAL) {
            std::cout << "  LIT(" << t.char_code << "='" << (char)t.char_code << "') ";
        } else if (t.type == RegexToken::CHAR_CLASS) {
            std::cout << "  CLASS(sz=" << t.char_set.size() << ") ";
        } else {
            std::cout << "  OP(" << (int)t.type << ") ";
        }
    }
    std::cout << std::endl;
    
    return 0;
}
