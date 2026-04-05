#include "ast.h"
#include <cstring>
#include <iostream>

char* cpp_strdup(const char* src) {
	if (!src) return nullptr;
	size_t len = strlen(src) + 1;
	char* dest = new char[len];
	strcpy(dest, src);
	return dest;
}

ASTNode* create_node(NodeType type, char* data, int line, ASTNode* left, ASTNode* right) {
	ASTNode* node = new ASTNode();

	node->type = type;

	node->data = data ? cpp_strdup(data) : nullptr;

	node->line = line;
	node->left = left;
	node->right = right;
	node->next = nullptr;

	return node;
}

ASTNode* append_node(ASTNode* head, ASTNode* new_node) {
	if (!head) return new_node;
	ASTNode* temp = head;
	while (temp->next) {
		temp = temp->next;
	}
	temp->next = new_node;
	return head;
}

void print_indent(int level) {
	for (int i = 0; i < level; i++) {
		std::cout << "  ";
	}
}

void print_ast(ASTNode* node, int level) {
	if (!node) return;

	print_indent(level);

	switch (node->type) {
	case NODE_PROGRAM:
		std::cout << "Program" << std::endl;
		break;
	case NODE_VAR_DECL:
		std::cout << "VarDecl: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_FUNC_DECL:
		std::cout << "FuncDecl: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_BLOCK:
		std::cout << "Block" << std::endl;
		break;
	case NODE_ASSIGN:
		std::cout << "Assign" << std::endl;
		break;
	case NODE_IF:
		std::cout << "If" << std::endl;
		break;
	case NODE_WHILE:
		std::cout << "While" << std::endl;
		break;
	case NODE_FOR:
		std::cout << "For" << std::endl;
		break;
	case NODE_RETURN:
		std::cout << "Return" << std::endl;
		break;
	case NODE_BIN_OP:
		std::cout << "Op: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_UNARY_OP:
		std::cout << "Unary: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_LITERAL:
		std::cout << "Literal: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_VAR_USE:
		std::cout << "Var: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_FUNC_CALL:
		std::cout << "Call: " << (node->data ? node->data : "") << std::endl;
		break;
	case NODE_IO:
		std::cout << "IO: " << (node->data ? node->data : "") << std::endl;
		break;
	default:
		std::cout << "Unknown Node" << std::endl;
		break;
	}

	print_ast(node->left, level + 1);
	print_ast(node->right, level + 1);

	print_ast(node->next, level);
}

void free_ast(ASTNode* node) {
	if (!node) return;

	free_ast(node->left);
	free_ast(node->right);
	free_ast(node->next);

	if (node->data) {
		delete[] node->data;
	}

	delete node;
}
