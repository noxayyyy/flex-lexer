%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

int yylineno; 
%}

%option yylineno
%option noyywrap

/* Regex Definitions */
DIGIT       [0-9]
LETTER      [a-zA-Z]
INT_NUM {DIGIT}+|-{DIGIT}+
FLOAT_NUM   {DIGIT}+"."{DIGIT}+
ID          {LETTER}({LETTER}|{DIGIT}|_)*

%%

[ \t\r\n]+          { /* Ignore Whitespace */ }

"//".* { /* Ignore Single-line Comment*/ }

 /* Keywords  */
"if"                { return T_IF; }
"else"              { return T_ELSE; }
"while"             { return T_WHILE; }
"for"               { return T_FOR; }
"return"            { return T_RETURN; }
"int"               { return T_INT; }
"float"             { return T_FLOAT; }
"void"              { return T_VOID; }
"main"              { return T_MAIN; }
"bool"              { return T_BOOL; } 
"cout"              {return T_COUT;}
"cin"               {return T_CIN;}

"true"      { yylval.sval = strdup("true"); return T_BOOL_LIT; }
"false"     { yylval.sval = strdup("false"); return T_BOOL_LIT; }

 /* Operators & Separators */
"++"                {return T_INCREMENT;}
"--"                {return T_DECREMENT;}
"+"                 { return T_PLUS; }
"-"                 { return T_MINUS; }
"*"                 { return T_MULT; }
"/"                 { return T_DIV; }
"%"                 { return T_MOD; }
"=="                { return T_EQ; } /* Order matters: Check == before = */
"!="                { return T_NEQ; }
"<="                { return T_LE; }
">="                { return T_GE; }
"<<"                { return T_STREAM_OUT; }
">>"                { return T_STREAM_IN; }
"&&"                { return T_AND; }
"||"                { return T_OR; }
"<"                 { return T_LT; }
">"                 { return T_GT; }
"="                 { return T_ASSIGN; }
"!"                 { return T_NOT; }

"("                 { return T_LPAREN; }
")"                 { return T_RPAREN; }
"{"                 { return T_LBRACE; }
"}"                 { return T_RBRACE; }
"["                 { return T_LBRACKET; }
"]"                 { return T_RBRACKET; }
";"                 { return T_SEMI; }
","                 { return T_COMMA; }

{FLOAT_NUM} { yylval.sval = strdup(yytext); return T_FLOAT_LIT; }
{INT_NUM}   { yylval.sval = strdup(yytext); return T_INT_LIT; }
{ID}        { yylval.sval = strdup(yytext); return T_ID; }

.                   { 
                        
                        return T_ERROR; 
                    }

%%
