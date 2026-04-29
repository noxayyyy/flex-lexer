#include <iostream>
#include <cstdlib>
#include <cstring>
#include "semantics.h"
#include "symbol_table.h"
#include "errors.h"

using namespace std;

static int semantic_errors = 0;
static char *current_func_return_type = nullptr;

void check_node(ASTNode *node);
char *get_expression_type(ASTNode *node);
int is_compatible(char *type1, char *type2);

char *sem_strdup(const char *src)
{
    if (!src)
        return nullptr;
    size_t len = strlen(src) + 1;
    char *dest = new char[len];
    strcpy(dest, src);
    return dest;
}

int semantic_analysis(ASTNode *root)
{
    semantic_errors = 0;
    current_func_return_type = nullptr;

    init_symbol_table();

    check_node(root);

    return semantic_errors;
}

void check_node(ASTNode *node)
{
    if (!node)
        return;

    ASTNode *current = node;
    while (current != nullptr)
    {

        switch (current->type)
        {
        case NODE_PROGRAM:
            check_node(current->left);
            break;

        case NODE_VAR_DECL:
        {
            SymbolKind kind = KIND_VAR;
            char *entryName = current->data;
            char *nameBuffer = nullptr;

            if (current->data && strchr(current->data, '['))
            {
                kind = KIND_ARRAY;

                char *bracketPos = strchr(current->data, '[');
                size_t len = bracketPos - current->data;
                nameBuffer = new char[len + 1];
                strncpy(nameBuffer, current->data, len);
                nameBuffer[len] = '\0';
                entryName = nameBuffer;
            }

            if (!insert_symbol(entryName, current->left->data, kind))
            {
                semantic_errors++;
                report_error(ERR_REDEF_VAR, current->line);
            }

            if (current->right)
            {
                char *exprType = get_expression_type(current->right);
                if (!is_compatible(current->left->data, exprType))
                {
                    semantic_errors++;
                    report_error(ERR_TYPE_MISMATCH_ASSIGN, current->line);
                }
                delete[] exprType;
            }

            if (nameBuffer)
                delete[] nameBuffer;

            break;
        }

        case NODE_FUNC_DECL:
        {
            ASTNode *signature = current->left;
            char *returnType = signature->left->data;

            if (!insert_symbol(current->data, returnType, KIND_FUNC))
            {
                semantic_errors++;
                report_error(ERR_REDEF_FUNC, current->line);
            }

            Symbol *funcSym = lookup_symbol(current->data);

            int pCount = 0;
            ASTNode *p = signature->right;
            while (p)
            {
                pCount++;
                p = p->next;
            }

            funcSym->param_count = pCount;
            if (pCount > 0)
            {
                funcSym->param_types = new char *[pCount];
            }
            current_func_return_type = returnType;
            push_scope();

            ASTNode *param = signature->right;
            int i = 0;
            while (param)
            {
                if (param->type == NODE_VAR_DECL)
                {
                    insert_symbol(param->data, param->left->data, KIND_PARAM);

                    if (funcSym->param_types)
                    {
                        funcSym->param_types[i] = sem_strdup(param->left->data);
                    }
                    i++;
                }
                param = param->next;
            }

            check_node(current->right);

            pop_scope();
            current_func_return_type = nullptr;
            break;
        }

        case NODE_BLOCK:
            push_scope();
            check_node(current->left);
            pop_scope();
            break;

        case NODE_ASSIGN:
            if (current->left->type == NODE_VAR_USE)
            {
                Symbol *sym = lookup_symbol(current->left->data);
                if (!sym)
                {
                    semantic_errors++;
                    report_error(ERR_UNDECLARED_VAR, current->line);
                }
                else
                {
                    if (current->left->left)
                    {
                        if (sym->kind != KIND_ARRAY)
                        {
                            semantic_errors++;
                            report_error(ERR_NOT_AN_ARRAY, current->line);
                        }
                        char *idxType = get_expression_type(current->left->left);
                        if (strcmp(idxType, "int") != 0)
                        {
                            semantic_errors++;
                            report_error(ERR_ARRAY_INDEX_TYPE, current->line);
                        }
                        delete[] idxType;
                    }
                    char *rhsType = get_expression_type(current->right);
                    if (!is_compatible(sym->type, rhsType))
                    {
                        semantic_errors++;
                        report_error(ERR_TYPE_MISMATCH_ASSIGN, current->line);
                    }
                    delete[] rhsType;
                }
            }
            break;

        case NODE_IF:
        {
            char *condType = get_expression_type(current->left);
            delete[] condType;

            check_node(current->right);
        }
        break;

        case NODE_WHILE:
        {
            char *condType = get_expression_type(current->left);
            delete[] condType;
            check_node(current->right);
        }
        break;

        case NODE_FOR:
        {
            check_node(current->left);
            check_node(current->right);
        }
        break;

        case NODE_RETURN:
            if (current_func_return_type)
            {
                const char *actualTypePtr = "void";
                char *tempType = nullptr;

                if (current->left)
                {
                    tempType = get_expression_type(current->left);
                    actualTypePtr = tempType;
                }

                if (!is_compatible(current_func_return_type, (char *)actualTypePtr))
                {
                    semantic_errors++;
                    report_error(ERR_TYPE_MISMATCH_RETURN, current->line);
                }
                if (tempType)
                    delete[] tempType;
            }
            break;

        case NODE_FUNC_CALL:
        {
            Symbol *funcSym = lookup_symbol(current->data);
            if (!funcSym)
            {
                semantic_errors++;
                report_error(ERR_UNDECLARED_FUNC, current->line);
            }
            else if (funcSym->kind != KIND_FUNC)
            {
                semantic_errors++;
                report_error(ERR_NOT_A_FUNCTION, current->line);
            }
            else
            {
                int argCount = 0;
                ASTNode *arg = current->left;

                ASTNode *temp = arg;
                while (temp)
                {
                    argCount++;
                    temp = temp->next;
                }

                if (argCount != funcSym->param_count)
                {
                    semantic_errors++;
                    report_error(ERR_FUNC_ARG_COUNT, current->line);
                }
                else
                {
                    int i = 0;
                    while (arg)
                    {
                        char *argType = get_expression_type(arg);

                        if (!is_compatible(funcSym->param_types[i], argType))
                        {
                            semantic_errors++;
                            report_error(ERR_FUNC_ARG_TYPE, current->line);
                        }

                        delete[] argType;
                        arg = arg->next;
                        i++;
                    }
                }
            }
        }
        break;

        case NODE_IO:
            if (strcmp(current->data, "read") == 0)
            {
                ASTNode *target = current->left;
                if (target->type == NODE_VAR_USE)
                {
                    Symbol *sym = lookup_symbol(target->data);
                    if (!sym)
                    {
                        semantic_errors++;
                        report_error(ERR_UNDECLARED_VAR, current->line);
                    }
                    else
                    {
                        if (target->left)
                        {
                            if (sym->kind != KIND_ARRAY)
                            {
                                semantic_errors++;
                                report_error(ERR_NOT_AN_ARRAY, current->line);
                            }
                            char *idxType = get_expression_type(target->left);
                            if (strcmp(idxType, "int") != 0)
                            {
                                semantic_errors++;
                                report_error(ERR_ARRAY_INDEX_TYPE, current->line);
                            }
                            delete[] idxType;
                        }
                    }
                }
            }
            else
            {
                char *type = get_expression_type(current->left);
                delete[] type;
            }
            break;

        case NODE_UNARY_OP:
        {
            char *type = get_expression_type(current->left);

            if (strcmp(current->data, "!") == 0)
            {
                if (strcmp(type, "bool") != 0)
                {
                    semantic_errors++;
                    report_error(ERR_TYPE_MISMATCH_OP, current->line);
                }
            }
            else
            {
                if (strcmp(type, "int") != 0 && strcmp(type, "float") != 0)
                {
                    semantic_errors++;
                    report_error(ERR_TYPE_MISMATCH_OP, current->line);
                }
            }
            delete[] type;
        }
        break;

        case NODE_BIN_OP:
        {
            char *type = get_expression_type(current);
            delete[] type;
        }
        break;

        default:
            check_node(current->left);
            check_node(current->right);
            break;
        }

        current = current->next;
    }
}

char *get_expression_type(ASTNode *node)
{
    if (!node)
        return sem_strdup("void");

    switch (node->type)
    {
    case NODE_LITERAL:
        if (strcmp(node->data, "true") == 0 || strcmp(node->data, "false") == 0)
            return sem_strdup("bool");
        if (strchr(node->data, '.'))
            return sem_strdup("float");
        return sem_strdup("int");

    case NODE_VAR_USE:
    {
        Symbol *sym = lookup_symbol(node->data);
        if (sym)
        {
            if (node->left)
            {
                if (sym->kind != KIND_ARRAY)
                {
                    semantic_errors++;
                    report_error(ERR_NOT_AN_ARRAY, node->line);
                }

                char *indexType = get_expression_type(node->left);
                if (strcmp(indexType, "int") != 0)
                {
                    semantic_errors++;
                    report_error(ERR_ARRAY_INDEX_TYPE, node->line);
                }
                delete[] indexType;
            }
            return sem_strdup(sym->type);
        }
        semantic_errors++;
        report_error(ERR_UNDECLARED_VAR, node->line);
        return sem_strdup("unknown");
    }

    case NODE_FUNC_CALL:
    {
        Symbol *sym = lookup_symbol(node->data);
        if (sym)
            return sem_strdup(sym->type);

        return sem_strdup("unknown");
    }

    case NODE_UNARY_OP:
    {
        char *type = get_expression_type(node->left);
        return type;
    }

    case NODE_BIN_OP:
    {
        char *leftT = get_expression_type(node->left);
        char *rightT = get_expression_type(node->right);
        char *result = sem_strdup("unknown");

        if (strcmp(node->data, "+") == 0 || strcmp(node->data, "-") == 0 ||
            strcmp(node->data, "*") == 0 || strcmp(node->data, "/") == 0 ||
            strcmp(node->data, "%") == 0)
        {
            int isLeftNumeric = (strcmp(leftT, "int") == 0 || strcmp(leftT, "float") == 0);
            int isRightNumeric = (strcmp(rightT, "int") == 0 || strcmp(rightT, "float") == 0);

            if (!isLeftNumeric || !isRightNumeric || strcmp(leftT, rightT) != 0) {
                semantic_errors++;
                report_error(ERR_TYPE_MISMATCH_OP, node->line);
            }
            else {
                delete[] result;
                result = sem_strdup(leftT); 
            }
        }
        
        else if (strcmp(node->data, ">") == 0 || strcmp(node->data, "<") == 0 ||
                 strcmp(node->data, "==") == 0 || strcmp(node->data, "!=") == 0 ||
                 strcmp(node->data, "<=") == 0 || strcmp(node->data, ">=") == 0)
        {
            if (strcmp(leftT, rightT) != 0) {
                semantic_errors++;
                report_error(ERR_TYPE_MISMATCH_OP, node->line);
            }
            else {
                if (strcmp(node->data, "==") != 0 && strcmp(node->data, "!=") != 0) {
                    int isNumeric = (strcmp(leftT, "int") == 0 || strcmp(leftT, "float") == 0);
                    if (!isNumeric) {
                        semantic_errors++;
                        report_error(ERR_TYPE_MISMATCH_OP, node->line);
                    }
                }
            }

            delete[] result;
            result = sem_strdup("bool");
        }
        
        else if (strcmp(node->data, "&&") == 0 || strcmp(node->data, "||") == 0)
        {
            if (strcmp(leftT, "bool") != 0 || strcmp(rightT, "bool") != 0) {
                semantic_errors++;
                report_error(ERR_TYPE_MISMATCH_OP, node->line);
            }
            delete[] result;
            result = sem_strdup("bool");
        }

        delete[] leftT;
        delete[] rightT;
        return result;
    }

    default:
        return sem_strdup("unknown");
    }
}

int is_compatible(char *targetType, char *expressionType)
{
    if (strcmp(expressionType, "unknown") == 0)
        return 1;

    if (strcmp(targetType, expressionType) == 0)
        return 1;

    return 0;
}