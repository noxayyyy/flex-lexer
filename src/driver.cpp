#include <stdio.h>
#include "tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int yylex();
extern char* yytext;
extern int yylineno;
extern FILE* yyin;

#ifdef __cplusplus
}
#endif

const char* get_token_name(int token) {
    switch(token) {
    // --- Keywords ---
    case T_IF:          return "KEYWORD_IF";
    case T_ELSE:        return "KEYWORD_ELSE";
    case T_WHILE:       return "KEYWORD_WHILE";
    case T_FOR:         return "KEYWORD_FOR";
    case T_RETURN:      return "KEYWORD_RETURN";
    case T_INT:         return "TYPE_INT";
    case T_FLOAT:       return "TYPE_FLOAT";
    case T_VOID:        return "TYPE_VOID";
    case T_BOOL:        return "TYPE_BOOL";
    case T_MAIN:        return "KEYWORD_MAIN";
    case T_CIN:         return "KEYWORD_CIN";
    case T_COUT:        return "KEYWORD_COUT";

    // --- Literals ---
    case T_ID:          return "IDENTIFIER";
    case T_INT_LIT:     return "LITERAL_INT";
    case T_FLOAT_LIT:   return "LITERAL_FLOAT";
    case T_BOOL_LIT:    return "LITERAL_BOOL";

    // --- Arithmetic Operators ---
    case T_PLUS:        return "OP_PLUS";
    case T_MINUS:       return "OP_MINUS";
    case T_MULT:        return "OP_MULT";
    case T_DIV:         return "OP_DIV";
    case T_MOD:         return "OP_MOD";
    case T_ASSIGN:      return "OP_ASSIGN";
    case T_INCREMENT:   return "OP_INCREMENT";
    case T_DECREMENT:   return "OP_DECREMENT";

    // --- Relational & Logical Operators ---
    case T_EQ:          return "OP_EQ";
    case T_NEQ:         return "OP_NEQ";
    case T_LT:          return "OP_LT";
    case T_LE:          return "OP_LE";
    case T_GT:          return "OP_GT";
    case T_GE:          return "OP_GE";
    case T_AND:         return "OP_AND";
    case T_OR:          return "OP_OR";
    case T_NOT:         return "OP_NOT";

    // --- I/O Stream Operators ---
    case T_STREAM_IN:   return "OP_STREAM_IN";  // >>
    case T_STREAM_OUT:  return "OP_STREAM_OUT"; // <<

    // --- Separators ---
    case T_LPAREN:      return "LPAREN";
    case T_RPAREN:      return "RPAREN";
    case T_LBRACE:      return "LBRACE";
    case T_RBRACE:      return "RBRACE";
    case T_LBRACKET:    return "LBRACKET";
    case T_RBRACKET:    return "RBRACKET";
    case T_SEMI:        return "SEMICOLON";
    case T_COMMA:       return "COMMA";

    // --- Special ---
    case T_EOF:         return "EOF";
    case T_ERROR:       return "ERROR";

    default:            return "UNKNOWN_TOKEN";
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    } else {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int token;
    
    printf("%-6s %-15s %s\n", "Line", "Token Type", "Lexeme");
    printf("--------------------------------\n");
    
    while ((token = yylex())) {
        const char* token_name = get_token_name(token);
        
        printf("%-6d %-15s %s\n", yylineno, token_name, yytext);
    }


    if (argc > 1) {
        fclose(yyin);
    }

    return 0;
}