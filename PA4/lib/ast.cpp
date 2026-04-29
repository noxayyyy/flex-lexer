#include <iostream>     
#include <cstring>
#include "ast.h"

using namespace std;

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
        cout << "  ";
    }
}

void print_ast(ASTNode* node, int level) {
    if (!node) return;

    print_indent(level);
    
    switch (node->type) {
        case NODE_PROGRAM:    cout << "Program" << endl; break;
        case NODE_VAR_DECL:   cout << "VarDecl: " << (node->data ? node->data : "") << endl; break;
        case NODE_FUNC_DECL:  cout << "FuncDecl: " << (node->data ? node->data : "") << endl; break;
        case NODE_BLOCK:      cout << "Block" << endl; break;
        case NODE_ASSIGN:     cout << "Assign" << endl; break;
        case NODE_IF:         cout << "If" << endl; break;
        case NODE_WHILE:      cout << "While" << endl; break;
        case NODE_FOR:        cout << "For" << endl; break;
        case NODE_RETURN:     cout << "Return" << endl; break;
        case NODE_BIN_OP:     cout << "Op: " << (node->data ? node->data : "") << endl; break;
        case NODE_UNARY_OP:   cout << "Unary: " << (node->data ? node->data : "") << endl; break;
        case NODE_LITERAL:    cout << "Literal: " << (node->data ? node->data : "") << endl; break;
        case NODE_VAR_USE:    cout << "Var: " << (node->data ? node->data : "") << endl; break;
        case NODE_FUNC_CALL:  cout << "Call: " << (node->data ? node->data : "") << endl; break;
        case NODE_IO:         cout << "IO: " << (node->data ? node->data : "") << endl; break;
        default:              cout << "Unknown Node" << endl; break;
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