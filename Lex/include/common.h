/**
 * common.h - Common type definitions for SeuLex
 */
#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <queue>
#include <stack>
#include <climits>

/* Epsilon transition marker (not a valid input character) */
static const int EPSILON = -1;

/* Number of possible input byte values (0-255) */
static const int NUM_CHARS = 256;

/* ========== Lex File Structures ========== */

struct LexRule {
    std::string pattern;
    std::string action;
    int rule_index;
};

struct LexFile {
    std::string definition_code;
    std::unordered_map<std::string, std::string> defs;
    std::vector<std::string> def_order;
    std::vector<LexRule> rules;
    std::string user_code;
};

/* ========== Regex Token (infix/postfix intermediate representation) ========== */

struct RegexToken {
    enum Type {
        LITERAL,       /* Single character to match */
        CHAR_CLASS,    /* A set of characters to match (from [...] or .) */
        OP_STAR,       /* Kleene star * */
        OP_PLUS,       /* Positive closure + */
        OP_QUESTION,   /* Optional ? */
        OP_OR,         /* Alternation | */
        OP_CONCAT,     /* Concatenation (explicitly inserted) */
        LPAREN,        /* ( */
        RPAREN         /* ) */
    };
    Type type;
    int char_code;              /* For LITERAL: the byte value (0-255) */
    std::set<int> char_set;     /* For CHAR_CLASS: set of matching byte values */

    RegexToken() : type(LITERAL), char_code(0) {}

    static RegexToken literal(int ch) {
        RegexToken t; t.type = LITERAL; t.char_code = ch; return t;
    }
    static RegexToken charClass(const std::set<int>& s) {
        RegexToken t; t.type = CHAR_CLASS; t.char_set = s; return t;
    }
    static RegexToken op(Type ty) {
        RegexToken t; t.type = ty; return t;
    }
};

/* ========== NFA Structures ========== */

struct NFAState {
    int id;
    /* Transitions: key = input char (0-255) or EPSILON (-1), value = target state id */
    std::unordered_multimap<int, int> transitions;
    bool is_accept = false;
    int rule_index = -1;
};

struct NFAFragment {
    int start;
    int accept;
};

struct NFA {
    std::vector<NFAState> states;
    int start_state = 0;

    int add_state() {
        int id = (int)states.size();
        states.emplace_back();
        states.back().id = id;
        return id;
    }

    void add_transition(int from, int input, int to) {
        states[from].transitions.insert({input, to});
    }
};

/* ========== DFA Structures ========== */

struct DFAState {
    int id;
    std::set<int> nfa_states;
    bool is_accept = false;
    int rule_index = -1;
    std::map<int, int> transitions; /* char (0-255) -> target DFA state id */
};

struct DFA {
    std::vector<DFAState> states;
    int start_state = 0;
    std::set<int> alphabet;
};

#endif /* COMMON_H */
