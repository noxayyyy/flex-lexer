#include "ir_generator.h"
#include "ir.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static IRInst* ir_head = NULL;

static void emit(IRInst* inst) {
	if (!ir_head) {
		ir_head = inst;
		return;
	}
	append_instruction(ir_head, inst);
}

char* generate_expression(ASTNode* node) {
	if (!node) return NULL;

	switch (node->type) {

	case NODE_LITERAL:
		return cpp_strdup(node->data);
	case NODE_VAR_USE: {
		if (!node->left) {
			return cpp_strdup(node->data);
		}
		char* idx = generate_expression(node->left);
		char* temp = new_temp();
		emit(create_instruction(IR_ARRAY_LOAD, node->data, idx, temp));

		return temp;
	}
	case NODE_BIN_OP: {
		char* left_arg = generate_expression(node->left);
		char* right_arg = generate_expression(node->right);
		char* result = new_temp();
		IROp op = IR_ADD;

		if (!strcmp(node->data, "+")) {
			op = IR_ADD;
		} else if (!strcmp(node->data, "-")) {
			op = IR_SUB;
		} else if (!strcmp(node->data, "*")) {
			op = IR_MUL;
		} else if (!strcmp(node->data, "/")) {
			op = IR_DIV;
		} else if (!strcmp(node->data, "%")) {
			op = IR_MOD;
		} else if (!strcmp(node->data, ">")) {
			op = IR_GT;
		} else if (!strcmp(node->data, "<")) {
			op = IR_LT;
		} else if (!strcmp(node->data, ">=")) {
			op = IR_GTE;
		} else if (!strcmp(node->data, "<=")) {
			op = IR_LTE;
		} else if (!strcmp(node->data, "==")) {
			op = IR_EQ;
		} else if (!strcmp(node->data, "!=")) {
			op = IR_NEQ;
		} else if (!strcmp(node->data, "&&")) {
			op = IR_AND;
		} else if (!strcmp(node->data, "||")) {
			op = IR_OR;
		}

		emit(create_instruction(op, left_arg, right_arg, result));

		return result;
	}
	case NODE_UNARY_OP: {
		char* arg = generate_expression(node->left);
		char* temp = new_temp();
		if (!strcmp(node->data, "-")) {
			emit(create_instruction(IR_SUB, (char*)"0", arg, temp));
		} else if (!strcmp(node->data, "!")) {
			emit(create_instruction(IR_NOT, arg, NULL, temp));
		} else if (!strcmp(node->data, "++") || !strcmp(node->data, "POST++")) {
			emit(create_instruction(IR_ADD, arg, (char*)"1", temp));
			emit(create_instruction(IR_ASSIGN, temp, NULL, arg));

			return arg;
		} else if (!strcmp(node->data, "--") || !strcmp(node->data, "POST--")) {
			emit(create_instruction(IR_SUB, arg, (char*)"1", temp));
			emit(create_instruction(IR_ASSIGN, temp, NULL, arg));

			return arg;
		}

		return temp;
	}
	case NODE_FUNC_CALL: {
		int count = 0;
		ASTNode* arg = node->left;
		while (arg) {
			count++;
			arg = arg->next;
		}

		char** arg_temps = new char*[count];
		arg = node->left;
		for (int i = 0; i < count; i++) {
			arg_temps[i] = generate_expression(arg);
			arg = arg->next;
		}
		for (int j = 0; j < count; j++) {
			emit(create_instruction(IR_PARAM, arg_temps[j], NULL, NULL));
			delete[] arg_temps[j];
		}
		delete[] arg_temps;

		char* temp = new_temp();
		char count_str[10];
		sprintf(count_str, "%d", count);
		emit(create_instruction(IR_CALL, node->data, count_str, temp));

		return temp;
	}
	default:
		return NULL;
	}
}

void generate_statement(ASTNode* node) {
	if (!node) return;

	switch (node->type) {
	case NODE_BLOCK: {
		ASTNode* curr = node->left;
		while (curr) {
			generate_statement(curr);
			curr = curr->next;
		}

		break;
	}
	case NODE_VAR_DECL: {
		if (!node->right) break;

		char* arg = generate_expression(node->right);
		char* base_name = cpp_strdup(node->data);
		char* bracket = strchr(base_name, '[');
		if (bracket) *bracket = '\0';
		emit(create_instruction(IR_ASSIGN, arg, NULL, base_name));
		delete[] base_name;

		break;
	}
	case NODE_ASSIGN: {
		char* rhs = generate_expression(node->right);
		if (!node->left->left) {
			emit(create_instruction(IR_ASSIGN, rhs, NULL, node->left->data));
			break;
		}
		char* idx = generate_expression(node->left->left);
		emit(create_instruction(IR_ARRAY_STORE, rhs, idx, node->left->data));

		break;
	}
	case NODE_IF: {
		if (strcmp(node->data, "if-else")) {
			char* condition = generate_expression(node->left);
			char* l_end = new_label();
			emit(create_instruction(IR_IFZ, condition, l_end, NULL));
			generate_statement(node->right);
			emit(create_instruction(IR_LABEL, l_end, NULL, NULL));

			break;
		}
		char* cond = generate_expression(node->left);
		char* l_else = new_label();
		char* l_end = new_label();
		emit(create_instruction(IR_IFZ, cond, l_else, NULL));

		generate_statement(node->right->left);
		emit(create_instruction(IR_GOTO, l_end, NULL, NULL));

		emit(create_instruction(IR_LABEL, l_else, NULL, NULL));
		generate_statement(node->right->right);

		emit(create_instruction(IR_LABEL, l_end, NULL, NULL));

		break;
	}
	case NODE_WHILE: {
		char* l_start = new_label();
		char* l_end = new_label();
		emit(create_instruction(IR_LABEL, l_start, NULL, NULL));

		char* condition = generate_expression(node->left);
		emit(create_instruction(IR_IFZ, condition, l_end, NULL));
		generate_statement(node->right);

		emit(create_instruction(IR_GOTO, l_start, NULL, NULL));
		emit(create_instruction(IR_LABEL, l_end, NULL, NULL));

		break;
	}
	case NODE_FOR: {
		generate_statement(node->left);
		char* l_start = new_label();
		char* l_end = new_label();
		emit(create_instruction(IR_LABEL, l_start, NULL, NULL));

		ASTNode* rest = node->right;
		if (rest->left) {
			char* cond = generate_expression(rest->left);
			emit(create_instruction(IR_IFZ, cond, l_end, NULL));
		}

		ASTNode* scope = rest->right;
		generate_statement(scope->right);
		generate_statement(scope->left);

		emit(create_instruction(IR_GOTO, l_start, NULL, NULL));
		emit(create_instruction(IR_LABEL, l_end, NULL, NULL));

		break;
	}
	case NODE_FUNC_DECL: {
		emit(create_instruction(IR_FUNC_START, node->data, NULL, NULL));

		// Reverse stack-popping parameters logic
		ASTNode* sig = node->left;
		ASTNode* param = sig->right;

		int param_count = 0;
		ASTNode* temp = param;
		while (temp) {
			param_count++;
			temp = temp->next;
		}

		char** params = new char*[param_count];
		int i = 0;
		temp = param;
		while (temp) {
			char* base_name = cpp_strdup(temp->data);
			char* bracket = strchr(base_name, '[');
			if (bracket) *bracket = '\0';
			params[i++] = base_name;
			temp = temp->next;
		}

		for (int j = param_count - 1; j >= 0; j--) {
			emit(create_instruction(IR_POP_PARAM, params[j], NULL, NULL));
			delete[] params[j];
		}
		delete[] params;

		generate_statement(node->right);

		break;
	}
	case NODE_RETURN: {
		if (node->left) {
			char* arg = generate_expression(node->left);
			emit(create_instruction(IR_RET, arg, NULL, NULL));
		} else {
			emit(create_instruction(IR_RET, NULL, NULL, NULL));
		}

		break;
	}
	case NODE_IO: {
		if (strcmp(node->data, "print") == 0) {
			char* arg = generate_expression(node->left);
			emit(create_instruction(IR_PRINT, arg, NULL, NULL));
		} else if (strcmp(node->data, "read") == 0) {
			if (node->left->left) {
				char* temp = new_temp();
				emit(create_instruction(IR_READ, temp, NULL, NULL));
				char* idx = generate_expression(node->left->left);
				emit(create_instruction(IR_ARRAY_STORE, temp, idx, node->left->data));
			} else {
				emit(create_instruction(IR_READ, node->left->data, NULL, NULL));
			}
		}

		break;
	}
	case NODE_FUNC_CALL: {
		char* temp = generate_expression(node);
		delete[] temp;

		break;
	}
	default:
		break;
	}
}

IRInst* generate_ir_from_ast(ASTNode* root) {
	ir_head = NULL;
	if (!root || root->type != NODE_PROGRAM) {
		return ir_head;
	}

	ASTNode* curr = root->left;
	while (curr) {
		generate_statement(curr);
		curr = curr->next;
	}

	return ir_head;
}
