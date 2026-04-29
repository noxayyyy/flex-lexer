#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ir.h"

static int temp_counter = 0;
static int label_counter = 0;

using namespace std;

char* ir_strdup(const char* src) {
    if (!src) return nullptr;
    size_t len = strlen(src) + 1;
    char* dest = new char[len];
    strcpy(dest, src);
    return dest;
}

char *new_temp()
{
    char buffer[16];
    sprintf(buffer, "t%d", temp_counter);
    temp_counter++;
    return ir_strdup(buffer);
}

char *new_label()
{
    char buffer[16];
    sprintf(buffer, "L%d", label_counter);
    label_counter++;
    return ir_strdup(buffer);
}

IRInst *create_instruction(IROp op, char *arg1, char *arg2, char *result)
{
    IRInst *new_inst = new IRInst();
    new_inst->op = op;
    new_inst->arg1 = arg1 ? ir_strdup(arg1) : nullptr;
    new_inst->arg2 = arg2 ? ir_strdup(arg2) : nullptr;
    new_inst->result = result ? ir_strdup(result) : nullptr;
    new_inst->next = nullptr;
    return new_inst;
}

void append_instruction(IRInst *head, IRInst *new_inst)
{
    if (!head)
        return;
    IRInst *current = head;
    while (current->next)
    {
        current = current->next;
    }
    current->next = new_inst;
}

void free_ir_list(IRInst *head)
{
    IRInst *current = head;
    while (current != nullptr)
    {
        IRInst *temp = current;
        current = current->next;

        if (temp->arg1) delete[] temp->arg1;
        if (temp->arg2) delete[] temp->arg2;
        if (temp->result) delete[] temp->result;
        delete temp;
    }
}

void print_ir_list(IRInst *head)
{
    IRInst *current = head;
    while (current)
    {
        switch (current->op)
        {
        case IR_ADD:
            cout << "ADD " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_SUB:
            cout << "SUB " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_MUL:
            cout << "MUL " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_DIV:
            cout << "DIV " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_MOD:
            cout << "MOD " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;

        case IR_GT:
            cout << "GT " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_LT:
            cout << "LT " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_GTE:
            cout << "GTE " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_LTE:
            cout << "LTE " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_EQ:
            cout << "EQ " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_NEQ:
            cout << "NEQ " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;

        case IR_AND:
            cout << "AND " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_OR:
            cout << "OR " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_NOT:
            cout << "NOT " << current->arg1 << " " << current->result << endl;
            break;

        case IR_ASSIGN:
            cout << "ASSIGN " << current->arg1 << " " << current->result << endl;
            break;

        case IR_IFZ:
            cout << "IFZ " << current->arg1 << " " << current->result << endl;
            break;
        case IR_GOTO:
            cout << "GOTO " << current->result << endl;
            break;
        case IR_LABEL:
            cout << "LABEL " << current->result << endl;
            break;

        case IR_FUNC_START:
            cout << "\nFUNCTION " << current->result << ":" << endl;
            break;
        case IR_PARAM:
            cout << "PARAM " << current->arg1 << endl;
            break;
        case IR_CALL:
            cout << "CALL " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_RET:
            cout << "RETURN " << (current->result ? current->result : "") << endl;
            break;

        case IR_ARRAY_LOAD:
            cout << "LOAD " << current->arg1 << " " << current->arg2 << " " << current->result << endl;
            break;
        case IR_ARRAY_STORE:
            cout << "STORE " << current->result << " " << current->arg1 << " " << current->arg2 << endl;
            break;

        case IR_READ:
            cout << "READ " << current->result << endl;
            break;
        case IR_PRINT:
            cout << "PRINT " << current->arg1 << endl;
            break;
        
        case IR_POP_PARAM:
            cout << "POP_PARAM " << current->result << endl;
            break;

        default:
            cout << "UNKNOWN_OP" << endl;
            break;
        }
        current = current->next;
    }
}