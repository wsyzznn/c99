/**
 * regex_converter.cpp
 *
 * Converts a Lex-style extended regular expression into a postfix
 * stream of RegexToken (ready for Thompson NFA construction).
 *
 * Steps:
 *   1. Expand named definitions  {name} -> (expanded_def)
 *   2. Tokenize:  handle  "...", [...], \x escapes, ., ?, +, *, |, (, )
 *   3. Insert explicit concatenation operators between adjacent tokens
 *   4. Shunting-yard infix -> postfix
 */
#include "regex_converter.h"

/* ---------- helper: resolve a C-style escape char ---------- */
static int resolve_escape(char c) {
    switch (c) {
        case 'a':  return '\a';
        case 'b':  return '\b';
        case 'f':  return '\f';
        case 'n':  return '\n';
        case 'r':  return '\r';
        case 't':  return '\t';
        case 'v':  return '\v';
        case '0':  return '\0';
        default:   return (unsigned char)c; /* e.g. \\, \", \', \?, \/, etc. */
    }
}

/* ---------- expand named definitions inside a string ---------- */

static std::string expand_defs(
    const std::string& s,
    const std::unordered_map<std::string, std::string>& defs)
{
    std::string out;
    out.reserve(s.size() * 2);
    size_t i = 0;
    bool in_quotes = false;
    bool in_brackets = false;

    while (i < s.size()) {
        char c = s[i];

        /* track state */
        if (c == '\\' && !in_brackets) {
            out += c;
            if (i + 1 < s.size()) { out += s[i + 1]; i += 2; }
            else i++;
            continue;
        }
        if (c == '"' && !in_brackets) {
            in_quotes = !in_quotes;
            out += c;
            i++;
            continue;
        }
        if (in_quotes) { out += c; i++; continue; }
        if (c == '[' && !in_brackets) { in_brackets = true; out += c; i++; continue; }
        if (c == ']' && in_brackets) { in_brackets = false; out += c; i++; continue; }
        if (in_brackets) { out += c; i++; continue; }

        /* {name} */
        if (c == '{') {
            size_t close = s.find('}', i + 1);
            if (close != std::string::npos) {
                std::string name = s.substr(i + 1, close - i - 1);

                /* Check for repetition syntax like {1,3} or {3} */
                bool is_rep = true;
                for (char ch : name) {
                    if (!isdigit(ch) && ch != ',') { is_rep = false; break; }
                }
                if (is_rep) {
                    out += s.substr(i, close - i + 1);
                    i = close + 1;
                    continue;
                }

                auto it = defs.find(name);
                if (it != defs.end()) {
                    out += '(';
                    out += it->second;
                    out += ')';
                    i = close + 1;
                    continue;
                }
            }
            out += c;
            i++;
            continue;
        }

        out += c;
        i++;
    }
    return out;
}

void expand_all_definitions(LexFile& lf) {
    /* Must expand in order so earlier defs are already resolved for later ones */
    for (auto& name : lf.def_order) {
        lf.defs[name] = expand_defs(lf.defs[name], lf.defs);
    }
}

/* ---------- build character set from [...] ---------- */

static std::set<int> parse_char_class(const std::string& s, size_t& pos) {
    /* pos points at '[' */
    pos++; /* skip '[' */
    bool negate = false;
    if (pos < s.size() && s[pos] == '^') {
        negate = true;
        pos++;
    }
    std::set<int> chars;

    /* Helper: read one char (may be \x escape) and return its value */
    auto read_one = [&]() -> int {
        if (pos >= s.size()) return -1;
        if (s[pos] == '\\' && pos + 1 < s.size()) {
            char c2 = s[pos + 1];
            pos += 2;
            if (c2 == 'x' || c2 == 'X') {
                /* hex: \xNN */
                std::string hex;
                while (pos < s.size() && isxdigit(s[pos])) {
                    hex += s[pos]; pos++;
                }
                return (int)strtol(hex.c_str(), nullptr, 16);
            }
            if (c2 >= '0' && c2 <= '7') {
                /* octal */
                std::string oct;
                oct += c2;
                for (int cnt = 0; cnt < 2 && pos < s.size() && s[pos] >= '0' && s[pos] <= '7'; cnt++) {
                    oct += s[pos]; pos++;
                }
                return (int)strtol(oct.c_str(), nullptr, 8);
            }
            return resolve_escape(c2);
        }
        char c = s[pos]; pos++;
        return (unsigned char)c;
    };

    while (pos < s.size() && s[pos] != ']') {
        int c1 = read_one();
        if (c1 < 0) break;
        /* range a-b */
        if (pos < s.size() && s[pos] == '-' && pos + 1 < s.size() && s[pos + 1] != ']') {
            pos++; /* skip '-' */
            int c2 = read_one();
            if (c2 < 0) { chars.insert(c1); break; }
            for (int c = c1; c <= c2; c++) chars.insert(c);
        } else {
            chars.insert(c1);
        }
    }
    if (pos < s.size() && s[pos] == ']') pos++; /* skip ']' */

    if (negate) {
        std::set<int> neg;
        for (int c = 1; c < 256; c++) { /* exclude 0 */
            if (chars.find(c) == chars.end()) neg.insert(c);
        }
        return neg;
    }
    return chars;
}

/* ---------- dot set: matches any char except \n ---------- */
static std::set<int> dot_set() {
    std::set<int> s;
    for (int c = 1; c < 256; c++) {
        if (c != '\n') s.insert(c);
    }
    return s;
}

/* ---------- tokenize: lex RE string -> infix RegexToken stream ---------- */

static bool is_postfix_op(RegexToken::Type t) {
    return t == RegexToken::OP_STAR ||
           t == RegexToken::OP_PLUS ||
           t == RegexToken::OP_QUESTION;
}

static bool is_operand_end(RegexToken::Type t) {
    return t == RegexToken::LITERAL ||
           t == RegexToken::CHAR_CLASS ||
           t == RegexToken::RPAREN ||
           is_postfix_op(t);
}

static bool is_operand_start(RegexToken::Type t) {
    return t == RegexToken::LITERAL ||
           t == RegexToken::CHAR_CLASS ||
           t == RegexToken::LPAREN;
}

static std::vector<RegexToken> tokenize(const std::string& s) {
    std::vector<RegexToken> tokens;
    size_t i = 0;

    while (i < s.size()) {
        char c = s[i];

        /* --- double-quoted literal string "..." --- */
        if (c == '"') {
            i++; /* skip opening quote */
            while (i < s.size() && s[i] != '"') {
                if (s[i] == '\\' && i + 1 < s.size()) {
                    tokens.push_back(RegexToken::literal(resolve_escape(s[i + 1])));
                    i += 2;
                } else {
                    tokens.push_back(RegexToken::literal((unsigned char)s[i]));
                    i++;
                }
            }
            if (i < s.size()) i++; /* skip closing quote */
            continue;
        }

        /* --- character class [...] --- */
        if (c == '[') {
            std::set<int> cs = parse_char_class(s, i);
            tokens.push_back(RegexToken::charClass(cs));
            continue;
        }

        /* --- escape sequence --- */
        if (c == '\\') {
            if (i + 1 < s.size()) {
                char c2 = s[i + 1];
                if (c2 == 'x' || c2 == 'X') {
                    /* \xNN */
                    i += 2;
                    std::string hex;
                    while (i < s.size() && isxdigit(s[i])) {
                        hex += s[i]; i++;
                    }
                    tokens.push_back(RegexToken::literal((int)strtol(hex.c_str(), nullptr, 16)));
                } else if (c2 >= '0' && c2 <= '7') {
                    i += 1;
                    std::string oct;
                    for (int cnt = 0; cnt < 3 && i < s.size() && s[i] >= '0' && s[i] <= '7'; cnt++) {
                        oct += s[i]; i++;
                    }
                    tokens.push_back(RegexToken::literal((int)strtol(oct.c_str(), nullptr, 8)));
                } else {
                    tokens.push_back(RegexToken::literal(resolve_escape(c2)));
                    i += 2;
                }
            } else {
                tokens.push_back(RegexToken::literal((unsigned char)c));
                i++;
            }
            continue;
        }

        /* --- dot (any char except \n) --- */
        if (c == '.') {
            tokens.push_back(RegexToken::charClass(dot_set()));
            i++;
            continue;
        }

        /* --- operators --- */
        if (c == '|') { tokens.push_back(RegexToken::op(RegexToken::OP_OR)); i++; continue; }
        if (c == '*') { tokens.push_back(RegexToken::op(RegexToken::OP_STAR)); i++; continue; }
        if (c == '?') { tokens.push_back(RegexToken::op(RegexToken::OP_QUESTION)); i++; continue; }
        if (c == '+') { tokens.push_back(RegexToken::op(RegexToken::OP_PLUS)); i++; continue; }
        if (c == '(') { tokens.push_back(RegexToken::op(RegexToken::LPAREN)); i++; continue; }
        if (c == ')') { tokens.push_back(RegexToken::op(RegexToken::RPAREN)); i++; continue; }

        /* --- {n,m} repetition --- */
        if (c == '{') {
            size_t close = s.find('}', i + 1);
            if (close != std::string::npos) {
                std::string inner = s.substr(i + 1, close - i - 1);
                /* Must look like digits or digits,digits */
                bool valid = !inner.empty();
                for (char ch : inner) {
                    if (!isdigit(ch) && ch != ',') { valid = false; break; }
                }
                if (valid && !tokens.empty()) {
                    /* Parse n,m */
                    int n = 0, m = -1;
                    size_t comma = inner.find(',');
                    if (comma == std::string::npos) {
                        n = m = std::stoi(inner);
                    } else {
                        n = std::stoi(inner.substr(0, comma));
                        std::string ms = inner.substr(comma + 1);
                        m = ms.empty() ? -1 : std::stoi(ms);
                    }

                    /* Expand {n,m}:
                       Pop the last operand (literal / charClass / parenthesized group)
                       and replicate it.
                       
                       For simplicity, locate the last "atom" in the token stream
                       and replicate it. An atom is:
                       - A single LITERAL or CHAR_CLASS token
                       - A (…) group
                    */
                    std::vector<RegexToken> atom;
                    /* find atom end */
                    int k = (int)tokens.size() - 1;
                    if (k >= 0 && tokens[k].type == RegexToken::RPAREN) {
                        /* Walk back to matching LPAREN */
                        int depth = 0;
                        while (k >= 0) {
                            if (tokens[k].type == RegexToken::RPAREN) depth++;
                            if (tokens[k].type == RegexToken::LPAREN) depth--;
                            if (depth == 0) break;
                            k--;
                        }
                    }
                    /* atom = tokens[k..end) */
                    for (int j = k; j < (int)tokens.size(); j++)
                        atom.push_back(tokens[j]);
                    /* remove atom from tokens */
                    tokens.resize(k);

                    /* Build: atom{n} (at least n copies concatenated) */
                    auto emit_atom = [&]() {
                        for (auto& t : atom) tokens.push_back(t);
                    };

                    if (m == -1) {
                        /* {n,} means at least n:  atom^n atom* */
                        for (int j = 0; j < n; j++) emit_atom();
                        emit_atom();
                        tokens.push_back(RegexToken::op(RegexToken::OP_STAR));
                    } else {
                        /* {n,m}: atom^n (atom?){m-n} */
                        for (int j = 0; j < n; j++) emit_atom();
                        for (int j = 0; j < m - n; j++) {
                            emit_atom();
                            tokens.push_back(RegexToken::op(RegexToken::OP_QUESTION));
                        }
                    }

                    i = close + 1;
                    continue;
                }
            }
            /* Fall through: treat '{' as literal */
            tokens.push_back(RegexToken::literal((unsigned char)c));
            i++;
            continue;
        }

        /* --- default: literal character --- */
        tokens.push_back(RegexToken::literal((unsigned char)c));
        i++;
    }

    return tokens;
}

/* ---------- insert explicit concatenation ---------- */

static std::vector<RegexToken> insert_concat(const std::vector<RegexToken>& tokens) {
    std::vector<RegexToken> out;
    for (size_t i = 0; i < tokens.size(); i++) {
        out.push_back(tokens[i]);
        if (i + 1 < tokens.size()) {
            RegexToken::Type a = tokens[i].type;
            RegexToken::Type b = tokens[i + 1].type;
            if (is_operand_end(a) && is_operand_start(b)) {
                out.push_back(RegexToken::op(RegexToken::OP_CONCAT));
            }
        }
    }
    return out;
}

/* ---------- shunting yard: infix -> postfix ---------- */

static int precedence(RegexToken::Type t) {
    switch (t) {
        case RegexToken::OP_OR:      return 1;
        case RegexToken::OP_CONCAT:  return 2;
        case RegexToken::OP_STAR:
        case RegexToken::OP_PLUS:
        case RegexToken::OP_QUESTION: return 3;
        default: return 0;
    }
}

static std::vector<RegexToken> to_postfix(const std::vector<RegexToken>& infix) {
    std::vector<RegexToken> output;
    std::stack<RegexToken> ops;

    for (auto& tok : infix) {
        switch (tok.type) {
            case RegexToken::LITERAL:
            case RegexToken::CHAR_CLASS:
                output.push_back(tok);
                break;
            case RegexToken::LPAREN:
                ops.push(tok);
                break;
            case RegexToken::RPAREN:
                while (!ops.empty() && ops.top().type != RegexToken::LPAREN) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!ops.empty()) ops.pop(); /* pop '(' */
                break;
            default: /* operators */
                while (!ops.empty() && ops.top().type != RegexToken::LPAREN &&
                       precedence(ops.top().type) >= precedence(tok.type)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(tok);
                break;
        }
    }
    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

/* ========== public interface ========== */

std::vector<RegexToken> convert_lex_re(
    const std::string& pattern,
    const std::unordered_map<std::string, std::string>& defs)
{
    /* Step 1: expand named definitions */
    std::string expanded = expand_defs(pattern, defs);
    /* Step 2: tokenize (handles char classes, escapes, quotes, ., {n,m}) */
    std::vector<RegexToken> infix = tokenize(expanded);
    /* Step 3: insert explicit concatenation */
    infix = insert_concat(infix);
    /* Step 4: shunting-yard -> postfix */
    return to_postfix(infix);
}
