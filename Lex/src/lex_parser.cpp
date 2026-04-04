/**
 * lex_parser.cpp - Implementation of .l file parser
 */
#include "lex_parser.h"

/**
 * Trim leading and trailing whitespace from a string.
 */
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

/**
 * Extract the action code from a rule line.
 * Actions can be:
 *   - A single { ... } block (possibly with nested braces)
 *   - A single statement without braces (terminated by newline)
 * Multi-line actions start with { on the first line and may span multiple lines.
 */
static std::string extract_action(const std::string& line, size_t action_start) {
    std::string rest = line.substr(action_start);
    rest = trim(rest);
    return rest;
}

/**
 * Find where the pattern ends and action begins in a rule line.
 * Patterns can contain spaces if quoted or in character classes.
 * The action starts after whitespace following the completed pattern.
 */
static size_t find_action_start(const std::string& line) {
    bool in_quotes = false;
    bool in_brackets = false;
    bool escaped = false;
    size_t i = 0;
    
    /* Skip leading whitespace */
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;
    
    /* Walk through the pattern */
    size_t pattern_start = i;
    while (i < line.size()) {
        if (escaped) {
            escaped = false;
            i++;
            continue;
        }
        if (line[i] == '\\') {
            escaped = true;
            i++;
            continue;
        }
        if (in_brackets) {
            if (line[i] == ']') {
                in_brackets = false;
            }
            i++;
            continue;
        }
        if (line[i] == '"') {
            in_quotes = !in_quotes;
            i++;
            continue;
        }
        if (in_quotes) {
            i++;
            continue;
        }
        if (line[i] == '[') {
            in_brackets = true;
            i++;
            continue;
        }
        /* Space or tab outside of quotes/brackets marks end of pattern */
        if (line[i] == ' ' || line[i] == '\t') {
            break;
        }
        i++;
    }
    
    /* Skip whitespace between pattern and action */
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;
    
    return i;
}

LexFile parse_lex_file(const std::string& filename) {
    LexFile result;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file '" << filename << "'" << std::endl;
        exit(1);
    }
    
    std::string content;
    {
        std::ostringstream ss;
        ss << file.rdbuf();
        content = ss.str();
    }
    file.close();
    
    /* Split by line-delimiter %% (more robust than searching for "\n%%") */
    std::vector<std::string> sections;
    sections.emplace_back();
    {
        std::istringstream stream(content);
        std::string raw_line;
        int section_idx = 0;
        bool in_def_code_block = false;

        while (std::getline(stream, raw_line)) {
            std::string trimmed = trim(raw_line);

            if (section_idx == 0) {
                if (trimmed == "%{") {
                    in_def_code_block = true;
                } else if (trimmed == "%}") {
                    in_def_code_block = false;
                }
            }

            if (!in_def_code_block && trimmed == "%%") {
                section_idx++;
                if ((int)sections.size() <= section_idx) {
                    sections.emplace_back();
                }
                continue;
            }

            sections[section_idx] += raw_line;
            sections[section_idx] += "\n";
        }
    }
    
    if (sections.size() < 2) {
        std::cerr << "Error: .l file must contain at least two sections separated by %%"
                  << std::endl;
        exit(1);
    }
    
    /* ===== Parse Definition Section ===== */
    std::string& def_section = sections[0];
    std::istringstream def_stream(def_section);
    std::string line;
    bool in_code_block = false;
    
    while (std::getline(def_stream, line)) {
        std::string trimmed = trim(line);
        
        if (trimmed == "%{") {
            in_code_block = true;
            continue;
        }
        if (trimmed == "%}") {
            in_code_block = false;
            continue;
        }
        
        if (in_code_block) {
            result.definition_code += line + "\n";
            continue;
        }
        
        if (trimmed.empty()) continue;
        
        /* Named regex definitions: NAME  regex */
        /* The name must start at column 0 (no leading whitespace) */
        if (!line.empty() && line[0] != ' ' && line[0] != '\t') {
            /* Find first whitespace to split name from definition */
            size_t ws = line.find_first_of(" \t");
            if (ws != std::string::npos) {
                std::string name = line.substr(0, ws);
                std::string def = trim(line.substr(ws));
                if (!name.empty() && !def.empty()) {
                    result.defs[name] = def;
                    result.def_order.push_back(name);
                }
            }
        }
    }
    
    /* ===== Parse Rules Section ===== */
    std::string& rule_section = sections[1];
    std::istringstream rule_stream(rule_section);
    int rule_index = 0;
    
    while (std::getline(rule_stream, line)) {
        if (trim(line).empty()) continue;
        
        /* Lines starting with whitespace that are not part of a multi-line action
           are continuation lines - skip for now (we handle multi-line actions below) */
        if (line[0] == ' ' || line[0] == '\t') {
            /* Could be a continuation of previous action */
            if (!result.rules.empty()) {
                /* Append to previous rule's action */
                result.rules.back().action += "\n" + line;
            }
            continue;
        }
        
        size_t action_pos = find_action_start(line);
        if (action_pos >= line.size()) continue;
        
        std::string pattern = trim(line.substr(0, action_pos));
        std::string action = extract_action(line, action_pos);
        
        if (pattern.empty()) continue;
        
        LexRule rule;
        rule.pattern = pattern;
        rule.action = action;
        rule.rule_index = rule_index++;
        
        /* Handle multi-line brace-delimited actions */
        if (!action.empty() && action[0] == '{') {
            int brace_depth = 0;
            bool in_str = false;
            bool in_chr = false;
            bool esc = false;
            for (char c : action) {
                if (esc) { esc = false; continue; }
                if (c == '\\') { esc = true; continue; }
                if (c == '"' && !in_chr) { in_str = !in_str; continue; }
                if (c == '\'' && !in_str) { in_chr = !in_chr; continue; }
                if (in_str || in_chr) continue;
                if (c == '{') brace_depth++;
                if (c == '}') brace_depth--;
            }
            while (brace_depth > 0 && std::getline(rule_stream, line)) {
                rule.action += "\n" + line;
                esc = false; in_str = false; in_chr = false;
                for (char c : line) {
                    if (esc) { esc = false; continue; }
                    if (c == '\\') { esc = true; continue; }
                    if (c == '"' && !in_chr) { in_str = !in_str; continue; }
                    if (c == '\'' && !in_str) { in_chr = !in_chr; continue; }
                    if (in_str || in_chr) continue;
                    if (c == '{') brace_depth++;
                    if (c == '}') brace_depth--;
                }
            }
        }
        
        result.rules.push_back(rule);
    }
    
    /* ===== User Subroutine Section ===== */
    if (sections.size() > 2) {
        result.user_code = sections[2];
    }
    
    return result;
}
