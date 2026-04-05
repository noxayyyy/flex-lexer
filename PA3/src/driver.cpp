#include <cstdio>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

#include "../lib/ast.h"
#include "ir.h"
#include "ir_generator.h"
#include "semantics.h"

extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;

#ifdef __cplusplus
}
#endif

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		perror("Error opening file");
		return 1;
	}
	yyin = file;

	std::cout << "--- Phase 2: Syntax Analysis ---" << std::endl;

	if (yyparse() != 0) {
		std::cerr << "Parsing failed due to syntax errors." << std::endl;
		fclose(file);
		return 1;
	}
	std::cout << "Syntax Analysis Complete. AST Generated." << std::endl;

	std::cout << "\n--- Phase 3: Semantic Analysis ---" << std::endl;

	int error_count = semantic_analysis(root);

	if (error_count > 0) {
		std::cout << error_count << " semantic error(s) found" << std::endl;
		if (root) free_ast(root);
		fclose(file);
		return 1;
	} else {
		std::cout << "No semantic errors found" << std::endl;

		std::cout << "\n--- Phase 4: IR Generation ---" << std::endl;

		IRInst* ir_list = generate_ir_from_ast(root);

		if (ir_list) {
			print_ir_list(ir_list);
			free_ir_list(ir_list);
		} else {
			std::cout << "No IR generated." << std::endl;
		}
	}

	if (root) {
		free_ast(root);
	}
	fclose(file);

	return 0;
}
