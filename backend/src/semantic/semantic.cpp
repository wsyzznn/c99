#include "../../include/ast.h"
#include "../../include/symbol.h"
#include <stdio.h>
#include <string.h>

extern void generate_ir(ASTNode *root);

// 辅助函数：计算类型大小
int calculate_type_size(ASTNode* type_node) {
    if (type_node == nullptr) return 4; // 默认返回 4 字节

    switch (type_node->type) {
        case AST_ARRAY_TYPE: {
            // 递归获取基础类型的大小 (比如 int 占 4)
            int base_size = calculate_type_size(type_node->at.base);
            // 获取数组长度 (假设长度是个常量 AST_INT)
            int array_len = 1; 
            if (type_node->at.size && type_node->at.size->type == AST_INT) {
                array_len = type_node->at.size->intval;
            }
            return base_size * array_len; // 如: 4 * 10 = 40
        }
        case AST_POINTER_TYPE:
            return 8; // 64位系统下指针占 8 字节
            
        // 如果 ast.h 中有表示基础类型的节点（如 AST_TYPE_NAME）
        // 可以根据具体的 int / double 返回 4 或 8。
        // 这里暂时统一默认基础类型占 4 字节。
        default: 
            return 4; 
    }
}

// 语义检查核心：一边遍历，一边推导类型
CType check_semantic_internal(ASTNode* node) {
    if (!node) return TYPE_VOID;

    switch (node->type) {
        case AST_TRANSLATION_UNIT:
            for (int i = 0; i < node->seq.count; i++) 
                check_semantic_internal(node->seq.list[i]);
            return TYPE_VOID;

        case AST_COMPOUND_STMT:
            symbol_enter_scope(); 
            for (int i = 0; i < node->seq.count; i++) 
                check_semantic_internal(node->seq.list[i]);
            symbol_exit_scope();
            return TYPE_VOID;

        case AST_INT:
            return TYPE_INT;

        case AST_DOUBLE:
            return TYPE_DOUBLE;

        case AST_VAR: {
            Symbol* sym = symbol_lookup(node->varname);
            if (!sym) { 
                printf("Error at line %d: Undeclared identifier '%s'\n", node->line, node->varname);
                return TYPE_ERROR;
            }
            // 【核心修改 1】：读取符号类型时使用 c_type
            return sym->c_type;
        }

        case AST_BINOP: {
            CType left_type = check_semantic_internal(node->bin.lhs);
            CType right_type = check_semantic_internal(node->bin.rhs);
            
            if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) return TYPE_ERROR;

            // 简单的类型提升规则：如果有 double，结果就是 double
            if (left_type == TYPE_DOUBLE || right_type == TYPE_DOUBLE) {
                return TYPE_DOUBLE;
            }
            return TYPE_INT;
        }

        case AST_ASSIGN: {
            CType left_type = check_semantic_internal(node->assign.l);
            CType right_type = check_semantic_internal(node->assign.r);

            if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) return TYPE_ERROR;

            if (left_type != right_type) {
                printf("Warning at line %d: Type mismatch in assignment.\n", node->line);
            }
            return left_type;
        }

        case AST_DECLARATION: {
            // 1. 提取声明的基础类型 (根据你 c99.y 的 type_specifier，它是存在 AST_VAR 里的字符串)
            CType decl_type = TYPE_VOID; 
            if (node->declaration.scount > 0) {
                ASTNode* type_node = node->declaration.specs[0];
                if (type_node && type_node->type == AST_VAR) {
                    if (strcmp(type_node->varname, "int") == 0) decl_type = TYPE_INT;
                    else if (strcmp(type_node->varname, "float") == 0 || 
                             strcmp(type_node->varname, "double") == 0) decl_type = TYPE_DOUBLE;
                }
            }

            // 2. 遍历该行声明的所有变量 (例如：int a = 5, b;)
            for (int i = 0; i < node->declaration.icount; i++) {
                ASTNode* init_decl = node->declaration.inits[i]; // 取出 AST_INIT_DECL
                
                if (init_decl && init_decl->type == AST_INIT_DECL) {
                    ASTNode* var_node = init_decl->id.declr; // 左边的变量名节点
                    
                    if (var_node && var_node->type == AST_VAR) {
                        // 本层查重
                        if (symbol_lookup_current_scope(var_node->varname) != nullptr) {
                            printf("\033[31mError at line %d: Redefinition of variable '%s'\033[0m\n", 
                                   node->line, var_node->varname);
                        } else {
                            //登记入册
                            Symbol new_sym;
                            new_sym.name = var_node->varname;
                            // 【核心修改 2】：赋值符号类型时使用 c_type
                            new_sym.c_type = decl_type;
                            // 如果加了数组/指针的大小计算，可以在这里加上：
                            // new_sym.size = calculate_type_size(node->declaration.specs[0]);
                            symbol_insert(&new_sym);
                        }
                    }

                    // 【核心 3：检查初始化表达式的类型匹配】(如果有的话)
                    if (init_decl->id.init) {
                        CType init_expr_type = check_semantic_internal(init_decl->id.init);
                        if (init_expr_type != TYPE_ERROR && init_expr_type != decl_type) {
                             printf("\033[33mWarning at line %d: Type mismatch in initialization.\033[0m\n", node->line);
                        }
                    }
                }
            }
            return TYPE_VOID; // 声明语句本身不参与外部计算
        }
        case AST_IF:
            // 检查条件表达式是否合法
            check_semantic_internal(node->sif.cond);
            // 检查 then 和 else 分支
            check_semantic_internal(node->sif.then_s);
            if (node->sif.else_s) {
                check_semantic_internal(node->sif.else_s);
            }
            return TYPE_VOID;

        case AST_WHILE:
            check_semantic_internal(node->sw.cond);
            check_semantic_internal(node->sw.body);
            return TYPE_VOID;

        case AST_FUNC_TYPE: {
            // 将函数名登记入册 (简易处理)
            if (symbol_lookup_current_scope(node->ft.name) == nullptr) {
                Symbol func_sym;
                func_sym.name = node->ft.name;
                // 【核心修改 3】：赋值符号类型时使用 c_type 
                func_sym.c_type = TYPE_VOID; // 实际应提取返回类型
                symbol_insert(&func_sym);
            }
            // 检查函数体
            check_semantic_internal(node->ft.body);
            return TYPE_VOID;
        }

        // [新增: FOR 循环各部分的语义检查]
        case AST_FOR:
            if (node->sf.init) check_semantic_internal(node->sf.init);
            if (node->sf.cond) check_semantic_internal(node->sf.cond);
            if (node->sf.post) check_semantic_internal(node->sf.post);
            if (node->sf.body) check_semantic_internal(node->sf.body);
            return TYPE_VOID;

        // [新增: 函数调用时的声明检查与参数检查]
        case AST_CALL: {
            Symbol* sym = symbol_lookup(node->call.fname);
            if (!sym) {
                // 如果发现调用的函数没声明过，抛出 Warning 或 Error
                printf("\033[33mWarning at line %d: Implicit declaration of function '%s'\033[0m\n", 
                       node->line, node->call.fname);
            }
            // 遍历检查每个传入的参数是否存在语义错误
            for (int i = 0; i < node->call.argc; i++) {
                check_semantic_internal(node->call.args[i]);
            }
            // 假设默认返回 INT
            return TYPE_INT; 
        }
        
        // [新增: 防止遇到控制流节点返回错误]
        case AST_BREAK:
        case AST_CONTINUE:
            return TYPE_VOID;
        
        case AST_RETURN:
            if (node->expr) {
                check_semantic_internal(node->expr);
            }
            return TYPE_VOID;
        default:
            return TYPE_VOID;
    }
}

// 语义分析对外接口
void semantic_analysis(ASTNode *root) {
    symbol_table_init();
    check_semantic_internal(root);
}