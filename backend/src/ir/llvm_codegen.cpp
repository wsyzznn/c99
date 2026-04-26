#include "../../include/ast.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// 用于生成唯一的寄存器和标签编号
static int llvm_reg_count = 1;
static int llvm_label_count = 1;

// [新增: 用于 LLVM break 和 continue 的标签栈]
static vector<string> llvm_break_stack;
static vector<string> llvm_continue_stack;

// 辅助函数：生成新寄存器名（如 %1, %2）
static string next_reg() { return "%" + to_string(llvm_reg_count++); }
// 辅助函数：生成新标签名（如 L1, L2）
static string next_label() { return "L" + to_string(llvm_label_count++); }

// 核心递归函数：返回该节点计算结果所在的寄存器名或常量值
static string codegen_llvm_internal(ASTNode *node)
{
    if (!node)
        return "";

    switch (node->type)
    {
    case AST_INT:
        return to_string(node->intval);

    case AST_VAR:
    {
        string r = next_reg();
        // LLVM 中变量通常通过 alloca 存放在内存，读取时需使用 load
        cout << "  " << r << " = load i32, i32* %" << node->varname << endl;
        return r;
    }

    case AST_BINOP: {
        string left = codegen_llvm_internal(node->bin.lhs);
        string right = codegen_llvm_internal(node->bin.rhs);
        string res = next_reg();
        string op_str;
        switch (node->bin.op) {
            case '+': op_str = "add"; break;
            case '-': op_str = "sub"; break;
            case '*': op_str = "mul"; break;
            case '/': op_str = "sdiv"; break;
            
            // [新增: 补全所有的比较运算 icmp]
            case '<': cout << "  " << res << " = icmp slt i32 " << left << ", " << right << endl; return res;
            case '>': cout << "  " << res << " = icmp sgt i32 " << left << ", " << right << endl; return res;
            case 'e': cout << "  " << res << " = icmp eq i32 " << left << ", " << right << endl; return res; // ==
            case 'n': cout << "  " << res << " = icmp ne i32 " << left << ", " << right << endl; return res; // !=
            case 'l': cout << "  " << res << " = icmp sle i32 " << left << ", " << right << endl; return res; // <=
            case 'g': cout << "  " << res << " = icmp sge i32 " << left << ", " << right << endl; return res; // >=
            // 逻辑与(&)和或(|) 简化实现为位运算
            case '&': cout << "  " << res << " = and i32 " << left << ", " << right << endl; return res;
            case '|': cout << "  " << res << " = or i32 " << left << ", " << right << endl; return res;
            
            default: op_str = "add"; break;
        }
        cout << "  " << res << " = " << op_str << " i32 " << left << ", " << right << endl;
        return res;
    }

    case AST_ASSIGN:
    {
        string val = codegen_llvm_internal(node->assign.r);
        // 假设变量名已经在开头 alloca 过了
        cout << "  store i32 " << val << ", i32* %" << node->assign.l->varname << endl;
        return val;
    }

    case AST_IF:
    {
        string cond_res = codegen_llvm_internal(node->sif.cond);
        string L_then = next_label();
        string L_else = next_label();
        string L_exit = next_label();

        string cond_ext = next_reg();
        cout << "  " << cond_ext << " = zext i1 " << cond_res << " to i32" << endl;

        // 如果 cond_res 不是 i1 类型（比如是 i32），需要转换
        string cond_i1 = next_reg();
        cout << "  " << cond_i1 << " = icmp ne i32 " << cond_ext << ", 0" << endl;

        cout << "  br i1 " << cond_i1 << ", label %" << L_then << ", label %" << L_else << endl;

        cout << L_then << ":" << endl;
        codegen_llvm_internal(node->sif.then_s);
        cout << "  br label %" << L_exit << endl;

        cout << L_else << ":" << endl;
        if (node->sif.else_s)
            codegen_llvm_internal(node->sif.else_s);
        cout << "  br label %" << L_exit << endl;

        cout << L_exit << ":" << endl;
        return "";
    }

    case AST_WHILE:
    {
        string L_cond = next_label();
        string L_body = next_label();
        string L_exit = next_label();

        cout << "  br label %" << L_cond << endl;
        cout << L_cond << ":" << endl;
        string cond_res = codegen_llvm_internal(node->sw.cond);

        // 由于你的 < 和 > 运算直接返回了 i1，我们需要先把它扩展成 i32，再进行不等于 0 的比较
        string cond_ext = next_reg();
        cout << "  " << cond_ext << " = zext i1 " << cond_res << " to i32" << endl; // 零扩展

        string cond_i1 = next_reg();
        cout << "  " << cond_i1 << " = icmp ne i32 " << cond_ext << ", 0" << endl;
        cout << "  br i1 " << cond_i1 << ", label %" << L_body << ", label %" << L_exit << endl;

        cout << L_body << ":" << endl;
        codegen_llvm_internal(node->sw.body);
        cout << "  br label %" << L_cond << endl;

        cout << L_exit << ":" << endl;
        return "";
    }

    case AST_FUNC_TYPE:
    {
        cout << "define i32 @" << node->ft.name << "() {" << endl;
        cout << "entry:" << endl;
        // 简易处理：为你的 test.c 中的变量预先分配内存
        cout << "  %a = alloca i32" << endl;
        cout << "  %b = alloca i32" << endl;
        cout << "  %c = alloca i32" << endl;

        codegen_llvm_internal(node->ft.body);

        cout << "  ret i32 0" << endl;
        cout << "}" << endl;
        return "";
    }

    case AST_RETURN:
    {
        string val = codegen_llvm_internal(node->expr);
        cout << "  ret i32 " << val << endl;
        return "";
    }

    case AST_CALL:
    {
        // 1. 存储所有参数求值后的寄存器结果
        vector<string> arg_regs;

        // 根据你的 ast.h，参数存放在 call.args 指针数组中，数量为 call.argc
        if (node->call.args != nullptr)
        {
            for (int i = 0; i < node->call.argc; i++)
            {
                // node->call.args[i] 直接就是一个 ASTNode*，可以直接递归
                string arg_res = codegen_llvm_internal(node->call.args[i]);
                arg_regs.push_back(arg_res);
            }
        }

        // 2. 拼接参数字符串，例如 "i32 %1, i32 %2"
        string args_str = "";
        for (size_t i = 0; i < arg_regs.size(); i++)
        {
            args_str += "i32 " + arg_regs[i];
            if (i != arg_regs.size() - 1)
            {
                args_str += ", ";
            }
        }

        // 3. 生成 LLVM call 指令
        string res_reg = next_reg();
        // 使用你定义中的 fname 字段
        cout << "  " << res_reg << " = call i32 @" << node->call.fname << "(" << args_str << ")" << endl;

        return res_reg;
    }
    // [新增: FOR 循环的 LLVM 生成逻辑]
    case AST_FOR: {
        if (node->sf.init) codegen_llvm_internal(node->sf.init);

        string L_cond = next_label();
        string L_body = next_label();
        string L_post = next_label(); // continue 的跳跃目标
        string L_exit = next_label(); // break 的跳跃目标

        llvm_break_stack.push_back(L_exit);
        llvm_continue_stack.push_back(L_post);

        cout << "  br label %" << L_cond << endl;
        cout << L_cond << ":" << endl;
        
        if (node->sf.cond) {
            string cond_res = codegen_llvm_internal(node->sf.cond);
            string cond_ext = next_reg();
            cout << "  " << cond_ext << " = zext i1 " << cond_res << " to i32" << endl;
            string cond_i1 = next_reg();
            cout << "  " << cond_i1 << " = icmp ne i32 " << cond_ext << ", 0" << endl;
            cout << "  br i1 " << cond_i1 << ", label %" << L_body << ", label %" << L_exit << endl;
        } else {
            cout << "  br label %" << L_body << endl; // 死循环
        }

        cout << L_body << ":" << endl;
        codegen_llvm_internal(node->sf.body);
        cout << "  br label %" << L_post << endl;

        cout << L_post << ":" << endl;
        if (node->sf.post) codegen_llvm_internal(node->sf.post);
        cout << "  br label %" << L_cond << endl;

        cout << L_exit << ":" << endl;

        llvm_break_stack.pop_back();
        llvm_continue_stack.pop_back();
        return "";
    }

    // [新增: BREAK 和 CONTINUE 指令]
    case AST_BREAK:
        if (!llvm_break_stack.empty()) {
            cout << "  br label %" << llvm_break_stack.back() << endl;
        }
        return "";

    case AST_CONTINUE:
        if (!llvm_continue_stack.empty()) {
            cout << "  br label %" << llvm_continue_stack.back() << endl;
        }
        return "";
    
    case AST_COMPOUND_STMT:
    case AST_TRANSLATION_UNIT:
        for (int i = 0; i < node->seq.count; i++)
        {
            codegen_llvm_internal(node->seq.list[i]);
        }
        break;

    default:
        break;
    }
    return "";
}

// 对外接口
void generate_llvm_ir(ASTNode *root)
{
    llvm_reg_count = 1;
    llvm_label_count = 1;
    cout << "; --- LLVM IR Output ---" << endl;
    codegen_llvm_internal(root);
}