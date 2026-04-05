#include "semantics.h"
#include "errors.h"
#include "symbol_table.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int semantic_errors = 0;
static char* current_func_type = NULL;

char* get_expr_type(ASTNode* node);
void check_node_list(ASTNode* head);
void check_node(ASTNode* node);

int semantic_analysis(ASTNode* root) {
	semantic_errors = 0;
	init_symbol_table();

	if (root && root->type == NODE_PROGRAM) {
		check_node_list(root->left);
	}

	return semantic_errors;
}

void check_node_list(ASTNode* head) {
	ASTNode* curr = head;
	while (curr) {
		check_node(curr);
		curr = curr->next;
	}
}

bool conditional_err(ErrorType type, int line, bool condition) {
	if (!condition) {
		return false;
	}
	report_error(type, line);
	semantic_errors++;

	return true;
}

char* get_expr_type(ASTNode* node) {
	if (!node) {
		return NULL;
	}

	switch (node->type) {
	case NODE_LITERAL:
		return (!strcmp(node->data, "true") || !strcmp(node->data, "false")) ? (char*)"bool"
			   : strchr(node->data, '.')                                     ? (char*)"float"
																			 : (char*)"int";
	case NODE_VAR_USE: {
		Symbol* symbol = lookup_symbol(node->data);
		if (conditional_err(ERR_UNDECLARED_VAR, node->line, !symbol)) {
			return NULL;
		}

		if (node->left) {
			conditional_err(ERR_NOT_AN_ARRAY, node->line, symbol->kind != KIND_ARRAY);
			char* idx_type = get_expr_type(node->left);
			conditional_err(ERR_ARRAY_INDEX_TYPE, node->line, idx_type && strcmp(idx_type, "int"));
		}
		return symbol->type;
	}
	case NODE_FUNC_CALL: {
		Symbol* symbol = lookup_symbol(node->data);
		if (conditional_err(ERR_UNDECLARED_FUNC, node->line, !symbol) ||
			conditional_err(ERR_NOT_A_FUNCTION, node->line, symbol->kind != KIND_FUNC)) {
			return NULL;
		}

		int arg_count = 0;
		ASTNode* arg = node->left;
		while (arg) {
			arg_count++;
			arg = arg->next;
		}

		if (conditional_err(ERR_FUNC_ARG_COUNT, node->line, arg_count != symbol->param_count)) {
			return symbol->type;
		}

		arg = node->left;
		for (int i = 0; i < arg_count; i++) {
			char* atype = get_expr_type(arg);
			conditional_err(
				ERR_FUNC_ARG_TYPE, node->line, atype && strcmp(atype, symbol->param_types[i])
			);
			arg = arg->next;
		}

		return symbol->type;
	}
	case NODE_BIN_OP: {
		char* left_type = get_expr_type(node->left);
		char* right_type = get_expr_type(node->right);

		if (!left_type || !right_type) {
			return NULL;
		}
		if (conditional_err(ERR_TYPE_MISMATCH_OP, node->line, strcmp(left_type, right_type))) {
			return NULL;
		}

		if (!strcmp(node->data, "<") || !strcmp(node->data, ">") || !strcmp(node->data, "<=") ||
			!strcmp(node->data, ">=") || !strcmp(node->data, "==") || !strcmp(node->data, "!=") ||
			!strcmp(node->data, "&&") || !strcmp(node->data, "||")) {
			return (char*)"bool";
		}

		return left_type;
	}
	case NODE_UNARY_OP:
		return get_expr_type(node->left);
	default:
		return NULL;
	}
}

void init_func(Symbol* func_sym, ASTNode* sig) {
	int param_count = 0;
	ASTNode* param = sig->right;
	while (param) {
		param_count++;
		param = param->next;
	}
	func_sym->param_count = param_count;
	if (param_count) {
		func_sym->param_types = new char*[param_count];
		param = sig->right;
	}
	for (int i = 0; i < param_count; i++) {
		func_sym->param_types[i] = cpp_strdup(param->left->data);
		param = param->next;
	}
}

void check_arr(ASTNode* node) {
	char* base_name = cpp_strdup(node->data);
	char* bracket = strchr(base_name, '[');
	SymbolKind kind = KIND_VAR;
	if (bracket) {
		*bracket = '\0';
		kind = KIND_ARRAY;
	}
	char* type_str = node->left->data;
	conditional_err(ERR_REDEF_VAR, node->line, !insert_symbol(base_name, type_str, kind));

	delete[] base_name;
}

void check_node(ASTNode* node) {
	if (!node) return;

	switch (node->type) {
	case NODE_VAR_DECL: {
		check_arr(node);
		if (node->right) {
			char* rhs_type = get_expr_type(node->right);
			conditional_err(
				ERR_TYPE_MISMATCH_ASSIGN, node->line, rhs_type && strcmp(node->left->data, rhs_type)
			);
		}
		break;
	}
	case NODE_FUNC_DECL: {
		char* func_name = node->data;
		ASTNode* sig = node->left;
		char* ret_type = sig->left->data;

		if (!conditional_err(ERR_REDEF_FUNC, node->line, lookup_local_symbol(func_name))) {
			insert_symbol(func_name, ret_type, KIND_FUNC);
			Symbol* func_sym = lookup_symbol(func_name);
			if (func_sym) {
				init_func(func_sym, sig);
			}
		}

		push_scope();
		current_func_type = ret_type;

		ASTNode* param = sig->right;
		while (param) {
			check_arr(param);
			param = param->next;
		}

		if (node->right && node->right->left) {
			check_node_list(node->right->left);
		}

		pop_scope();
		current_func_type = NULL;

		break;
	}
	case NODE_BLOCK: {
		push_scope();
		check_node_list(node->left);
		pop_scope();

		break;
	}
	case NODE_ASSIGN: {
		char* lhs_type = get_expr_type(node->left);
		char* rhs_type = get_expr_type(node->right);
		conditional_err(
			ERR_TYPE_MISMATCH_ASSIGN, node->line, lhs_type && rhs_type && strcmp(lhs_type, rhs_type)
		);

		break;
	}
	case NODE_IF: {
		if (strcmp(node->data, "if-else")) {
			get_expr_type(node->left);
			check_node(node->right);

			break;
		}
		get_expr_type(node->left);
		check_node(node->right->left);
		check_node(node->right->right);

		break;
	}
	case NODE_WHILE: {
		get_expr_type(node->left);
		check_node(node->right);

		break;
	}
	case NODE_FOR: {
		push_scope();
		if (node->left) {
			check_node(node->left);
		}
		if (node->right) {
			ASTNode* rest = node->right;
			if (rest->left) {
				get_expr_type(rest->left);
			}
			if (rest->right) {
				check_node(rest->right->left);
				check_node(rest->right->right);
			}
		}
		pop_scope();

		break;
	}
	case NODE_RETURN: {
		if (!node->left) {
			conditional_err(
				ERR_TYPE_MISMATCH_RETURN,
				node->line,
				current_func_type && strcmp(current_func_type, "void")
			);

			break;
		}
		char* ret_type = get_expr_type(node->left);
		conditional_err(
			ERR_TYPE_MISMATCH_RETURN,
			node->line,
			(current_func_type && !strcmp(current_func_type, "void")) ||
				(ret_type && current_func_type && strcmp(ret_type, current_func_type))
		);

		break;
	}
	case NODE_IO:
	case NODE_FUNC_CALL:
	case NODE_BIN_OP:
	case NODE_UNARY_OP:
	case NODE_VAR_USE:
	case NODE_LITERAL: {
		get_expr_type(node);
		break;
	}
	default:
		break;
	}
}
