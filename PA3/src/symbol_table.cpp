#include "symbol_table.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Symbol* symbol_table_head = NULL;
static int current_scope_level = 0;

void init_symbol_table() {
	symbol_table_head = NULL;
	current_scope_level = 0;
}

void push_scope() {
	current_scope_level++;
}

void pop_scope() {
	while (symbol_table_head && symbol_table_head->scope_level == current_scope_level) {
		Symbol* temp = symbol_table_head;
		symbol_table_head = symbol_table_head->next;

		delete[] temp->name;
		delete[] temp->type;
		if (temp->param_types) {
			for (int i = 0; i < temp->param_count; i++) {
				delete[] temp->param_types[i];
			}
			delete[] temp->param_types;
		}
		delete temp;
	}
	current_scope_level--;
}

int insert_symbol(char* name, char* type, SymbolKind kind) {
	if (lookup_local_symbol(name)) {
		return 0;
	}

	Symbol* symbol = new Symbol();
	symbol->name = cpp_strdup(name);
	symbol->type = cpp_strdup(type);
	symbol->kind = kind;
	symbol->scope_level = current_scope_level;
	symbol->param_count = 0;
	symbol->param_types = NULL;
	symbol->next = symbol_table_head;
	symbol_table_head = symbol;

	return 1;
}

Symbol* lookup_symbol(char* name) {
	Symbol* curr = symbol_table_head;
	while (curr) {
		if (!strcmp(curr->name, name)) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

Symbol* lookup_local_symbol(char* name) {
	Symbol* curr = symbol_table_head;
	while (curr && curr->scope_level == current_scope_level) {
		if (!strcmp(curr->name, name)) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

void print_symbol_table() {
	// Optional: for your own debugging
}
