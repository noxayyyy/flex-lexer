#include "symbol_table.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

static Symbol* symbol_table_head = nullptr;
static int current_scope_level = 0;

char* sym_strdup(const char* src) {
	if (!src) return nullptr;
	size_t len = strlen(src) + 1;
	char* dest = new char[len];
	strcpy(dest, src);
	return dest;
}

void init_symbol_table() {
	symbol_table_head = nullptr;
	current_scope_level = 0;
}

void push_scope() {
	current_scope_level++;
}

void pop_scope() {
	while (symbol_table_head != nullptr && symbol_table_head->scope_level == current_scope_level) {
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
	if (lookup_local_symbol(name) != nullptr) {
		return 0;
	}

	Symbol* new_sym = new Symbol();
	new_sym->name = sym_strdup(name);
	new_sym->type = sym_strdup(type);
	new_sym->kind = kind;
	new_sym->scope_level = current_scope_level;
	new_sym->param_types = nullptr;
	new_sym->param_count = 0;

	new_sym->next = symbol_table_head;
	symbol_table_head = new_sym;

	return 1;
}

Symbol* lookup_symbol(char* name) {
	Symbol* current = symbol_table_head;

	while (current != nullptr) {
		if (strcmp(current->name, name) == 0) {
			return current;
		}
		current = current->next;
	}

	return nullptr;
}

Symbol* lookup_local_symbol(char* name) {
	Symbol* current = symbol_table_head;

	while (current != nullptr && current->scope_level == current_scope_level) {
		if (strcmp(current->name, name) == 0) {
			return current;
		}
		current = current->next;
	}

	return nullptr;
}

void print_symbol_table() {
	Symbol* current = symbol_table_head;
	while (current != nullptr) {
		const char* kind_str;
		if (current->kind == KIND_VAR)
			kind_str = "variable";
		else if (current->kind == KIND_FUNC)
			kind_str = "func";
		else if (current->kind == KIND_ARRAY)
			kind_str = "arr";
		else
			kind_str = "paramet";

		cout << "Name: " << current->name << " Type: " << current->type << " Kind: " << kind_str
			 << " Scope: " << current->scope_level << endl;
		current = current->next;
	}
}
