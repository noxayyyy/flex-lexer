#include <iostream>     
#include <cstring>
#include "ast.h"

using namespace std;

ASTNode* create_node(NodeType type, char* data, int line, ASTNode* left, ASTNode* right, ASTNode* extra)
{
}

ASTNode *append_node(ASTNode *head, ASTNode *new_node)
{
}

void free_ast(ASTNode *node)
{
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
    print_ast(node->extra, level + 1);

    print_ast(node->next, level);
}