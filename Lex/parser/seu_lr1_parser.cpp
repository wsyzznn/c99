/**
 * LR(1) 表驱动 + 规约建 AST（与 grammar.cpp / y.tab.h 一致）
 *
 * SeuYacc / LR(1)+LALR(1) 管线：
 * - First 集与可空（nullable）
 * - LR(1) 项 [A→α·β, a] 的闭包（对 βa 求 FIRST 作为预测符）
 * - GOTO 与 LR(1) 项集族；再合并同心项得 LALR(1) 表（状态数减少）
 * - ACTION/GOTO 表；移进-规约冲突时移进优先
 */
#include "seu_lr1_parser.hpp"
#include "grammar.hpp"
#include "symbols.hpp"
#include "../ast.h"
#include "../y.tab.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

extern "C" {
extern int yylex(void);
extern FILE *yyin;
extern char yytext[];
extern int line_num;
}

using namespace seulr;

struct LR1Item {
    int pid{};
    int dot{};
    int la{}; /* 向前看终结符，含 T_DOLLAR */
    bool operator<(const LR1Item &o) const {
        if (pid != o.pid)
            return pid < o.pid;
        if (dot != o.dot)
            return dot < o.dot;
        return la < o.la;
    }
};

using LR1ItemSet = std::set<LR1Item>;

static constexpr int EPS = -2;
static constexpr int ACT_SHIFT_BASE = 1;
static constexpr int ACT_ACCEPT = 1000000;
static constexpr int ACT_ERR = 0;

static std::vector<LR1ItemSet> g_states;
static std::map<std::vector<int>, int> g_state_index;
static std::vector<std::map<int, int>> g_action;
static std::vector<std::vector<int>> g_goto;

static std::vector<char> g_nullable_nt;
static std::vector<std::set<int>> g_first_nt;

static bool is_nt_sym(int s) { return s >= NT_BASE; }

static void collect_syms(std::set<int> &sy) {
    for (const auto &p : g_prods) {
        sy.insert(p.lhs);
        for (int x : p.rhs)
            sy.insert(x);
    }
}

/** FIRST(β) ∪ {a} 中终结部分：β 为点右侧后缀，a 为项的向前看 */
static std::set<int> first_future(const std::vector<int> &beta, int a) {
    std::set<int> r;
    for (size_t i = 0; i < beta.size(); ++i) {
        int x = beta[i];
        if (!is_nt_sym(x)) {
            r.insert(x);
            return r;
        }
        const auto &fx = g_first_nt[(size_t)(x - NT_BASE)];
        for (int t : fx) {
            if (t != EPS)
                r.insert(t);
        }
        if (!g_nullable_nt[(size_t)(x - NT_BASE)])
            return r;
    }
    r.insert(a);
    return r;
}

static void compute_nullable_and_first() {
    std::set<int> allsy;
    collect_syms(allsy);

    g_nullable_nt.assign((size_t)NT_NUM, 0);
    bool chg = true;
    while (chg) {
        chg = false;
        for (const auto &p : g_prods) {
            if (!is_nt_sym(p.lhs))
                continue;
            size_t idx = (size_t)(p.lhs - NT_BASE);
            if (p.rhs.empty()) {
                if (!g_nullable_nt[idx]) {
                    g_nullable_nt[idx] = 1;
                    chg = true;
                }
                continue;
            }
            bool alln = true;
            for (int x : p.rhs) {
                if (is_nt_sym(x)) {
                    if (!g_nullable_nt[(size_t)(x - NT_BASE)]) {
                        alln = false;
                        break;
                    }
                } else {
                    alln = false;
                    break;
                }
            }
            if (alln && !g_nullable_nt[idx]) {
                g_nullable_nt[idx] = 1;
                chg = true;
            }
        }
    }

    g_first_nt.assign((size_t)NT_NUM, {});
    for (size_t i = 0; i < (size_t)NT_NUM; ++i)
        if (g_nullable_nt[i])
            g_first_nt[i].insert(EPS);

    chg = true;
    while (chg) {
        chg = false;
        for (const auto &p : g_prods) {
            if (!is_nt_sym(p.lhs))
                continue;
            size_t ai = (size_t)(p.lhs - NT_BASE);
            std::set<int> add;
            for (size_t i = 0; i < p.rhs.size(); ++i) {
                int x = p.rhs[i];
                if (!is_nt_sym(x)) {
                    add.insert(x);
                    break;
                }
                const auto &fx = g_first_nt[(size_t)(x - NT_BASE)];
                for (int t : fx) {
                    if (t != EPS)
                        add.insert(t);
                }
                if (!fx.count(EPS))
                    break;
                if (i + 1 == p.rhs.size())
                    add.insert(EPS);
            }
            if (p.rhs.empty())
                add.insert(EPS);
            for (int t : add) {
                if (!g_first_nt[ai].count(t)) {
                    g_first_nt[ai].insert(t);
                    chg = true;
                }
            }
        }
    }
    (void)allsy;
}

static LR1ItemSet closure_lr1(const LR1ItemSet &I) {
    LR1ItemSet J = I;
    for (;;) {
        LR1ItemSet add;
        for (const LR1Item &it : J) {
            const auto &rhs = g_prods[(size_t)it.pid].rhs;
            if (it.dot >= (int)rhs.size())
                continue;
            int B = rhs[(size_t)it.dot];
            if (!is_nt_sym(B))
                continue;
            std::vector<int> beta(rhs.begin() + it.dot + 1, rhs.end());
            std::set<int> look = first_future(beta, it.la);
            for (size_t j = 0; j < g_prods.size(); ++j) {
                if (g_prods[j].lhs != B)
                    continue;
                for (int b : look) {
                    LR1Item n{(int)j, 0, b};
                    if (!J.count(n))
                        add.insert(n);
                }
            }
        }
        if (add.empty())
            break;
        J.insert(add.begin(), add.end());
    }
    return J;
}

static LR1ItemSet goto_lr1(const LR1ItemSet &I, int X) {
    LR1ItemSet raw;
    for (const LR1Item &it : I) {
        const auto &rhs = g_prods[(size_t)it.pid].rhs;
        if (it.dot < (int)rhs.size() && rhs[(size_t)it.dot] == X)
            raw.insert({it.pid, it.dot + 1, it.la});
    }
    return closure_lr1(raw);
}

static std::vector<int> canon_key_lr1(const LR1ItemSet &s) {
    std::vector<int> v;
    for (const LR1Item &p : s) {
        v.push_back(p.pid);
        v.push_back(p.dot);
        v.push_back(p.la);
    }
    return v;
}

static int intern_state_lr1(const LR1ItemSet &S) {
    std::vector<int> key = canon_key_lr1(S);
    auto it = g_state_index.find(key);
    if (it != g_state_index.end())
        return it->second;
    int id = (int)g_states.size();
    g_states.push_back(S);
    g_state_index[key] = id;
    return id;
}

static void compress_lr1_to_lalr();

static void build_lr1_table() {
    g_states.clear();
    g_state_index.clear();
    g_action.clear();
    g_goto.clear();

    LR1ItemSet I0 = closure_lr1({LR1Item{0, 0, T_DOLLAR}});
    intern_state_lr1(I0);
    std::queue<int> q;
    q.push(0);

    while (!q.empty()) {
        int sid = q.front();
        q.pop();
        std::set<int> outs;
        for (const LR1Item &it : g_states[(size_t)sid]) {
            const auto &rhs = g_prods[(size_t)it.pid].rhs;
            if (it.dot < (int)rhs.size())
                outs.insert(rhs[(size_t)it.dot]);
        }
        for (int X : outs) {
            LR1ItemSet G = goto_lr1(g_states[(size_t)sid], X);
            if (G.empty())
                continue;
            size_t before = g_states.size();
            int gid = intern_state_lr1(G);
            if (g_states.size() > before)
                q.push(gid);
            if (!is_nt_sym(X)) {
                if ((int)g_action.size() <= sid)
                    g_action.resize(sid + 1);
                int &slot = g_action[sid][X];
                int sh = ACT_SHIFT_BASE + gid;
                if (slot == ACT_ERR)
                    slot = sh;
                else if (slot >= ACT_SHIFT_BASE && slot != sh)
                    std::fprintf(stderr, "[seulr LR1] shift/shift conflict state=%d sym=%d\n", sid, X);
                else if (slot < 0) {
                    std::fprintf(stderr, "[seulr LR1] shift/reduce conflict state=%d sym=%d -> shift\n", sid,
                                 X);
                    slot = sh;
                }
            } else {
                if ((int)g_goto.size() <= sid)
                    g_goto.resize(sid + 1, std::vector<int>(NT_NUM, -1));
                g_goto[sid][X - NT_BASE] = gid;
            }
        }
    }

    int n = (int)g_states.size();
    g_action.resize((size_t)n);
    g_goto.resize((size_t)n, std::vector<int>(NT_NUM, -1));

    for (int sid = 0; sid < n; ++sid) {
        for (const LR1Item &it : g_states[(size_t)sid]) {
            const auto &rhs = g_prods[(size_t)it.pid].rhs;
            if (it.dot != (int)rhs.size())
                continue;
            if (it.pid == 0) {
                int &slot = g_action[sid][it.la];
                if (slot == ACT_ERR)
                    slot = ACT_ACCEPT;
                else if (slot != ACT_ACCEPT)
                    std::fprintf(stderr, "[seulr LR1] warn: accept slot busy state=%d la=%d\n", sid, it.la);
                continue;
            }
            int &slot = g_action[sid][it.la];
            int red = -(it.pid + 1);
            if (slot == ACT_ERR)
                slot = red;
            else if (slot >= ACT_SHIFT_BASE) {
                std::fprintf(stderr, "[seulr LR1] reduce/shift conflict state=%d la=%d -> shift\n", sid, it.la);
            } else if (slot < 0 && slot != red) {
                std::fprintf(stderr, "[seulr LR1] reduce/reduce state=%d la=%d (keep first)\n", sid, it.la);
            }
        }
    }

    compress_lr1_to_lalr();
}

/** 同心 LR(1) 状态合并为 LALR(1)：合并 ACTION/GOTO 行，移进目标改为 LALR 状态号 */
static void compress_lr1_to_lalr() {
    int n = (int)g_states.size();
    if (n <= 0)
        return;

    auto core_key = [&](int sid) {
        std::vector<std::pair<int, int>> pairs;
        for (const LR1Item &it : g_states[(size_t)sid])
            pairs.push_back({it.pid, it.dot});
        std::sort(pairs.begin(), pairs.end());
        pairs.erase(std::unique(pairs.begin(), pairs.end()), pairs.end());
        std::vector<int> flat;
        for (const auto &p : pairs) {
            flat.push_back(p.first);
            flat.push_back(p.second);
        }
        return flat;
    };

    std::map<std::vector<int>, std::vector<int>> core2lr1;
    for (int i = 0; i < n; ++i)
        core2lr1[core_key(i)].push_back(i);

    std::vector<int> lr1_to_lalr((size_t)n, -1);
    std::vector<LR1ItemSet> lalr_states;
    int m = 0;
    for (const auto &kv : core2lr1) {
        (void)kv.first;
        for (int sid : kv.second)
            lr1_to_lalr[(size_t)sid] = m;
        LR1ItemSet merged;
        for (int sid : kv.second) {
            for (const LR1Item &it : g_states[(size_t)sid])
                merged.insert(it);
        }
        lalr_states.push_back(std::move(merged));
        ++m;
    }

    std::vector<std::map<int, int>> lalr_action((size_t)m);
    auto merge_act = [&](int u, int sym, int act) {
        int &slot = lalr_action[(size_t)u][sym];
        if (slot == ACT_ERR) {
            slot = act;
            return;
        }
        if (act == slot)
            return;
        if (act == ACT_ACCEPT || slot == ACT_ACCEPT) {
            if (act == ACT_ACCEPT)
                slot = ACT_ACCEPT;
            return;
        }
        if (act >= ACT_SHIFT_BASE && slot >= ACT_SHIFT_BASE) {
            if (act != slot)
                std::fprintf(stderr, "[seulr LALR] shift/shift u=%d sym=%d\n", u, sym);
            return;
        }
        if (act >= ACT_SHIFT_BASE && slot < 0) {
            slot = act;
            return;
        }
        if (act < 0 && slot >= ACT_SHIFT_BASE)
            return;
        if (act < 0 && slot < 0 && act != slot)
            std::fprintf(stderr, "[seulr LALR] reduce/reduce u=%d sym=%d (keep first)\n", u, sym);
    };

    for (int i = 0; i < n; ++i) {
        int u = lr1_to_lalr[(size_t)i];
        for (const auto &pr : g_action[(size_t)i]) {
            int sym = pr.first;
            int act = pr.second;
            if (act >= ACT_SHIFT_BASE) {
                int j = act - ACT_SHIFT_BASE;
                if (j >= 0 && j < n)
                    act = ACT_SHIFT_BASE + lr1_to_lalr[(size_t)j];
            }
            merge_act(u, sym, act);
        }
    }

    std::vector<std::vector<int>> lalr_goto((size_t)m, std::vector<int>(NT_NUM, -1));
    for (int i = 0; i < n; ++i) {
        int u = lr1_to_lalr[(size_t)i];
        for (int k = 0; k < NT_NUM; ++k) {
            int j = g_goto[(size_t)i][(size_t)k];
            if (j < 0)
                continue;
            int v = lr1_to_lalr[(size_t)j];
            int &g = lalr_goto[(size_t)u][(size_t)k];
            if (g >= 0 && g != v)
                std::fprintf(stderr, "[seulr LALR] GOTO conflict u=%d nt=%d\n", u, NT_BASE + k);
            g = v;
        }
    }

    g_states = std::move(lalr_states);
    g_action = std::move(lalr_action);
    g_goto = std::move(lalr_goto);
    g_state_index.clear();
    std::fprintf(stderr, "[seulr LALR] compressed LR1 %d states -> %zu LALR states\n", n, g_states.size());
}

static ASTNode *shift_semantic(int tok) {
    switch (tok) {
        case IDENTIFIER:
            return ast_var(yylval.str_val ? strdup(yylval.str_val) : strdup("?"));
        case I_CONSTANT:
            return ast_int(yylval.intval);
        case F_CONSTANT:
            return ast_double(yylval.fval);
        case STRING_LITERAL:
            return ast_string(yylval.str_val ? strdup(yylval.str_val) : strdup(""));
        case VOID: {
            char *p = strdup("void");
            ASTNode *n = ast_var(p);
            free(p);
            return n;
        }
        case INT: {
            char *p = strdup("int");
            ASTNode *n = ast_var(p);
            free(p);
            return n;
        }
        case FLOAT: {
            char *p = strdup("float");
            ASTNode *n = ast_var(p);
            free(p);
            return n;
        }
        case DOUBLE: {
            char *p = strdup("double");
            ASTNode *n = ast_var(p);
            free(p);
            return n;
        }
        case CHAR: {
            char *p = strdup("char");
            ASTNode *n = ast_var(p);
            free(p);
            return n;
        }
        default:
            return nullptr;
    }
}

static ASTNode *make_decl_spec(ASTNode *ts) {
    ASTNode **a = (ASTNode **)malloc(sizeof(ASTNode *));
    a[0] = ts;
    ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
    memset(n, 0, sizeof(*n));
    n->type = AST_DECL_SPEC;
    n->ds.specs = a;
    n->ds.scount = 1;
    return n;
}

static ASTNode *reduce_prod(int pid, std::vector<ASTNode *> &r) {
    switch (pid) {
        case 1: {
            ASTNode *root = (ASTNode *)malloc(sizeof(ASTNode));
            memset(root, 0, sizeof(*root));
            root->type = AST_TRANSLATION_UNIT;
            root->seq.count = 1;
            root->seq.list = (ASTNode **)malloc(sizeof(ASTNode *));
            root->seq.list[0] = r[0];
            return root;
        }
        case 2: {
            ASTNode *tu = r[0];
            ASTNode *ed = r[1];
            int n = tu->seq.count;
            tu->seq.list = (ASTNode **)realloc(tu->seq.list, sizeof(ASTNode *) * (n + 1));
            tu->seq.list[n] = ed;
            tu->seq.count = n + 1;
            return tu;
        }
        case 3:
        case 4:
            return r[0];
        case 5: {
            ASTNode *spec = r[0];
            ASTNode *fn = r[1];
            ASTNode *body = r[2];
            if (fn->type == AST_FUNC_TYPE) {
                fn->ft.ret_type = spec->ds.specs[0];
                fn->ft.body = body;
                free(spec);
            }
            return fn;
        }
        case 6: {
            ASTNode *spec = r[0];
            ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
            memset(n, 0, sizeof(*n));
            n->type = AST_DECLARATION;
            n->declaration.specs = spec->ds.specs;
            n->declaration.scount = spec->ds.scount;
            n->declaration.inits = r[1]->seq.list;
            n->declaration.icount = r[1]->seq.count;
            free(r[1]);
            free(spec);
            return n;
        }
        case 7:
            return make_decl_spec(r[0]);
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            return r[0];
        case 13: {
            ASTNode *w = (ASTNode *)malloc(sizeof(ASTNode));
            memset(w, 0, sizeof(*w));
            w->type = AST_TRANSLATION_UNIT;
            w->seq.count = 1;
            w->seq.list = (ASTNode **)malloc(sizeof(ASTNode *));
            w->seq.list[0] = r[0];
            return w;
        }
        case 14: {
            ASTNode *listw = r[0];
            int n = listw->seq.count;
            listw->seq.list = (ASTNode **)realloc(listw->seq.list, sizeof(ASTNode *) * (n + 1));
            listw->seq.list[n] = r[2];
            listw->seq.count = n + 1;
            return listw;
        }
        case 15: {
            ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
            memset(n, 0, sizeof(*n));
            n->type = AST_INIT_DECL;
            n->id.declr = r[0];
            n->id.init = nullptr;
            return n;
        }
        case 16: {
            ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
            memset(n, 0, sizeof(*n));
            n->type = AST_INIT_DECL;
            n->id.declr = r[0];
            n->id.init = r[2];
            return n;
        }
        case 17:
        case 18:
            return r[0];
        case 19: {
            ASTNode *base = r[0];
            ASTNode *pl = r[2];
            ASTNode *fn = (ASTNode *)malloc(sizeof(ASTNode));
            memset(fn, 0, sizeof(*fn));
            fn->type = AST_FUNC_TYPE;
            fn->ft.name = strdup(base->varname);
            fn->ft.params = pl;
            fn->ft.ret_type = nullptr;
            fn->ft.body = nullptr;
            ast_free(base);
            return fn;
        }
        case 20: {
            ASTNode *pl = (ASTNode *)malloc(sizeof(ASTNode));
            memset(pl, 0, sizeof(*pl));
            pl->type = AST_PARAM_LIST;
            pl->seq.list = nullptr;
            pl->seq.count = 0;
            return pl;
        }
        case 21:
            return r[0];
        case 22: {
            ASTNode *pl = (ASTNode *)malloc(sizeof(ASTNode));
            memset(pl, 0, sizeof(*pl));
            pl->type = AST_PARAM_LIST;
            pl->seq.count = 1;
            pl->seq.list = (ASTNode **)malloc(sizeof(ASTNode *));
            pl->seq.list[0] = r[0];
            return pl;
        }
        case 23: {
            ASTNode *pl = r[0];
            int n = pl->seq.count;
            pl->seq.list = (ASTNode **)realloc(pl->seq.list, sizeof(ASTNode *) * (n + 1));
            pl->seq.list[n] = r[2];
            pl->seq.count = n + 1;
            return pl;
        }
        case 24:
            return ast_compound(nullptr, 0);
        case 25: {
            ASTNode *w = r[1];
            if (w->type == AST_TRANSLATION_UNIT) {
                ASTNode *b = ast_compound(w->seq.list, w->seq.count);
                /* ast_compound 接管 list 所有权，禁止再 free(w->seq.list) */
                free(w);
                return b;
            }
            return ast_compound(nullptr, 0);
        }
        case 26: {
            ASTNode *w = (ASTNode *)malloc(sizeof(ASTNode));
            memset(w, 0, sizeof(*w));
            w->type = AST_TRANSLATION_UNIT;
            w->seq.count = 1;
            w->seq.list = (ASTNode **)malloc(sizeof(ASTNode *));
            w->seq.list[0] = r[0];
            return w;
        }
        case 27: {
            ASTNode *w = r[0];
            int n = w->seq.count;
            w->seq.list = (ASTNode **)realloc(w->seq.list, sizeof(ASTNode *) * (n + 1));
            w->seq.list[n] = r[1];
            w->seq.count = n + 1;
            return w;
        }
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            return r[0];
        case 35:
            return nullptr;
        case 36:
            return r[0];
        case 37:
            return nullptr;
        case 38:
            return r[0];
        case 39:
            return ast_if(r[2], r[4], nullptr);
        case 40:
            return ast_if(r[2], r[4], r[6]);
        case 41:
            return ast_while(r[2], r[4]);
        case 42:
            return ast_do_while(r[1], r[4]);
        case 43:
            return ast_for(r[2], r[3], nullptr, r[5]);
        case 44:
            return ast_for(r[2], r[3], r[4], r[6]);
        case 45:
            return ast_for(r[2], r[3], r[5], r[7]);
        case 46: {
            ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
            memset(n, 0, sizeof(*n));
            n->type = AST_RETURN;
            n->expr = r[1];
            return n;
        }
        case 47: {
            ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
            memset(n, 0, sizeof(*n));
            n->type = AST_BREAK;
            return n;
        }
        case 48:
            return ast_continue();
        case 49:
        case 50:
            return r[0];
        case 51:
            return ast_assign(r[0], r[2]);
        case 52:
            return r[0];
        case 53:
            return ast_binop('|', r[0], r[2]);
        case 54:
            return r[0];
        case 55:
            return ast_binop('&', r[0], r[2]);
        case 56:
            return r[0];
        case 57:
            return ast_binop('e', r[0], r[2]);
        case 58:
            return ast_binop('n', r[0], r[2]);
        case 59:
            return r[0];
        case 60:
            return ast_binop('<', r[0], r[2]);
        case 61:
            return ast_binop('>', r[0], r[2]);
        case 62:
            return ast_binop('l', r[0], r[2]);
        case 63:
            return ast_binop('g', r[0], r[2]);
        case 64:
            return r[0];
        case 65:
            return ast_binop('+', r[0], r[2]);
        case 66:
            return ast_binop('-', r[0], r[2]);
        case 67:
            return r[0];
        case 68:
            return ast_binop('*', r[0], r[2]);
        case 69:
            return ast_binop('/', r[0], r[2]);
        case 70:
            return ast_binop('%', r[0], r[2]);
        case 71:
            return r[0];
        case 72:
            return ast_binop('-', ast_int(0), r[1]);
        case 73:
            return r[0];
        case 74: {
            if (r[0]->type != AST_VAR)
                return r[0];
            char *fn = strdup(r[0]->varname);
            ast_free(r[0]);
            return ast_call(fn, nullptr, 0);
        }
        case 75: {
            ASTNode *pf = r[0];
            ASTNode *al = r[2];
            int argc = al->seq.count;
            ASTNode **args = (ASTNode **)malloc(sizeof(ASTNode *) * (size_t)argc);
            memcpy(args, al->seq.list, sizeof(ASTNode *) * (size_t)argc);
            free(al->seq.list);
            free(al);
            if (pf->type != AST_VAR) {
                free(args);
                return pf;
            }
            char *fn = strdup(pf->varname);
            ast_free(pf);
            return ast_call(fn, args, argc);
        }
        case 76: {
            ASTNode *w = (ASTNode *)malloc(sizeof(ASTNode));
            memset(w, 0, sizeof(*w));
            w->type = AST_TRANSLATION_UNIT;
            w->seq.count = 1;
            w->seq.list = (ASTNode **)malloc(sizeof(ASTNode *));
            w->seq.list[0] = r[0];
            return w;
        }
        case 77: {
            ASTNode *w = r[0];
            int n = w->seq.count;
            w->seq.list = (ASTNode **)realloc(w->seq.list, sizeof(ASTNode *) * (n + 1));
            w->seq.list[n] = r[2];
            w->seq.count = n + 1;
            return w;
        }
        case 78:
        case 79:
        case 80:
        case 81:
        case 82:
            return r[0];
        default:
            std::fprintf(stderr, "[seulr LR1] missing reduce prod %d\n", pid);
            return nullptr;
    }
}

int seulr_init(void) {
    grammar_init();
    compute_nullable_and_first();
    build_lr1_table();
    std::fprintf(stderr, "[seulr] LALR(1) states=%zu (after core merge)\n", g_states.size());
    return 0;
}

static int act_col(int la) { return la == 0 ? T_DOLLAR : la; }

int seulr_parse(FILE *in, ASTNode **root) {
    *root = nullptr;
    seulr_init();
    yyin = in;
    line_num = 1;

    std::vector<int> st;
    std::vector<ASTNode *> vl;
    st.push_back(0);
    vl.push_back(nullptr);

    int look = yylex();
    for (;;) {
        int s = st.back();
        int a = act_col(look);
        auto it = g_action[(size_t)s].find(a);
        int act = (it == g_action[(size_t)s].end()) ? ACT_ERR : it->second;
        if (act == ACT_ERR) {
            std::fprintf(stderr, "syntax error state=%d la=%d (%s) line=%d\n", s, look,
                         yytext, line_num);
            return 1;
        }
        if (act == ACT_ACCEPT) {
            if (vl.size() >= 2)
                *root = vl[vl.size() - 2];
            else
                *root = vl.back();
            return 0;
        }
        if (act >= ACT_SHIFT_BASE && act < ACT_SHIFT_BASE + (int)g_states.size()) {
            int ns = act - ACT_SHIFT_BASE;
            ASTNode *sem = shift_semantic(look);
            st.push_back(ns);
            vl.push_back(sem);
            if (a != T_DOLLAR)
                look = yylex();
            else
                look = 0;
            continue;
        }
        if (act < 0) {
            int pid = -act - 1;
            const auto &rhs = g_prods[(size_t)pid].rhs;
            int k = (int)rhs.size();
            std::vector<ASTNode *> child((size_t)k);
            for (int i = k - 1; i >= 0; --i) {
                st.pop_back();
                child[(size_t)i] = vl.back();
                vl.pop_back();
            }
            ASTNode *nv = reduce_prod(pid, child);
            int s2 = st.back();
            int lhs = g_prods[(size_t)pid].lhs;
            int gt = g_goto[(size_t)s2][lhs - NT_BASE];
            if (gt < 0) {
                std::fprintf(stderr, "internal: goto error prod=%d lhs=%d\n", pid, lhs);
                return 1;
            }
            st.push_back(gt);
            vl.push_back(nv);
            continue;
        }
        std::fprintf(stderr, "internal: bad action %d\n", act);
        return 1;
    }
}
