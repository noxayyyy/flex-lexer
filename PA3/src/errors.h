#ifndef ERRORS_H
#define ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ERR_UNDECLARED_VAR,         // Variable used but not declared ...
    ERR_UNDECLARED_FUNC,        // Function called but not declared
    ERR_REDEF_VAR,              // Variable declared again in the same scope
    ERR_REDEF_FUNC,             // Function declared again

    ERR_TYPE_MISMATCH_ASSIGN,   // int x = "string"; or int x = true;
    ERR_TYPE_MISMATCH_OP,       // true + 5 or "hello" * 3
    ERR_TYPE_MISMATCH_RETURN,   // Function declared void returns 5

    ERR_FUNC_ARG_COUNT,         // Expected 2 args, got 3 
    ERR_FUNC_ARG_TYPE,          // Expected int, got bool for arg #2 
    
    ERR_NOT_A_FUNCTION,         // trying to call a non-function: int x; x();
    ERR_NOT_AN_ARRAY,           // trying to index a non-array: int x; x[0]; ////
    ERR_ARRAY_INDEX_TYPE,       // Array index is not an integer: arr[true] ////
} ErrorType;

/**
 * Reports a semantic error to stdout in a standardized format.
 * @param type: The specific error code (see enum above)
 * @param lineno: The line number where the error occurred
 */
void report_error(ErrorType type, int lineno);

#ifdef __cplusplus
}
#endif

#endif