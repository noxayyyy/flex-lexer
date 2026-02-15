#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    T_EOF = 0,
    T_ERROR,  // For invalid characters
    
    // Keywords
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN,  // if, else, while, for, return
    T_INT, T_FLOAT, T_VOID, T_BOOL, T_MAIN,  // int, float, void, bool, main
    T_CIN, T_COUT,      //cin, cout
    
    // Literals
    T_ID,          // identifiers e.g., sum_value
    T_INT_LIT,     // e.g., 42
    T_FLOAT_LIT,   // e.g., 3.14
    T_BOOL_LIT,    // true, false

    // Operators
    T_PLUS, T_MINUS, T_MULT, T_DIV, T_MOD, // + - * / %
    T_INCREMENT, T_DECREMENT, // ++ --
    T_ASSIGN,      // =
    T_EQ, T_NEQ,   // == !=
    T_LT, T_LE,    // < <=
    T_GT, T_GE,    // > >=
    T_AND, T_OR, T_NOT, // && || !
    
    // Stream Operators
    T_STREAM_OUT,  // << (for cout support)
    T_STREAM_IN,   // >> (for cin support)

    // Separators
    T_LPAREN, T_RPAREN, // ( )
    T_LBRACE, T_RBRACE, // { }
    T_LBRACKET, T_RBRACKET, // [ ]
    T_SEMI, T_COMMA     // ; ,

} TokenType;

#endif