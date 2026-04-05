#include "ir.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* new_temp() {
	static int temp_count = 0;
	char buffer[16];
	sprintf(buffer, "t%d", temp_count++);

	return cpp_strdup(buffer);
}

char* new_label() {
	static int label_count = 0;
	char buffer[16];
	sprintf(buffer, "L%d", label_count++);

	return cpp_strdup(buffer);
}

IRInst* create_instruction(IROp op, char* arg1, char* arg2, char* result) {
	IRInst* inst = new IRInst();
	inst->op = op;
	inst->arg1 = arg1 ? cpp_strdup(arg1) : NULL;
	inst->arg2 = arg2 ? cpp_strdup(arg2) : NULL;
	inst->result = result ? cpp_strdup(result) : NULL;
	inst->next = NULL;

	return inst;
}

void append_instruction(IRInst* head, IRInst* new_inst) {
	if (!head) return;

	IRInst* curr = head;
	while (curr->next) {
		curr = curr->next;
	}
	curr->next = new_inst;
}

void print_ir_list(IRInst* head) {
	IRInst* curr = head;
	while (curr) {
		switch (curr->op) {
		case IR_ADD:
			printf("ADD %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_SUB:
			printf("SUB %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_MUL:
			printf("MUL %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_DIV:
			printf("DIV %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_MOD:
			printf("MOD %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;

		case IR_GT:
			printf("GT %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_LT:
			printf("LT %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_GTE:
			printf("GTE %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_LTE:
			printf("LTE %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_EQ:
			printf("EQ %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_NEQ:
			printf("NEQ %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;

		case IR_AND:
			printf("AND %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_OR:
			printf("OR %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_NOT:
			printf("NOT %s %s\n", curr->arg1, curr->result);
			break;

		case IR_ASSIGN:
			printf("ASSIGN %s %s\n", curr->arg1, curr->result);
			break; // ASSIGN src dest

		case IR_IFZ:
			printf("IFZ %s %s\n", curr->arg1, curr->arg2);
			break;
		case IR_GOTO:
			printf("GOTO %s\n", curr->arg1);
			break;
		case IR_LABEL:
			printf("LABEL %s\n", curr->arg1);
			break;

		case IR_FUNC_START:
			printf("\nFUNCTION %s:\n", curr->arg1);
			break;
		case IR_PARAM:
			printf("PARAM %s\n", curr->arg1);
			break;
		case IR_POP_PARAM:
			printf("POP_PARAM %s\n", curr->arg1);
			break;
		case IR_CALL:
			printf("CALL %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_RET:
			if (curr->arg1) {
				printf("RETURN %s\n", curr->arg1);
				break;
			}
			printf("RETURN\n");
			break;

		case IR_ARRAY_LOAD:
			printf("ARRAY_LOAD %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_ARRAY_STORE:
			printf("ARRAY_STORE %s %s %s\n", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_READ:
			printf("READ %s\n", curr->arg1);
			break;
		case IR_PRINT:
			printf("PRINT %s\n", curr->arg1);
			break;
		default:
			break;
		}
		curr = curr->next;
	}
}

void free_ir_list(IRInst* head) {
	IRInst* temp = NULL;
	while (head) {
		temp = head;
		head = head->next;
		if (temp->arg1) {
			delete[] temp->arg1;
		}
		if (temp->arg2) {
			delete[] temp->arg2;
		}
		if (temp->result) {
			delete[] temp->result;
		}
		delete temp;
	}
}
