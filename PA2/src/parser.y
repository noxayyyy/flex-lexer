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


/* --- PRECEDENCE AND ASSOCIATIVITY --- */

/* --- GRAMMAR RULES --- */
%%
program
    : /* start writing grammar rules here */
    ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s (Token: %s)\n", yylineno, s, yytext);
    syntax_errors++;
}