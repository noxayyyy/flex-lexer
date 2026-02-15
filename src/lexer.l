%{
#include <stdio.h>
#include <string.h>
#include "tokens.h"

int yylineno;
%}

%option yylineno
%option noyywrap

/* Regex Definitions */
DIGIT [0-9]
LETTER [a-zA-Z]

/* lexer rules section */
/* define your regex rules to match the tokens here (an example has been provided)*/
%%
<<EOF>> { return T_EOF; }

"if"    { return T_IF; }
"else" { return T_ELSE; }
"while" { return T_WHILE; }
"for" { return T_FOR; }
"return" { return T_RETURN; }
"int" { return T_INT; }
"float" { return T_FLOAT; }
"void" { return T_VOID; }
"bool" { return T_BOOL; }
"main" { return T_MAIN; }
"cin" { return T_CIN; }
"cout" { return T_COUT; }

"true" |
"false" { return T_BOOL_LIT; }
({LETTER}|_)({LETTER}|{DIGIT}|_)* { return T_ID; }
{DIGIT}+ { return T_INT_LIT; }
{DIGIT}+\.{DIGIT}+ { return T_FLOAT_LIT; }

"+" { return T_PLUS; }
"-" { return T_MINUS; }
"*" { return T_MULT; }
"/" { return T_DIV; }
"%" { return T_MOD; }
"++" { return T_INCREMENT; }
"--" { return T_DECREMENT; }
"=" { return T_ASSIGN; }
"==" { return T_EQ; }
"!=" { return T_NEQ; }
"<" { return T_LT; }
"<=" { return T_LE; }
">" { return T_GT; }
">=" { return T_GE; }
"&&" { return T_AND; }
"||" { return T_OR; }
"!" { return T_NOT; }

">>" { return T_STREAM_OUT; }
"<<" { return T_STREAM_IN; }

"(" { return T_LPAREN; }
")" { return T_RPAREN; }
"{" { return T_LBRACE; }
"}" { return T_RBRACE; }
"[" { return T_LBRACKET; }
"]" { return T_RBRACKET; }
";" { return T_SEMI; }
"," { return T_COMMA; }

[\t\n\r ]+ {}

. {
	printf("Lexical Error at line %d: Unknown char '%s'\n", yylineno, yytext);
	return T_ERROR;
}
%%
