/**
 * 中间代码（三地址）骨架：遍历 AST，演示 translation scheme 的「编译期」产物。
 * 符号表/回填/基本块可在此基础上扩展（课件 LLVM IR 准备）。
 */
#include "ir_from_ast.hpp"
#include "../ast.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static int g_tmp = 0;
static int g_label = 0;

static std::string fresh_tmp() { return "t" + std::to_string(++g_tmp); }
static std::string fresh_label() { return "L" + std::to_string(++g_label); }

struct Quad {
    std::string op, a, b, res;
};

static void emit_line(FILE *out, const Quad &q) {
    if (q.op == "=" && q.a.empty())
        std::fprintf(out, "%s = %s\n", q.res.c_str(), q.b.c_str());
    else if (q.op == "=")
        std::fprintf(out, "%s = %s\n", q.res.c_str(), q.a.c_str());
    else if (q.op == "copy")
        std::fprintf(out, "%s = %s\n", q.res.c_str(), q.a.c_str());
    else
        std::fprintf(out, "%s = %s %s %s\n", q.res.c_str(), q.a.c_str(), q.op.c_str(), q.b.c_str());
}

static std::string gen_expr(FILE *out, ASTNode *n);

static std::string gen_expr(FILE *out, ASTNode *n) {
    if (!n)
        return "";
    switch (n->type) {
        case AST_INT: {
            std::string t = fresh_tmp();
            emit_line(out, Quad{"=", "", std::to_string(n->intval), t});
            return t;
        }
        case AST_DOUBLE: {
            std::string t = fresh_tmp();
            char buf[64];
            std::snprintf(buf, sizeof buf, "%g", n->fval);
            emit_line(out, Quad{"=", "", buf, t});
            return t;
        }
        case AST_VAR:
            return n->varname ? std::string(n->varname) : std::string("?");
        case AST_BINOP: {
            std::string L = gen_expr(out, n->bin.lhs);
            std::string R = gen_expr(out, n->bin.rhs);
            std::string t = fresh_tmp();
            char op[4] = {n->bin.op, 0};
            emit_line(out, Quad{op, L, R, t});
            return t;
        }
        case AST_ASSIGN: {
            std::string rv = gen_expr(out, n->assign.r);
            if (n->assign.l && n->assign.l->type == AST_VAR && n->assign.l->varname) {
                emit_line(out, Quad{"copy", rv, "", n->assign.l->varname});
                return n->assign.l->varname;
            }
            return rv;
        }
        case AST_CALL: {
            std::string t = fresh_tmp();
            std::fprintf(out, "param /* %d args */\n", n->call.argc);
            for (int i = 0; i < n->call.argc; ++i)
                (void)gen_expr(out, n->call.args[i]);
            std::fprintf(out, "%s = call %s\n", t.c_str(), n->call.fname ? n->call.fname : "?");
            return t;
        }
        default:
            return "";
    }
}

static void gen_stmt(FILE *out, ASTNode *n) {
    if (!n)
        return;
    switch (n->type) {
        case AST_COMPOUND_STMT:
            for (int i = 0; i < n->seq.count; ++i)
                gen_stmt(out, n->seq.list[i]);
            break;
        case AST_ASSIGN:
        case AST_BINOP:
        case AST_CALL:
        case AST_INT:
        case AST_VAR:
            (void)gen_expr(out, n);
            break;
        case AST_IF: {
            std::string c = gen_expr(out, n->sif.cond);
            std::string L1 = fresh_label();
            std::string L2 = fresh_label();
            std::fprintf(out, "if_false %s goto %s\n", c.c_str(), L1.c_str());
            gen_stmt(out, n->sif.then_s);
            if (n->sif.else_s) {
                std::fprintf(out, "goto %s\n", L2.c_str());
                std::fprintf(out, "%s:\n", L1.c_str());
                gen_stmt(out, n->sif.else_s);
                std::fprintf(out, "%s:\n", L2.c_str());
            } else {
                std::fprintf(out, "%s:\n", L1.c_str());
            }
            break;
        }
        case AST_WHILE: {
            std::string L0 = fresh_label();
            std::string L1 = fresh_label();
            std::fprintf(out, "%s:\n", L0.c_str());
            std::string c = gen_expr(out, n->sw.cond);
            std::fprintf(out, "if_false %s goto %s\n", c.c_str(), L1.c_str());
            gen_stmt(out, n->sw.body);
            std::fprintf(out, "goto %s\n", L0.c_str());
            std::fprintf(out, "%s:\n", L1.c_str());
            break;
        }
        case AST_RETURN: {
            std::string v = gen_expr(out, n->expr);
            std::fprintf(out, "return %s\n", v.c_str());
            break;
        }
        case AST_DECLARATION:
            for (int i = 0; i < n->declaration.icount; ++i)
                gen_stmt(out, n->declaration.inits[i]);
            break;
        case AST_INIT_DECL:
            if (n->id.init && n->id.declr && n->id.declr->type == AST_VAR && n->id.declr->varname) {
                std::string rv = gen_expr(out, n->id.init);
                emit_line(out, Quad{"copy", rv, "", n->id.declr->varname});
            }
            break;
        default:
            break;
    }
}

static void gen_ext(FILE *out, ASTNode *n) {
    if (!n)
        return;
    if (n->type == AST_FUNC_TYPE && n->ft.name) {
        std::fprintf(out, "begin_func %s\n", n->ft.name);
        gen_stmt(out, n->ft.body);
        std::fprintf(out, "end_func %s\n", n->ft.name);
    } else if (n->type == AST_DECLARATION) {
        gen_stmt(out, n);
    }
}

extern "C" void emit_ir_from_ast(FILE *out, ASTNode *root) {
    if (!root || root->type != AST_TRANSLATION_UNIT)
        return;
    std::fprintf(out, "// --- three-address (subset demo) ---\n");
    for (int i = 0; i < root->seq.count; ++i)
        gen_ext(out, root->seq.list[i]);
}
