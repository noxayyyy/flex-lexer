%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// External Lexer variables
extern int yylex();
extern int yylineno;
extern char* yytext;

// Error handling function
void yyerror(const char *s);

// Global Root for the Driver
ASTNode* root = NULL;

int syntax_errors = 0;
#define NEW_NODE(type, data, left, right, extra) create_node(type, data, yylineno, left, right, extra)
%}

/* --- DEFINITIONS --- */
%union {
    char* sval;       // For lexemes (IDs, Literals)
    struct ASTNode* node; // For AST Nodes
}

/* --- TOKEN DECLARATIONS (DO NOT CHANGE THESE) --- */

/* Tokens with values (Strings) */
%token <sval> T_ID T_INT_LIT T_FLOAT_LIT T_BOOL_LIT

/* Keywords */
%token T_INT T_FLOAT T_BOOL T_VOID
%token T_IF T_ELSE T_WHILE T_FOR T_RETURN T_MAIN 
%token T_CIN T_COUT 

/* Operators */
%token T_PLUS T_MINUS T_MULT T_DIV T_MOD T_ASSIGN
%token T_EQ T_NEQ T_LT T_GT T_LE T_GE
%token T_AND T_OR T_NOT 
%token T_INCREMENT T_DECREMENT
%token T_STREAM_IN T_STREAM_OUT

/* Separators */
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE 
%token T_LBRACKET T_RBRACKET T_SEMI T_COMMA
%token T_ERROR

/* --- NON-TERMINAL TYPES --- */
%type <node> program declarations declare variable function
%type <node> parameters param block statements stmt
%type <node> expr_stmt if_stmt while_stmt for_stmt return_stmt io_stmt
%type <node> expression type arguments

/* --- PRECEDENCE AND ASSOCIATIVITY --- */
%right T_ASSIGN
%left T_OR
%left T_AND
%left T_EQ T_NEQ
%left T_LT T_GT T_LE T_GE
%left T_PLUS T_MINUS
%left T_MULT T_DIV T_MOD
%right T_NOT
%left T_INCREMENT T_DECREMENT

/* --- GRAMMAR RULES --- */
%%
program
    : declarations { root = NEW_NODE(NODE_PROGRAM, "Program", $1, nullptr, nullptr); }
    ;

declarations
	: declare { $$ = $1; }
	| declarations declare { $$ = append_node($1, $2); }
	;

declare
	: variable { $$ = $1; }
	| function { $$ = $1; }
	;

/* --- Variables and Types --- */

type
	: T_INT { $$ = NEW_NODE(NODE_LITERAL, "int", nullptr, nullptr, nullptr); }
	| T_FLOAT { $$ = NEW_NODE(NODE_LITERAL, "float", nullptr, nullptr, nullptr); }
	| T_BOOL { $$ = NEW_NODE(NODE_LITERAL, "bool", nullptr, nullptr, nullptr); }
	| T_VOID { $$ = NEW_NODE(NODE_LITERAL, "void", nullptr, nullptr, nullptr); }
	;

variable
	: type T_ID T_SEMI {
		$$ = NEW_NODE(NODE_VAR_DECL, $2, $1, nullptr, nullptr);
	}
	| type T_ID T_ASSIGN expression T_SEMI {
		$$ = NEW_NODE(NODE_VAR_DECL, $2, $1, $4, nullptr);
	}
	;

/* --- Functions --- */

function
	: type T_MAIN T_LPAREN T_RPAREN block {
		ASTNode* func_decl = NEW_NODE(NODE_BLOCK, "Block", $1, nullptr, nullptr);
		$$ = NEW_NODE(NODE_FUNC_DECL, "main", func_decl, $5, nullptr);
	}
	| type T_ID T_LPAREN parameters T_RPAREN block {
		ASTNode* arg_decl = append_node($1, $4);
		ASTNode* func_decl = NEW_NODE(NODE_BLOCK, "Block", arg_decl, nullptr, nullptr);
		$$ = NEW_NODE(NODE_FUNC_DECL, $2, func_decl, $6, nullptr);
	}
	| type T_ID T_LPAREN T_RPAREN block {
		ASTNode* func_decl = NEW_NODE(NODE_BLOCK, "Block", $1, nullptr, nullptr);
		$$ = NEW_NODE(NODE_FUNC_DECL, $2, func_decl, $5, nullptr);
	}
	;

parameters
	: param { $$ = $1; }
	| parameters T_COMMA param { $$ = append_node($1, $3); }
	;

param
	: type T_ID { $$ = NEW_NODE(NODE_VAR_DECL, $2, $1, nullptr, nullptr); }
	;

/* --- Blocks and Statements --- */

block
	: T_LBRACE statements T_RBRACE { $$ = NEW_NODE(NODE_BLOCK, "Block", $2, nullptr, nullptr); }
	| T_LBRACE T_RBRACE { $$ = NEW_NODE(NODE_BLOCK, "Block", nullptr, nullptr, nullptr); }
	;

statements
	: stmt { $$ = $1; }
	| statements stmt { $$ = append_node($1, $2); }

stmt
	: variable { $$ = $1; }
	| expr_stmt { $$ = $1; }
	| if_stmt { $$ = $1; }
	| while_stmt { $$ = $1; }
	| for_stmt { $$ = $1; }
	| return_stmt { $$ = $1; }
	| io_stmt { $$ = $1; }
	| block { $$ = $1; }
	| error T_SEMI { yyerror; $$ = nullptr; }
	;

expr_stmt
	: expression T_SEMI { $$ = $1; }
	| T_SEMI { $$ = nullptr; }

if_stmt
	: T_IF T_LPAREN expression T_RPAREN stmt {
		$$ = NEW_NODE(NODE_IF, "If", $3, $5, nullptr);
	}
	| T_IF T_LPAREN expression T_RPAREN stmt T_ELSE stmt {
		ASTNode* branches = NEW_NODE(NODE_BLOCK, "IfElseBranches", $5, $7, nullptr);
		$$ = NEW_NODE(NODE_IF, "If", $3, branches, nullptr);
	}
	;

while_stmt
	: T_WHILE T_LPAREN expression T_RPAREN stmt {
		$$ = NEW_NODE(NODE_WHILE, "While", $3, $5, nullptr);
	}
	;

for_stmt
	: T_FOR T_LPAREN expr_stmt expr_stmt expr_stmt T_RPAREN stmt {
		ASTNode* loop_scope = NEW_NODE(NODE_BLOCK, "LoopScope", $5, $7, nullptr);
		ASTNode* loop_rest = NEW_NODE(NODE_BLOCK, "LoopRest", $4, loop_scope, nullptr);
		$$ = NEW_NODE(NODE_FOR, "For", $3, loop_rest, nullptr);
	}
	;

return_stmt
	: T_RETURN expression T_SEMI { $$ = NEW_NODE(NODE_RETURN, "Return", $2, nullptr, nullptr); }
	| T_RETURN T_SEMI { $$ = NEW_NODE(NODE_RETURN, "Return", nullptr, nullptr, nullptr); }
	;

io_stmt
	: T_CIN T_STREAM_IN T_ID T_SEMI {
		ASTNode* node = NEW_NODE(NODE_VAR_USE, $3, nullptr, nullptr, nullptr);
		$$ = NEW_NODE(NODE_IO, "read", node, nullptr, nullptr);
	}
	| T_COUT T_STREAM_OUT expression T_SEMI {
		$$ = NEW_NODE(NODE_IO, "print", $3, nullptr, nullptr);
	}
	;

expression
	: T_ID T_ASSIGN expression {
		ASTNode* node = NEW_NODE(NODE_VAR_USE, $1, nullptr, nullptr, nullptr);
		$$ = NEW_NODE(NODE_ASSIGN, "=", node, $3, nullptr);
	}
	| expression T_OR expression { $$ = NEW_NODE(NODE_BIN_OP, "||", $1, $3, nullptr); }
	| expression T_AND expression { $$ = NEW_NODE(NODE_BIN_OP, "&&", $1, $3, nullptr); }
	| expression T_EQ expression { $$ = NEW_NODE(NODE_BIN_OP, "==", $1, $3, nullptr); }
	| expression T_NEQ expression { $$ = NEW_NODE(NODE_BIN_OP, "!=", $1, $3, nullptr); }
	| expression T_LT expression { $$ = NEW_NODE(NODE_BIN_OP, "<", $1, $3, nullptr); }
	| expression T_GT expression { $$ = NEW_NODE(NODE_BIN_OP, ">", $1, $3, nullptr); }
	| expression T_LE expression { $$ = NEW_NODE(NODE_BIN_OP, "<=", $1, $3, nullptr); }
	| expression T_GE expression { $$ = NEW_NODE(NODE_BIN_OP, ">=", $1, $3, nullptr); }
	| expression T_PLUS expression { $$ = NEW_NODE(NODE_BIN_OP, "+", $1, $3, nullptr); }
	| expression T_MINUS expression { $$ = NEW_NODE(NODE_BIN_OP, "-", $1, $3, nullptr); }
	| expression T_MULT expression { $$ = NEW_NODE(NODE_BIN_OP, "*", $1, $3, nullptr); }
	| expression T_DIV expression { $$ = NEW_NODE(NODE_BIN_OP, "/", $1, $3, nullptr); }
	| expression T_MOD expression { $$ = NEW_NODE(NODE_BIN_OP, "%", $1, $3, nullptr); }
	| T_NOT expression { $$ = NEW_NODE(NODE_UNARY_OP, "!", $2, nullptr, nullptr); }
	| T_ID T_INCREMENT {
		ASTNode* node = NEW_NODE(NODE_VAR_USE, $1, nullptr, nullptr, nullptr);
		$$ = NEW_NODE(NODE_UNARY_OP, "++", node, nullptr, nullptr);
	}
	| T_ID T_DECREMENT {
		ASTNode* node = NEW_NODE(NODE_VAR_USE, $1, nullptr, nullptr, nullptr);
		$$ = NEW_NODE(NODE_UNARY_OP, "--", node, nullptr, nullptr);
	}
	| T_LPAREN expression T_RPAREN { $$ = $2; }
	| T_ID T_LPAREN arguments T_RPAREN {
		$$ = NEW_NODE(NODE_FUNC_CALL, $1, $3, nullptr, nullptr);
	}
	| T_ID T_LPAREN T_RPAREN {
		$$ = NEW_NODE(NODE_FUNC_CALL, $1, nullptr, nullptr, nullptr);
	}
	| T_ID { $$ = NEW_NODE(NODE_VAR_USE, $1, nullptr, nullptr, nullptr); }
	| T_INT_LIT { $$ = NEW_NODE(NODE_LITERAL, $1, nullptr, nullptr, nullptr); }
	| T_FLOAT_LIT { $$ = NEW_NODE(NODE_LITERAL, $1, nullptr, nullptr, nullptr); }
	| T_BOOL_LIT { $$ = NEW_NODE(NODE_LITERAL, $1, nullptr, nullptr, nullptr); }
	;

arguments
	: expression { $$ = $1; }
	| arguments T_COMMA expression { $$ = append_node($1, $3); }
	;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s (Token: %s)\n", yylineno, s, yytext);
    syntax_errors++;
}
