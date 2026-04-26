#include "../../include/ast.h"
#include <iostream>
#include <string>

using namespace std;

// 计数器：为生成的 C 代码分配内部变量名
static int c_var_count = 0;
static string next_c_var() { return "v" + to_string(c_var_count++); }

// 前向声明
static string generate_c_expr(ASTNode* node);
static string generate_c_bool(ASTNode* node);
static void traverse_and_gen_meta_c(ASTNode* node);

// ============================================================================
// 基础设施模板：这部分文本会被完整输出到 output_temp.c 的头部
// 它包含了在 ir.cpp 里写过的 emit, makelist, backpatch 的 C 语言等价实现
// ============================================================================
const char* c_infrastructure = R"(
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Quaternary structure
typedef struct { char op[16]; char arg1[32]; char arg2[32]; char result[32]; } Quad;
Quad ir_codes[10000];
int ir_ptr = 0;
int temp_counter = 1;

char* new_temp() {
    char* t = (char*)malloc(16);
    sprintf(t, "t%d", temp_counter++);
    return t;
}

int nextinstr() { return ir_ptr; }

void emit(const char* op, const char* arg1, const char* arg2, const char* result) {
    strcpy(ir_codes[ir_ptr].op, op);
    strcpy(ir_codes[ir_ptr].arg1, arg1);
    strcpy(ir_codes[ir_ptr].arg2, arg2);
    strcpy(ir_codes[ir_ptr].result, result);
    ir_ptr++;
}

// Linked list structure is used for --backpatch--
typedef struct IntNode { int val; struct IntNode* next; } IntNode;
typedef struct { IntNode* head; IntNode* tail; } IntList;

IntList* makelist(int index) {
    IntList* list = (IntList*)malloc(sizeof(IntList));
    IntNode* node = (IntNode*)malloc(sizeof(IntNode));
    node->val = index; node->next = NULL;
    list->head = list->tail = node;
    return list;
}

IntList* merge_list(IntList* p1, IntList* p2) {
    if (!p1 || !p1->head) return p2;
    if (!p2 || !p2->head) return p1;
    p1->tail->next = p2->head;
    p1->tail = p2->tail;
    return p1;
}

void backpatch(IntList* list, int target_instr) {
    if (!list) return;
    char buf[32]; sprintf(buf, "%d", target_instr);
    IntNode* curr = list->head;
    while (curr) {
        strcpy(ir_codes[curr->val].result, buf);
        curr = curr->next;
    }
}

typedef struct { IntList* truelist; IntList* falselist; } CondOut;

void print_ir() {
    for (int i = 0; i < ir_ptr; i++) {
        Quad q = ir_codes[i];
        printf("%d:\t", i);
        if (q.op[0] == 'j') {
            if (strcmp(q.op, "j") == 0) printf("goto %s\n", q.result);
            else printf("if %s %s %s goto %s\n", q.arg1, q.op + 1, q.arg2, q.result);
        } else if (strcmp(q.op, "LABEL") == 0) {
            printf("%s:\n", q.arg1);
        } else if (strcmp(q.op, "RET") == 0) {
            printf("return %s\n", q.arg1);
        } else if (strcmp(q.op, "=") == 0) {
            printf("%s = %s\n", q.result, q.arg1);
        } else if (strcmp(q.op, "call") == 0) {
            printf("%s = call %s\n", q.result, q.arg1);
        } else {
            printf("%s = %s %s %s\n", q.result, q.arg1, q.op, q.arg2);
        }
    }
}
)";

// ============================================================================
// 元编译逻辑：以下函数生成的全是能在 C 中调用 emit 和 backpatch 的语句
// ============================================================================

static string generate_c_expr(ASTNode* node) {
    if (!node) return "\"\"";
    switch (node->type) {
        case AST_INT: return "\"" + to_string(node->intval) + "\"";
        case AST_DOUBLE: return "\"" + to_string(node->fval) + "\"";
        case AST_VAR: return "\"" + string(node->varname) + "\"";
        case AST_BINOP: {
            string left = generate_c_expr(node->bin.lhs);
            string right = generate_c_expr(node->bin.rhs);
            string res = next_c_var();
            printf("    char* %s = new_temp();\n", res.c_str());
            printf("    emit(\"%c\", %s, %s, %s);\n", node->bin.op, left.c_str(), right.c_str(), res.c_str());
            return res;
        }
        case AST_ASSIGN: {
            string right = generate_c_expr(node->assign.r);
            string left_name = node->assign.l->varname;
            printf("    emit(\"=\", %s, \"\", \"%s\");\n", right.c_str(), left_name.c_str());
            return "\"" + left_name + "\"";
        }
        case AST_CALL: {
            for (int i = 0; i < node->call.argc; i++) {
                generate_c_expr(node->call.args[i]); // 实际中需要存入参数列表
            }
            string res = next_c_var();
            printf("    char* %s = new_temp();\n", res.c_str());
            printf("    emit(\"call\", \"%s\", \"\", %s);\n", node->call.fname, res.c_str());
            return res;
        }
        default: return "\"\"";
    }
}

static string generate_c_bool(ASTNode* node) {
    string res_var = next_c_var();
    printf("    CondOut %s;\n", res_var.c_str());

    if (node->type == AST_BINOP) {
        // 1. 处理关系运算符 ( <, >, ==, <= )
        if (node->bin.op == '<' || node->bin.op == '>' || node->bin.op == 'e' || node->bin.op == 'l') {
            string left = generate_c_expr(node->bin.lhs);
            string right = generate_c_expr(node->bin.rhs);
            printf("    %s.truelist = makelist(nextinstr());\n", res_var.c_str());
            printf("    %s.falselist = makelist(nextinstr() + 1);\n", res_var.c_str());
            printf("    emit(\"j%c\", %s, %s, \"_\");\n", node->bin.op, left.c_str(), right.c_str());
            printf("    emit(\"j\", \"\", \"\", \"_\");\n");
            return res_var;
        }
        
        // 2. 处理逻辑与运算符 ( && )
        if (node->bin.op == '&') {
            string left = generate_c_bool(node->bin.lhs);
            string m_instr = next_c_var();
            printf("    int %s = nextinstr();\n", m_instr.c_str()); // 对应 M 标记
            string right = generate_c_bool(node->bin.rhs);
            
            // 左边为真时，跳转到右边表达式开始计算的位置
            printf("    backpatch(%s.truelist, %s);\n", left.c_str(), m_instr.c_str());
            // 综合属性传递
            printf("    %s.truelist = %s.truelist;\n", res_var.c_str(), right.c_str());
            printf("    %s.falselist = merge_list(%s.falselist, %s.falselist);\n", res_var.c_str(), left.c_str(), right.c_str());
            return res_var;
        }

        // 3. 处理逻辑或运算符 ( || ) -> 【新增补全的部分】
        if (node->bin.op == '|') {
            string left = generate_c_bool(node->bin.lhs);
            string m_instr = next_c_var();
            printf("    int %s = nextinstr();\n", m_instr.c_str()); // 对应 M 标记
            string right = generate_c_bool(node->bin.rhs);
            
            // 左边为假时，才跳转到右边表达式开始计算的位置 (短路原则)
            printf("    backpatch(%s.falselist, %s);\n", left.c_str(), m_instr.c_str());
            // 综合属性传递：只要有任意一个为真，整体即为真
            printf("    %s.truelist = merge_list(%s.truelist, %s.truelist);\n", res_var.c_str(), left.c_str(), right.c_str());
            // 整体为假的前提是：两边都为假 (右边的 falselist 就是整体的 falselist)
            printf("    %s.falselist = %s.falselist;\n", res_var.c_str(), right.c_str());
            return res_var;
        }
    }

    // 4. 处理单变量作为条件 (例如: if(a) )
    string val = generate_c_expr(node);
    printf("    %s.truelist = makelist(nextinstr());\n", res_var.c_str());
    printf("    %s.falselist = makelist(nextinstr() + 1);\n", res_var.c_str());
    printf("    emit(\"j!=\", %s, \"0\", \"_\");\n", val.c_str());
    printf("    emit(\"j\", \"\", \"\", \"_\");\n");
    return res_var;
}

static void traverse_and_gen_meta_c(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_INT:
        case AST_DOUBLE:
        case AST_VAR:
        case AST_BINOP:
        case AST_ASSIGN:
        case AST_CALL:
            generate_c_expr(node);
            break;

        case AST_IF: {
            printf("    // Generate -IF- zipper backfill logic\n");
            string cond = generate_c_bool(node->sif.cond);
            string m1 = next_c_var();
            printf("    int %s = nextinstr();\n", m1.c_str());
            printf("    backpatch(%s.truelist, %s);\n", cond.c_str(), m1.c_str());
            traverse_and_gen_meta_c(node->sif.then_s);

            if (node->sif.else_s) {
                string nlist = next_c_var();
                printf("    IntList* %s = makelist(nextinstr());\n", nlist.c_str());
                printf("    emit(\"j\", \"\", \"\", \"_\");\n");
                string m2 = next_c_var();
                printf("    int %s = nextinstr();\n", m2.c_str());
                printf("    backpatch(%s.falselist, %s);\n", cond.c_str(), m2.c_str());
                traverse_and_gen_meta_c(node->sif.else_s);
                printf("    backpatch(%s, nextinstr());\n", nlist.c_str());
            } else {
                printf("    backpatch(%s.falselist, nextinstr());\n", cond.c_str());
            }
            break;
        }

        case AST_WHILE: {
            printf("    // Generate -WHILE- zipper backfill logic\n");
            string m1 = next_c_var();
            printf("    int %s = nextinstr();\n", m1.c_str());
            string cond = generate_c_bool(node->sw.cond);
            string m2 = next_c_var();
            printf("    int %s = nextinstr();\n", m2.c_str());
            printf("    backpatch(%s.truelist, %s);\n", cond.c_str(), m2.c_str());
            traverse_and_gen_meta_c(node->sw.body);
            printf("    char buf_%s[16]; sprintf(buf_%s, \"%%d\", %s);\n", m1.c_str(), m1.c_str(), m1.c_str());
            printf("    emit(\"j\", \"\", \"\", buf_%s);\n", m1.c_str());
            printf("    backpatch(%s.falselist, nextinstr());\n", cond.c_str());
            break;
        }

        case AST_FUNC_TYPE:
            printf("    emit(\"LABEL\", \"%s\", \"\", \"\");\n", node->ft.name);
            traverse_and_gen_meta_c(node->ft.body);
            break;

        case AST_RETURN: {
            string ret_val = generate_c_expr(node->expr);
            printf("    emit(\"RET\", %s, \"\", \"\");\n", ret_val.c_str());
            break;
        }

        case AST_EXPR_STMT:
        case AST_COMPOUND_STMT:
        case AST_TRANSLATION_UNIT:
            for (int i = 0; i < node->seq.count; i++) traverse_and_gen_meta_c(node->seq.list[i]);
            break;
        
        default: break;
    }
}

// 供外部调用的入口函数
void generate_c_from_ast(ASTNode* root) {
    // 1. 打印包含 emit/backpatch 实现的 C 头部基础设施
    printf("%s\n", c_infrastructure);
    
    // 2. 生成 C 语言的主函数入口
    printf("int main() {\n");
    c_var_count = 0;
    
    // 3. 递归遍历 AST，将控制流翻译成一连串对 emit 和 backpatch 的硬编码调用
    traverse_and_gen_meta_c(root);
    
    // 4. 收尾：在生成的 C 程序执行完逻辑后，调用 print_ir 打印出正确的三地址码
    printf("    printf(\"\\n=== Three-address code generated by meta-compilation (Meta-Compilation) ===\\n\");\n");
    printf("    print_ir();\n");
    printf("    return 0;\n");
    printf("}\n");
}