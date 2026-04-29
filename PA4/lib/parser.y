%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char *s);

ASTNode* root = NULL;
int syntax_errors = 0;

#define NEW_NODE(type, data, left, right) create_node(type, data, yylineno, left, right)
%}

/* --- DEFINITIONS --- */
%union {
    char* sval;
    struct ASTNode* node;
}

/* --- TOKEN DECLARATIONS --- */
%token <sval> T_ID T_INT_LIT T_FLOAT_LIT T_BOOL_LIT
%token T_INT T_FLOAT T_BOOL T_VOID
%token T_IF T_ELSE T_WHILE T_FOR T_RETURN T_MAIN 
%token T_CIN T_COUT 
%token T_PLUS T_MINUS T_MULT T_DIV T_MOD T_ASSIGN
%token T_EQ T_NEQ T_LT T_GT T_LE T_GE
%token T_AND T_OR T_NOT 
%token T_INCREMENT T_DECREMENT
%token T_STREAM_IN T_STREAM_OUT
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE 
%token T_LBRACKET T_RBRACKET T_SEMI T_COMMA
%token T_ERROR

/* --- NON-TERMINAL TYPES --- */
%type <node> program declaration_list declaration var_declaration func_declaration
%type <node> type param_list param block statement_list statement
%type <node> assignment_stmt if_stmt while_stmt for_stmt return_stmt io_stmt
%type <node> expression arg_list

/* --- PRECEDENCE --- */
%right T_ASSIGN
%left T_OR
%left T_AND
%left T_EQ T_NEQ
%left T_LT T_GT T_LE T_GE
%left T_PLUS T_MINUS
%left T_MULT T_DIV T_MOD
%right T_NOT T_INCREMENT T_DECREMENT
%nonassoc LOWER_THAN_ELSE
%nonassoc T_ELSE

%%

/* --- GRAMMAR RULES --- */

program
    : declaration_list { root = NEW_NODE(NODE_PROGRAM, "ROOT", $1, NULL); }
    ;

declaration_list
    : declaration_list declaration { $$ = append_node($1, $2); }
    | { $$ = NULL; }
    ;

declaration
    : var_declaration { $$ = $1; }
    | func_declaration { $$ = $1; }
    | error T_SEMI {
        fprintf(stderr, "Panic Mode: Invalid declaration at line %d.\n", yylineno);
        yyerrok;
        $$ = NULL; 
    }
    ;

var_declaration
    : type T_ID T_SEMI 
      { $$ = NEW_NODE(NODE_VAR_DECL, $2, $1, NULL); }
    
    | type T_ID T_ASSIGN expression T_SEMI 
      { $$ = NEW_NODE(NODE_VAR_DECL, $2, $1, $4); }
    
    | type T_ID T_LBRACKET T_INT_LIT T_RBRACKET T_SEMI
      { 
          char buffer[50];
          sprintf(buffer, "%s[%s]", $2, $4);
          $$ = NEW_NODE(NODE_VAR_DECL, buffer, $1, NULL); 
      }
    ;

type
    : T_INT   { $$ = NEW_NODE(NODE_LITERAL, "int", NULL, NULL); }
    | T_FLOAT { $$ = NEW_NODE(NODE_LITERAL, "float", NULL, NULL); }
    | T_VOID  { $$ = NEW_NODE(NODE_LITERAL, "void", NULL, NULL); }
    | T_BOOL  { $$ = NEW_NODE(NODE_LITERAL, "bool", NULL, NULL); }
    ;

func_declaration
    : type T_ID T_LPAREN param_list T_RPAREN block
      { 
        ASTNode* signature = NEW_NODE(NODE_BLOCK, "Signature", $1, $4);
        $$ = create_node(NODE_FUNC_DECL, $2, $1->line, signature, $6);
      } 
    
    | type T_MAIN T_LPAREN param_list T_RPAREN block
      { 
        ASTNode* signature = NEW_NODE(NODE_BLOCK, "Signature", $1, $4);
        $$ = create_node(NODE_FUNC_DECL, "main", $1->line, signature, $6);
      }
    ;

param_list
    : param_list T_COMMA param { $$ = append_node($1, $3); }
    | param { $$ = $1; }
    | { $$ = NULL; }
    ;

param
    : type T_ID { $$ = NEW_NODE(NODE_VAR_DECL, $2, $1, NULL); }
    | type T_ID T_LBRACKET T_RBRACKET 
      { 
          char buffer[50];
          sprintf(buffer, "%s[]", $2);
          $$ = NEW_NODE(NODE_VAR_DECL, buffer, $1, NULL); 
      }
    ;

block
    : T_LBRACE statement_list T_RBRACE 
      { $$ = NEW_NODE(NODE_BLOCK, "{}", $2, NULL); }
    ;

statement_list
    : statement_list statement { $$ = append_node($1, $2); }
    | { $$ = NULL; }
    ;

statement
    : assignment_stmt { $$ = $1; }
    | if_stmt         { $$ = $1; }
    | while_stmt      { $$ = $1; }
    | for_stmt        { $$ = $1; }
    | return_stmt     { $$ = $1; }
    | io_stmt         { $$ = $1; }
    | block           { $$ = $1; }
    | var_declaration { $$ = $1; }
    | expression T_SEMI { $$ = $1; } 
    | error T_SEMI { 
        fprintf(stderr, "Panic Mode: Invalid statement at line %d.\n", yylineno);
        yyerrok; 
        $$ = NULL; 
    }
    ;

assignment_stmt
    : T_ID T_ASSIGN expression T_SEMI 
      { $$ = NEW_NODE(NODE_ASSIGN, "=", NEW_NODE(NODE_VAR_USE, $1, NULL, NULL), $3); }
      
    | T_ID T_LBRACKET expression T_RBRACKET T_ASSIGN expression T_SEMI
      {
         ASTNode* arrAccess = NEW_NODE(NODE_VAR_USE, $1, $3, NULL);
         $$ = NEW_NODE(NODE_ASSIGN, "[]=", arrAccess, $6);
      }
    ;

if_stmt
    : T_IF T_LPAREN expression T_RPAREN statement %prec LOWER_THAN_ELSE
      { 
        $$ = NEW_NODE(NODE_IF, "if", $3, $5);
      }
    | T_IF T_LPAREN expression T_RPAREN statement T_ELSE statement 
      { 
        ASTNode* branches = NEW_NODE(NODE_BLOCK, "IfElseBranches", $5, $7);
        $$ = NEW_NODE(NODE_IF, "if-else", $3, branches);
      }
    ;

while_stmt
    : T_WHILE T_LPAREN expression T_RPAREN statement
      { $$ = NEW_NODE(NODE_WHILE, "while", $3, $5); }
    ;

for_stmt
    : T_FOR T_LPAREN assignment_stmt expression T_SEMI assignment_stmt T_RPAREN statement
      { 
          ASTNode* loopScope = NEW_NODE(NODE_BLOCK, "LoopScope", $6, $8);
          
          ASTNode* loopRest = NEW_NODE(NODE_BLOCK, "LoopRest", $4, loopScope);

          $$ = NEW_NODE(NODE_FOR, "for", $3, loopRest);
      }
    ;

io_stmt
    : T_CIN T_STREAM_IN T_ID T_SEMI
      { $$ = NEW_NODE(NODE_IO, "read", NEW_NODE(NODE_VAR_USE, $3, NULL, NULL), NULL); }
    
    | T_CIN T_STREAM_IN T_ID T_LBRACKET expression T_RBRACKET T_SEMI
      { 
          // Create a VAR_USE node that has the index (expression) as its Left child
          ASTNode* arrAccess = NEW_NODE(NODE_VAR_USE, $3, $5, NULL);
          $$ = NEW_NODE(NODE_IO, "read", arrAccess, NULL); 
      }

    | T_COUT T_STREAM_OUT expression T_SEMI
      { $$ = NEW_NODE(NODE_IO, "print", $3, NULL); }
    ;

return_stmt
    : T_RETURN expression T_SEMI
      { $$ = NEW_NODE(NODE_RETURN, "return", $2, NULL); }
    | T_RETURN T_SEMI
      { $$ = NEW_NODE(NODE_RETURN, "return", NULL, NULL); }
    ;

expression
    : expression T_PLUS expression   { $$ = NEW_NODE(NODE_BIN_OP, "+", $1, $3); }
    | expression T_MINUS expression  { $$ = NEW_NODE(NODE_BIN_OP, "-", $1, $3); }
    | expression T_MULT expression   { $$ = NEW_NODE(NODE_BIN_OP, "*", $1, $3); }
    | expression T_DIV expression    { $$ = NEW_NODE(NODE_BIN_OP, "/", $1, $3); }
    | expression T_MOD expression    { $$ = NEW_NODE(NODE_BIN_OP, "%", $1, $3); }
    | expression T_LT expression     { $$ = NEW_NODE(NODE_BIN_OP, "<", $1, $3); }
    | expression T_GT expression     { $$ = NEW_NODE(NODE_BIN_OP, ">", $1, $3); }
    | expression T_LE expression     { $$ = NEW_NODE(NODE_BIN_OP, "<=", $1, $3); }
    | expression T_GE expression     { $$ = NEW_NODE(NODE_BIN_OP, ">=", $1, $3); }
    | expression T_EQ expression     { $$ = NEW_NODE(NODE_BIN_OP, "==", $1, $3); }
    | expression T_NEQ expression    { $$ = NEW_NODE(NODE_BIN_OP, "!=", $1, $3); }
    | expression T_AND expression    { $$ = NEW_NODE(NODE_BIN_OP, "&&", $1, $3); }
    | expression T_OR expression     { $$ = NEW_NODE(NODE_BIN_OP, "||", $1, $3); }
    
    | T_MINUS expression %prec T_NOT { $$ = NEW_NODE(NODE_UNARY_OP, "-", $2, NULL); }
    | T_NOT expression               { $$ = NEW_NODE(NODE_UNARY_OP, "!", $2, NULL); }
    | T_INCREMENT expression         { $$ = NEW_NODE(NODE_UNARY_OP, "++", $2, NULL); }
    | T_DECREMENT expression         { $$ = NEW_NODE(NODE_UNARY_OP, "--", $2, NULL); }
    | expression T_INCREMENT         { $$ = NEW_NODE(NODE_UNARY_OP, "POST++", $1, NULL); }
    | expression T_DECREMENT         { $$ = NEW_NODE(NODE_UNARY_OP, "POST--", $1, NULL); }
    
    | T_LPAREN expression T_RPAREN   { $$ = $2; }
    
    | T_ID                           { $$ = NEW_NODE(NODE_VAR_USE, $1, NULL, NULL); }
    | T_INT_LIT                      { $$ = NEW_NODE(NODE_LITERAL, $1, NULL, NULL); }
    | T_FLOAT_LIT                    { $$ = NEW_NODE(NODE_LITERAL, $1, NULL, NULL); }
    | T_BOOL_LIT                     { $$ = NEW_NODE(NODE_LITERAL, $1, NULL, NULL); }
    
    | T_ID T_LBRACKET expression T_RBRACKET
      { $$ = NEW_NODE(NODE_VAR_USE, $1, $3, NULL); }

    | T_ID T_LPAREN T_RPAREN 
      { $$ = NEW_NODE(NODE_FUNC_CALL, $1, NULL, NULL); }
    
    | T_ID T_LPAREN arg_list T_RPAREN 
      { $$ = NEW_NODE(NODE_FUNC_CALL, $1, $3, NULL); }
    ;

arg_list
    : arg_list T_COMMA expression { $$ = append_node($1, $3); }
    | expression { $$ = $1; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s (Token: %s)\n", yylineno, s, yytext);
    syntax_errors++;
}