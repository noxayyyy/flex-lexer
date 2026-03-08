#ifndef AST_H
#define AST_H

typedef enum NodeType {
	NODE_PROGRAM,
	NODE_VAR_DECL,
	NODE_FUNC_DECL,
	NODE_BLOCK,
	NODE_ASSIGN,
	NODE_IF,
	NODE_WHILE,
	NODE_FOR,
	NODE_RETURN,
	NODE_BIN_OP,   // +, -, *, /, etc.
	NODE_UNARY_OP, // x++, --x
	NODE_LITERAL,  // 10, 3.14, true
	NODE_VAR_USE,  // x
	NODE_FUNC_CALL,
	NODE_IO // cin/cout
} NodeType;

typedef struct ASTNode {
	NodeType type;
	char* data; // Stores the operator ("+"), ID ("x"), or Value ("10")
	int line;
	struct ASTNode* left;  // Child 1 (e.g., LHS of assignment, or If-Condition)
	struct ASTNode* right; // Child 2 (e.g., RHS of assignment, or If-Body)
	struct ASTNode* extra; // Child 3 (e.g., Else-Body)
	struct ASTNode* next;  // For lists (next statement in a block)
} ASTNode;

#ifdef __cplusplus
extern "C" {
#endif

ASTNode* create_node(NodeType type, char* data, int line, ASTNode* left, ASTNode* right);
ASTNode* append_node(ASTNode* head, ASTNode* new_node);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

#ifdef __cplusplus
}
#endif

#endif
