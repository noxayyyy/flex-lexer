#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { KIND_VAR, KIND_FUNC, KIND_PARAM, KIND_ARRAY } SymbolKind;

typedef struct Symbol {
	char* name;
	char* type;
	SymbolKind kind;
	int scope_level;

	int param_count;
	char** param_types;

	struct Symbol* next;
} Symbol;

void init_symbol_table();

void push_scope();

void pop_scope();

/**
 * Inserts a new symbol into the current scope.
 * Returns 1 if successful, 0 if it's a redeclaration (error).
 */
int insert_symbol(char* name, char* type, SymbolKind kind);

/**
 * Searches for a symbol by name.
 * Scans from current scope down to global scope.
 * Returns NULL if not found.
 */
Symbol* lookup_symbol(char* name);

/**
 * Searches only in the CURRENT scope.
 * Returns NULL if not found.
 */
Symbol* lookup_local_symbol(char* name);

// for debugging
void print_symbol_table();

#ifdef __cplusplus
}
#endif

#endif
