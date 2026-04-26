#include "../../include/ast.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ---------------------------------------------------------
// [新增: 基础设施 - 四元式(Quad)与拉链回填(Backpatching)]
// ---------------------------------------------------------
struct Quad
{
    string op;
    string arg1;
    string arg2;
    string result;
};

static vector<Quad> ir_codes;
static int temp_counter = 1;
// 用于维护 break 和 continue 的拉链回填栈
static vector<vector<int>> break_stack;
static vector<vector<int>> continue_stack;

// 申请一个新的临时变量名
static string new_temp()
{
    return "t" + to_string(temp_counter++);
}

// 获取下一条指令的索引 (对应示例代码中的 nextInstr)
static int nextinstr()
{
    return ir_codes.size();
}

// 触发一条四元式指令 (对应示例代码中的 emit)
static void emit(string op, string arg1, string arg2, string result)
{
    ir_codes.push_back({op, arg1, arg2, result});
}

// 生成包含单个指令索引的列表 (对应 makelist)
static vector<int> makelist(int index)
{
    return {index};
}

// 合并两个列表 (对应 merge)
static vector<int> merge_list(vector<int> p1, vector<int> p2)
{
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}

// 拉链回填 (对应 backpatch)
static void backpatch(const vector<int> &list, int target_instr)
{
    for (int instr_idx : list)
    {
        ir_codes[instr_idx].result = to_string(target_instr);
    }
}

// 用于条件表达式返回的真假出口链表
struct CondOut
{
    vector<int> truelist;
    vector<int> falselist;
};

// ---------------------------------------------------------
// [新增: 逻辑与关系表达式的解析 (用于 IF / WHILE 条件)]
// ---------------------------------------------------------
static string traverse_and_gen_tac(ASTNode *node); // 前向声明

static CondOut gen_bool_expr(ASTNode *node)
{
    CondOut out;
    if (!node)
        return out;

    if (node->type == AST_BINOP)
    {
        // 处理关系运算符: <, >, <=, >=, ==, !=
        if (node->bin.op == '<' || node->bin.op == '>' || node->bin.op == 'e' || 
            node->bin.op == 'l' || node->bin.op == 'g' || node->bin.op == 'n')
        {
            string left_val = traverse_and_gen_tac(node->bin.lhs);
            string right_val = traverse_and_gen_tac(node->bin.rhs);

            out.truelist = makelist(nextinstr());
            out.falselist = makelist(nextinstr() + 1);

            // 将 AST 字符映射为标准的汇编跳转符
            string op_str = "j";
            if (node->bin.op == 'e') op_str += "==";
            else if (node->bin.op == 'n') op_str += "!=";
            else if (node->bin.op == 'l') op_str += "<=";
            else if (node->bin.op == 'g') op_str += ">=";
            else op_str += node->bin.op; // < 或 >

            emit(op_str, left_val, right_val, "_"); // _ 表示待回填
            emit("j", "", "", "_");
            return out;
        }

        // 处理逻辑运算符: && ('&')
        if (node->bin.op == '&')
        {
            CondOut left_cond = gen_bool_expr(node->bin.lhs);
            int m_instr = nextinstr(); // 对应示例代码里的 M
            CondOut right_cond = gen_bool_expr(node->bin.rhs);

            backpatch(left_cond.truelist, m_instr);
            out.truelist = right_cond.truelist;
            out.falselist = merge_list(left_cond.falselist, right_cond.falselist);
            return out;
        }

        // 处理逻辑运算符: || ('|')
        if (node->bin.op == '|')
        {
            CondOut left_cond = gen_bool_expr(node->bin.lhs);
            int m_instr = nextinstr();
            CondOut right_cond = gen_bool_expr(node->bin.rhs);

            backpatch(left_cond.falselist, m_instr);
            out.truelist = merge_list(left_cond.truelist, right_cond.truelist);
            out.falselist = right_cond.falselist;
            return out;
        }
    }

    // 如果是单个变量作为条件 (如 if (a) )
    string val = traverse_and_gen_tac(node);
    out.truelist = makelist(nextinstr());
    out.falselist = makelist(nextinstr() + 1);
    emit("j!=", val, "0", "_");
    emit("j", "", "", "_");
    return out;
}

// ---------------------------------------------------------
// [修改: 核心的 AST 遍历器，适配了最新的控制流与函数调用]
// ---------------------------------------------------------
static string traverse_and_gen_tac(ASTNode *node)
{
    if (!node)
        return "";

    switch (node->type)
    {
    case AST_INT:
        return to_string(node->intval);

    case AST_DOUBLE:
        return to_string(node->fval);

    case AST_VAR:
        return node->varname ? string(node->varname) : "";

    case AST_ASSIGN:
    {
        string right_val = traverse_and_gen_tac(node->assign.r);
        string left_val = (node->assign.l && node->assign.l->type == AST_VAR) ? string(node->assign.l->varname) : "unknown";
        emit("=", right_val, "", left_val);
        return left_val;
    }

    case AST_BINOP:
    {
        string left_val = traverse_and_gen_tac(node->bin.lhs);
        string right_val = traverse_and_gen_tac(node->bin.rhs);
        string result_temp = new_temp();
        string op_str(1, node->bin.op);
        emit(op_str, left_val, right_val, result_temp);
        return result_temp;
    }

    // [新增: 处理 IF 控制流与回填]
    case AST_IF:
    {
        CondOut cond = gen_bool_expr(node->sif.cond);
        int m1 = nextinstr();
        backpatch(cond.truelist, m1);
        traverse_and_gen_tac(node->sif.then_s);

        if (node->sif.else_s)
        {
            vector<int> n_list = makelist(nextinstr());
            emit("j", "", "", "_"); // 跳过 else 块

            int m2 = nextinstr();
            backpatch(cond.falselist, m2);
            traverse_and_gen_tac(node->sif.else_s);

            // then 块结束后的跳转回填至结尾
            backpatch(n_list, nextinstr());
        }
        else
        {
            backpatch(cond.falselist, nextinstr());
        }
        return "";
    }

    case AST_CALL:
    {
        // 1. 依次生成每个实参的三地址码，并保存到临时变量
        vector<string> arg_temps;
        for (int i = 0; i < node->call.argc; i++)
        {
            string arg_val = traverse_and_gen_tac(node->call.args[i]);
            arg_temps.push_back(arg_val);
        }
        // 2. 生成调用指令（假设函数返回int）
        string result_temp = new_temp();
        // 将参数列表拼成字符串（这里简化处理，实际可能需要逐个传参）
        emit("call", node->call.fname, "", result_temp);
        // 注意：真正的三地址码需要处理参数传递，你可以简化为 call func, arg1, arg2, ...
        return result_temp;
    }

    case AST_DECLARATION:
        // 声明语句不生成运行时代码，但需要记录变量已声明（供后续使用）
        // 可以什么都不做，因为变量名已经在符号表中
        return "";

    case AST_WHILE:
    {
        break_stack.push_back(vector<int>());    // 压栈
        continue_stack.push_back(vector<int>()); // 压栈

        int m1_start = nextinstr();
        CondOut cond = gen_bool_expr(node->sw.cond);

        int m2_body = nextinstr();
        backpatch(cond.truelist, m2_body);

        traverse_and_gen_tac(node->sw.body);

        emit("j", "", "", to_string(m1_start)); 
        
        backpatch(cond.falselist, nextinstr());
        
        // 统一回填 break 和 continue
        backpatch(break_stack.back(), nextinstr()); // break 跳到循环外面
        backpatch(continue_stack.back(), m1_start); // continue 跳回条件判断
        
        break_stack.pop_back();    // 出栈
        continue_stack.pop_back(); // 出栈
        return "";
    }

    case AST_FOR:
    {
        // 1. 初始化表达式 (init)
        if (node->sf.init) traverse_and_gen_tac(node->sf.init);
        
        // 2. 标记条件判断的起始位置
        int m1_cond = nextinstr();
        CondOut cond;
        if (node->sf.cond) {
            cond = gen_bool_expr(node->sf.cond);
        } else {
            // 如果没有条件 (死循环)，默认走真出口
            cond.truelist = makelist(nextinstr());
            emit("j", "", "", "_");
        }

        // 3. 标记循环体的起始位置并回填真出口
        int m2_body = nextinstr();
        backpatch(cond.truelist, m2_body);

        // 4. 生成循环体代码
        traverse_and_gen_tac(node->sf.body);

        // 5. 标记并执行步进表达式 (post)
        int m3_post = nextinstr();
        if (node->sf.post) traverse_and_gen_tac(node->sf.post);
        
        // 6. 跳回条件判断
        emit("j", "", "", to_string(m1_cond));
        
        // 7. 回填假出口 (跳出循环)
        backpatch(cond.falselist, nextinstr());
        return "";
    }

    // [新增: 处理函数定义与返回]
    case AST_FUNC_TYPE:
        emit("LABEL", node->ft.name, "", "");
        traverse_and_gen_tac(node->ft.body);
        return "";

    case AST_RETURN:
    {
        string ret_val = traverse_and_gen_tac(node->expr);
        emit("RET", ret_val, "", "");
        return "";
    }

    case AST_BREAK:
        if (!break_stack.empty()) {
            break_stack.back().push_back(nextinstr());
            emit("j", "", "", "_"); // 等待循环外回填
        }
        return "";

    case AST_CONTINUE:
        if (!continue_stack.empty()) {
            continue_stack.back().push_back(nextinstr());
            emit("j", "", "", "_"); // 等待回填到条件判断处
        }
        return "";
    
    case AST_EXPR_STMT:
    case AST_COMPOUND_STMT:
    case AST_TRANSLATION_UNIT:
        for (int i = 0; i < node->seq.count; i++)
        {
            traverse_and_gen_tac(node->seq.list[i]);
        }
        break;

    default:
        break;
    }
    return "";
}

// --- 暴露给外部的接口 ---

void generate_three_address(ASTNode *root)
{
    temp_counter = 1;
    ir_codes.clear(); // 清空历史指令

    traverse_and_gen_tac(root);

    // [修改: 集中打印所有生成完毕并回填的三地址码]
    for (size_t i = 0; i < ir_codes.size(); i++)
    {
        const Quad &q = ir_codes[i];
        cout << i << ":\t";
        if (q.op == "j")
        {
            cout << "goto " << q.result << endl;
        }
        else if (q.op == "j<" || q.op == "j>" || q.op == "je" || q.op == "j!=")
        {
            cout << "if " << q.arg1 << " " << q.op.substr(1) << " " << q.arg2 << " goto " << q.result << endl;
        }
        else if (q.op == "LABEL")
        {
            cout << q.arg1 << ":" << endl;
        }
        else if (q.op == "RET")
        {
            cout << "return " << q.arg1 << endl;
        }
        else if (q.op == "=")
        {
            cout << q.result << " = " << q.arg1 << endl;
        }
        else
        {
            cout << q.result << " = " << q.arg1 << " " << q.op << " " << q.arg2 << endl;
        }
    }
}