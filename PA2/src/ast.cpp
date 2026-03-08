#include "ast.h"
#include <cstring>
#include <iostream>

using namespace std;

ASTNode*
create_node(NodeType type, char* data, int line, ASTNode* left, ASTNode* right, ASTNode* extra) {
	char* data_cpy = nullptr;
	if (data) {
		data_cpy = new char[strlen(data) + 1];
		strcpy(data_cpy, data);
	}

	return new ASTNode { type, data_cpy, line, left, right, extra, nullptr };
}

ASTNode* append_node(ASTNode* head, ASTNode* new_node) {
	if (!head) {
		return new_node;
	}

	ASTNode* curr = head;
	while (curr->next) {
		curr = curr->next;
	}
	curr->next = new_node;

	return head;
}

void free_ast(ASTNode* node) {
	if (!node) return;

	free_ast(node->left);
	free_ast(node->right);
	free_ast(node->extra);
	free_ast(node->next);

	if (node->data) {
		delete[] node->data;
	}
	delete node;
}

// DO NOT CHANGE THESE FUNCTIONS
void print_indent(int level) {
	for (int i = 0; i < level; i++) {
		cout << "  ";
	}
}

void print_ast(ASTNode* node, int level) {
	if (!node) return;

	print_indent(level);

	switch (node->type) {
	case NODE_PROGRAM:
		cout << "Program" << endl;
		break;
	case NODE_VAR_DECL:
		cout << "VarDecl: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_FUNC_DECL:
		cout << "FuncDecl: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_BLOCK:
		cout << "Block" << endl;
		break;
	case NODE_ASSIGN:
		cout << "Assign" << endl;
		break;
	case NODE_IF:
		cout << "If" << endl;
		break;
	case NODE_WHILE:
		cout << "While" << endl;
		break;
	case NODE_FOR:
		cout << "For" << endl;
		break;
	case NODE_RETURN:
		cout << "Return" << endl;
		break;
	case NODE_BIN_OP:
		cout << "Op: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_UNARY_OP:
		cout << "Unary: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_LITERAL:
		cout << "Literal: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_VAR_USE:
		cout << "Var: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_FUNC_CALL:
		cout << "Call: " << (node->data ? node->data : "") << endl;
		break;
	case NODE_IO:
		cout << "IO: " << (node->data ? node->data : "") << endl;
		break;
	default:
		cout << "Unknown Node" << endl;
		break;
	}

	print_ast(node->left, level + 1);
	print_ast(node->right, level + 1);
	print_ast(node->extra, level + 1);

	print_ast(node->next, level);
}
