#include "ir_generator.h"
#include "ast.h"
#include "ir.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

char* gen_strdup(const char* src) {
	if (!src) return nullptr;
	size_t len = strlen(src) + 1;
	char* dest = new char[len];
	strcpy(dest, src);
	return dest;
}

static IRInst* ir_head = nullptr;

void emit(IRInst* inst) {
	if (ir_head == nullptr) {
		ir_head = inst;
	} else {
		append_instruction(ir_head, inst);
	}
}

char* generate_ir(ASTNode* node);

char* generate_ir(ASTNode* node) {
	if (!node) return nullptr;

	switch (node->type) {
	case NODE_PROGRAM:
	case NODE_BLOCK: {
		ASTNode* current = (node->type == NODE_PROGRAM) ? node->left : node;

		if (node->type == NODE_BLOCK && node->left && strcmp(node->data, "{}") == 0) {
			current = node->left;
		} else if (node->type == NODE_BLOCK) {
		}

		while (current) {
			generate_ir(current);
			current = current->next;
		}
	}
		return nullptr;

	case NODE_VAR_DECL:
		if (node->right) {
			char* val_temp = generate_ir(node->right);
			emit(create_instruction(IR_ASSIGN, val_temp, nullptr, node->data));
		}
		return nullptr;

	case NODE_ASSIGN: {
		char* rhs_temp = generate_ir(node->right);

		if (node->left->type == NODE_VAR_USE && node->left->left) {
			char* index_temp = generate_ir(node->left->left);
			emit(create_instruction(IR_ARRAY_STORE, index_temp, rhs_temp, node->left->data));
			if (index_temp) delete[] index_temp;
			if (rhs_temp) delete[] rhs_temp;
			return gen_strdup(node->left->data);
		} else {
			emit(create_instruction(IR_ASSIGN, rhs_temp, nullptr, node->left->data));
			if (rhs_temp) delete[] rhs_temp;
			return gen_strdup(node->left->data);
		}
	}

	case NODE_IO: {
		if (strcmp(node->data, "read") == 0) {
			ASTNode* target = node->left;

			if (target->type == NODE_VAR_USE && target->left) {
				char* index_temp = generate_ir(target->left);
				char* read_temp = new_temp();

				emit(create_instruction(IR_READ, nullptr, nullptr, read_temp));
				emit(create_instruction(IR_ARRAY_STORE, index_temp, read_temp, target->data));

				if (index_temp) delete[] index_temp;
			} else {
				emit(create_instruction(IR_READ, nullptr, nullptr, target->data));
			}
		} else if (strcmp(node->data, "print") == 0) {
			char* expr_temp = generate_ir(node->left);
			emit(create_instruction(IR_PRINT, expr_temp, nullptr, nullptr));
			if (expr_temp) delete[] expr_temp;
		}
		return nullptr;
	}

	case NODE_BIN_OP: {
		char* t1 = generate_ir(node->left);
		char* t2 = generate_ir(node->right);
		char* result = new_temp();

		IROp op = IR_ADD;
		if (strcmp(node->data, "+") == 0)
			op = IR_ADD;
		else if (strcmp(node->data, "-") == 0)
			op = IR_SUB;
		else if (strcmp(node->data, "*") == 0)
			op = IR_MUL;
		else if (strcmp(node->data, "/") == 0)
			op = IR_DIV;
		else if (strcmp(node->data, "%") == 0)
			op = IR_MOD;
		else if (strcmp(node->data, ">") == 0)
			op = IR_GT;
		else if (strcmp(node->data, "<") == 0)
			op = IR_LT;
		else if (strcmp(node->data, ">=") == 0)
			op = IR_GTE;
		else if (strcmp(node->data, "<=") == 0)
			op = IR_LTE;
		else if (strcmp(node->data, "==") == 0)
			op = IR_EQ;
		else if (strcmp(node->data, "!=") == 0)
			op = IR_NEQ;
		else if (strcmp(node->data, "&&") == 0)
			op = IR_AND;
		else if (strcmp(node->data, "||") == 0)
			op = IR_OR;

		emit(create_instruction(op, t1, t2, result));

		if (t1) delete[] t1;
		if (t2) delete[] t2;
		return result;
	}

	case NODE_LITERAL:
		return gen_strdup(node->data);

	case NODE_VAR_USE:
		if (node->left) {
			char* index_temp = generate_ir(node->left);
			char* result = new_temp();
			emit(create_instruction(IR_ARRAY_LOAD, node->data, index_temp, result));
			if (index_temp) delete[] index_temp;
			return result;
		}
		return gen_strdup(node->data);

	case NODE_IF: {
		char* cond_temp = generate_ir(node->left);
		char* L_end = new_label();
		ASTNode* body = node->right;

		if (body && body->type == NODE_BLOCK && strcmp(body->data, "IfElseBranches") == 0) {
			ASTNode* then_block = body->left;
			ASTNode* else_block = body->right;
			char* L_else = new_label();

			emit(create_instruction(IR_IFZ, cond_temp, nullptr, L_else));

			generate_ir(then_block);
			emit(create_instruction(IR_GOTO, nullptr, nullptr, L_end));

			emit(create_instruction(IR_LABEL, nullptr, nullptr, L_else));
			generate_ir(else_block);

			if (L_else) delete[] L_else;
		} else {
			emit(create_instruction(IR_IFZ, cond_temp, nullptr, L_end));
			generate_ir(body);
		}

		emit(create_instruction(IR_LABEL, nullptr, nullptr, L_end));

		if (cond_temp) delete[] cond_temp;
		if (L_end) delete[] L_end;
		return nullptr;
	}

	case NODE_UNARY_OP: {
		int is_incr = (strstr(node->data, "++") != nullptr);
		int is_decr = (strstr(node->data, "--") != nullptr);

		if (is_incr || is_decr) {
			ASTNode* target = node->left;
			char* target_name = target->data;

			char* old_val_temp = generate_ir(target);
			char* new_val_temp = new_temp();

			IROp op = is_incr ? IR_ADD : IR_SUB;
			emit(create_instruction(op, old_val_temp, (char*)"1", new_val_temp));

			if (target->type == NODE_VAR_USE && target->left) {
				char* index_temp = generate_ir(target->left);
				emit(create_instruction(IR_ARRAY_STORE, index_temp, new_val_temp, target_name));
				if (index_temp) delete[] index_temp;
			} else {
				emit(create_instruction(IR_ASSIGN, new_val_temp, nullptr, target_name));
			}

			if (strncmp(node->data, "POST", 4) == 0) {
				if (new_val_temp) delete[] new_val_temp;
				return old_val_temp;
			} else {
				if (old_val_temp) delete[] old_val_temp;
				return new_val_temp;
			}
		} else if (strcmp(node->data, "!") == 0) {
			char* operand = generate_ir(node->left);
			char* result = new_temp();
			emit(create_instruction(IR_NOT, operand, nullptr, result));
			if (operand) delete[] operand;
			return result;
		} else if (strcmp(node->data, "-") == 0) {
			char* operand = generate_ir(node->left);
			char* result = new_temp();
			emit(create_instruction(IR_SUB, (char*)"0", operand, result));
			if (operand) delete[] operand;
			return result;
		}

		return nullptr;
	}

	case NODE_WHILE: {
		char* L_start = new_label();
		char* L_end = new_label();

		emit(create_instruction(IR_LABEL, nullptr, nullptr, L_start));
		char* cond_temp = generate_ir(node->left);
		emit(create_instruction(IR_IFZ, cond_temp, nullptr, L_end));

		generate_ir(node->right);

		emit(create_instruction(IR_GOTO, nullptr, nullptr, L_start));
		emit(create_instruction(IR_LABEL, nullptr, nullptr, L_end));

		if (L_start) delete[] L_start;
		if (L_end) delete[] L_end;
		if (cond_temp) delete[] cond_temp;
		return nullptr;
	}

	case NODE_FOR: {
		ASTNode* init = node->left;
		ASTNode* loopRest = node->right;

		ASTNode* cond = loopRest ? loopRest->left : nullptr;
		ASTNode* loopScope = loopRest ? loopRest->right : nullptr;

		ASTNode* update = loopScope ? loopScope->left : nullptr;
		ASTNode* body = loopScope ? loopScope->right : nullptr;

		if (init) generate_ir(init);

		char* L_start = new_label();
		char* L_end = new_label();

		emit(create_instruction(IR_LABEL, nullptr, nullptr, L_start));

		if (cond) {
			char* cond_temp = generate_ir(cond);
			emit(create_instruction(IR_IFZ, cond_temp, nullptr, L_end));
			if (cond_temp) delete[] cond_temp;
		}

		if (body) generate_ir(body);

		if (update) generate_ir(update);

		emit(create_instruction(IR_GOTO, nullptr, nullptr, L_start));
		emit(create_instruction(IR_LABEL, nullptr, nullptr, L_end));

		if (L_start) delete[] L_start;
		if (L_end) delete[] L_end;
		return nullptr;
	}

	case NODE_FUNC_DECL: {
		emit(create_instruction(IR_FUNC_START, nullptr, nullptr, node->data));
		ASTNode* signature = node->left;
		ASTNode* param_list = (signature) ? signature->right : nullptr;

		vector<char*> params;
		ASTNode* param = param_list;

		while (param) {
			if (param->data) {
				params.push_back(param->data);
			}
			param = param->next;
		}

		for (int i = params.size() - 1; i >= 0; i--) {
			emit(create_instruction(IR_POP_PARAM, nullptr, nullptr, params[i]));
		}
		generate_ir(node->right);
		return nullptr;
	}

	case NODE_RETURN: {
		char* res = nullptr;
		if (node->left) res = generate_ir(node->left);
		emit(create_instruction(IR_RET, nullptr, nullptr, res));
		if (res) delete[] res;
		return nullptr;
	}

	case NODE_FUNC_CALL: {
		ASTNode* arg = node->left;
		int count = 0;

		while (arg) {
			char* arg_temp = generate_ir(arg);
			emit(create_instruction(IR_PARAM, arg_temp, nullptr, nullptr));
			if (arg_temp) delete[] arg_temp;
			count++;
			arg = arg->next;
		}

		char* result = new_temp();
		char count_str[16];
		sprintf(count_str, "%d", count);
		emit(create_instruction(IR_CALL, node->data, count_str, result));
		return result;
	}

	default:
		return nullptr;
	}
}

IRInst* generate_ir_from_ast(ASTNode* root) {
	ir_head = nullptr;
	generate_ir(root);
	return ir_head;
}
