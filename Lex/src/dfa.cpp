/**
 * dfa.cpp - Subset construction and Hopcroft minimization
 */
#include "dfa.h"

/* ==================== Subset Construction (NFA -> DFA) ==================== */

static void epsilon_closure(const NFA& nfa, std::set<int>& states) {
    std::stack<int> work;
    for (int s : states) work.push(s);

    while (!work.empty()) {
        int cur = work.top(); work.pop();
        auto range = nfa.states[cur].transitions.equal_range(EPSILON);
        for (auto it = range.first; it != range.second; ++it) {
            if (states.insert(it->second).second) {
                work.push(it->second);
            }
        }
    }
}

static std::set<int> move_set(const NFA& nfa, const std::set<int>& states, int ch) {
    std::set<int> result;
    for (int s : states) {
        auto range = nfa.states[s].transitions.equal_range(ch);
        for (auto it = range.first; it != range.second; ++it) {
            result.insert(it->second);
        }
    }
    return result;
}

DFA nfa_to_dfa(const NFA& nfa) {
    DFA dfa;

    /* Determine the alphabet (all input chars used in NFA, excluding EPSILON) */
    for (auto& st : nfa.states) {
        for (auto& tr : st.transitions) {
            if (tr.first != EPSILON) dfa.alphabet.insert(tr.first);
        }
    }

    /* Map from NFA state set -> DFA state id */
    std::map<std::set<int>, int> state_map;

    /* Start: epsilon-closure of NFA start state */
    std::set<int> start_set;
    start_set.insert(nfa.start_state);
    epsilon_closure(nfa, start_set);

    DFAState s0;
    s0.id = 0;
    s0.nfa_states = start_set;
    dfa.states.push_back(s0);
    state_map[start_set] = 0;
    dfa.start_state = 0;

    std::queue<int> worklist;
    worklist.push(0);

    while (!worklist.empty()) {
        int dfa_id = worklist.front(); worklist.pop();

        for (int ch : dfa.alphabet) {
            std::set<int> mv = move_set(nfa, dfa.states[dfa_id].nfa_states, ch);
            if (mv.empty()) continue;
            epsilon_closure(nfa, mv);

            auto it = state_map.find(mv);
            int target_id;
            if (it == state_map.end()) {
                target_id = (int)dfa.states.size();
                DFAState ns;
                ns.id = target_id;
                ns.nfa_states = mv;
                dfa.states.push_back(ns);
                state_map[mv] = target_id;
                worklist.push(target_id);
            } else {
                target_id = it->second;
            }
            dfa.states[dfa_id].transitions[ch] = target_id;
        }
    }

    /* Determine accept states: pick the rule with lowest index (highest priority) */
    for (auto& ds : dfa.states) {
        int best = INT_MAX;
        for (int ns : ds.nfa_states) {
            if (nfa.states[ns].is_accept && nfa.states[ns].rule_index < best) {
                best = nfa.states[ns].rule_index;
            }
        }
        if (best != INT_MAX) {
            ds.is_accept = true;
            ds.rule_index = best;
        }
    }

    std::cerr << "[SeuLex] DFA constructed: " << dfa.states.size() << " states" << std::endl;
    return dfa;
}

/* ==================== Hopcroft DFA Minimization ==================== */

DFA minimize_dfa(const DFA& dfa) {
    int n = (int)dfa.states.size();
    if (n == 0) return dfa;

    /* Initial partition:
       - Non-accept states -> group 0
       - Accept states: states with different rule_index -> different groups */
    std::map<int, int> rule_to_group; /* rule_index -> group_id */
    std::vector<int> group(n, -1);
    int next_group = 0;

    /* Group for non-accept */
    int non_accept_group = next_group++;
    for (int i = 0; i < n; i++) {
        if (!dfa.states[i].is_accept) {
            group[i] = non_accept_group;
        }
    }

    /* Groups for accept states by rule_index */
    for (int i = 0; i < n; i++) {
        if (dfa.states[i].is_accept) {
            int ri = dfa.states[i].rule_index;
            auto it = rule_to_group.find(ri);
            if (it == rule_to_group.end()) {
                rule_to_group[ri] = next_group;
                group[i] = next_group++;
            } else {
                group[i] = it->second;
            }
        }
    }

    int num_groups = next_group;

    /* Iterative refinement */
    bool changed = true;
    while (changed) {
        changed = false;
        std::map<int, std::vector<int>> group_members;
        for (int i = 0; i < n; i++) {
            group_members[group[i]].push_back(i);
        }

        int new_num_groups = 0;
        std::vector<int> new_group(n, -1);

        for (auto& [gid, members] : group_members) {
            /* Build a signature for each state in this group:
               (for each char in alphabet, the group of the target state, or -1 for no transition) */
            std::map<std::vector<int>, int> sig_to_new_group;

            for (int s : members) {
                std::vector<int> sig;
                for (int ch : dfa.alphabet) {
                    auto it = dfa.states[s].transitions.find(ch);
                    if (it != dfa.states[s].transitions.end()) {
                        sig.push_back(group[it->second]);
                    } else {
                        sig.push_back(-1);
                    }
                }

                auto it = sig_to_new_group.find(sig);
                if (it == sig_to_new_group.end()) {
                    int ng = new_num_groups++;
                    sig_to_new_group[sig] = ng;
                    new_group[s] = ng;
                } else {
                    new_group[s] = it->second;
                }
            }
        }

        if (new_num_groups != num_groups) {
            changed = true;
            num_groups = new_num_groups;
            group = new_group;
        }
    }

    /* Build minimized DFA */
    DFA min_dfa;
    min_dfa.alphabet = dfa.alphabet;

    /* Pick a representative for each group */
    std::map<int, int> group_rep; /* group_id -> first state in that group */
    for (int i = 0; i < n; i++) {
        if (group_rep.find(group[i]) == group_rep.end()) {
            group_rep[group[i]] = i;
        }
    }

    /* Map old group_id -> new DFA state id */
    std::map<int, int> group_to_dfa;
    int new_id = 0;
    for (auto& [gid, rep] : group_rep) {
        group_to_dfa[gid] = new_id++;
    }

    min_dfa.states.resize(new_id);
    for (auto& [gid, rep] : group_rep) {
        int did = group_to_dfa[gid];
        DFAState& ns = min_dfa.states[did];
        ns.id = did;
        ns.is_accept = dfa.states[rep].is_accept;
        ns.rule_index = dfa.states[rep].rule_index;

        for (int ch : dfa.alphabet) {
            auto it = dfa.states[rep].transitions.find(ch);
            if (it != dfa.states[rep].transitions.end()) {
                ns.transitions[ch] = group_to_dfa[group[it->second]];
            }
        }
    }
    min_dfa.start_state = group_to_dfa[group[dfa.start_state]];

    /* Remove dead states:
       A dead state is non-accept and either has no outgoing transitions
       or all transitions lead only back to itself.
       We keep all states reachable from start that can reach an accept state. */
    {
        int sz = (int)min_dfa.states.size();

        /* Forward reachability from start */
        std::vector<bool> fwd_reach(sz, false);
        {
            std::queue<int> q;
            q.push(min_dfa.start_state);
            fwd_reach[min_dfa.start_state] = true;
            while (!q.empty()) {
                int s = q.front(); q.pop();
                for (auto& [ch, t] : min_dfa.states[s].transitions) {
                    if (!fwd_reach[t]) { fwd_reach[t] = true; q.push(t); }
                }
            }
        }

        /* Backward reachability from any accept state */
        std::vector<std::vector<int>> rev(sz);
        for (int i = 0; i < sz; i++) {
            for (auto& [ch, t] : min_dfa.states[i].transitions) {
                rev[t].push_back(i);
            }
        }
        std::vector<bool> bwd_reach(sz, false);
        {
            std::queue<int> q;
            for (int i = 0; i < sz; i++) {
                if (min_dfa.states[i].is_accept) {
                    bwd_reach[i] = true; q.push(i);
                }
            }
            while (!q.empty()) {
                int s = q.front(); q.pop();
                for (int p : rev[s]) {
                    if (!bwd_reach[p]) { bwd_reach[p] = true; q.push(p); }
                }
            }
        }

        /* Keep only states that are both forward and backward reachable,
           OR are the start state */
        std::vector<bool> keep(sz, false);
        for (int i = 0; i < sz; i++) {
            keep[i] = (fwd_reach[i] && (bwd_reach[i] || i == min_dfa.start_state));
        }

        /* Remap */
        std::map<int, int> remap;
        int cnt = 0;
        for (int i = 0; i < sz; i++) {
            if (keep[i]) remap[i] = cnt++;
        }

        DFA clean;
        clean.alphabet = min_dfa.alphabet;
        clean.states.resize(cnt);
        for (int i = 0; i < sz; i++) {
            if (!keep[i]) continue;
            int ni = remap[i];
            clean.states[ni].id = ni;
            clean.states[ni].is_accept = min_dfa.states[i].is_accept;
            clean.states[ni].rule_index = min_dfa.states[i].rule_index;
            for (auto& [ch, t] : min_dfa.states[i].transitions) {
                if (keep[t]) {
                    clean.states[ni].transitions[ch] = remap[t];
                }
            }
        }
        clean.start_state = remap.count(min_dfa.start_state) ? remap[min_dfa.start_state] : 0;
        min_dfa = clean;
    }

    std::cerr << "[SeuLex] DFA minimized: " << min_dfa.states.size() << " states" << std::endl;
    return min_dfa;
}
