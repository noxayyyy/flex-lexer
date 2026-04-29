#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "ast.h"
#include "semantics.h"
#include "symbol_table.h"
#include "ir.h"
#include "optimizer.h"
#include "codegen.h"
#include "ir_generator.h"

extern "C" {
    extern int yyparse();
    extern FILE *yyin;
}

extern ASTNode *root;
extern IRInst *generate_ir_from_ast(ASTNode *root);

char *get_output_filename(const char *input_filename) {
    const char *dot = strrchr(input_filename, '.');
    if (!dot || dot == input_filename) {
        char *dup = (char*)malloc(strlen("output.s") + 1);
        strcpy(dup, "output.s");
        return dup;
    }
    
    size_t len = dot - input_filename;
    char *new_name = (char*)malloc(len + 3);
    strncpy(new_name, input_filename, len);
    strcpy(new_name + len, ".s");
    return new_name;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }
    yyin = file;

    printf("--- Phase 2: Syntax Analysis ---\n");
    if (yyparse() != 0)
    {
        printf("Parsing failed due to syntax errors.\n");
        fclose(file);
        return 1;
    }
    printf("Syntax Analysis Complete. AST Generated.\n");

    printf("\n--- Phase 3: Semantic Analysis ---\n");
    int error_count = semantic_analysis(root);

    if (error_count > 0)
    {
        printf("%d semantic error(s) found. Compilation halted.\n", error_count);
        if (root) free_ast(root);
        fclose(file);
        return 1;
    }
    
    printf("No semantic errors found.\n");

    printf("\n--- Phase 4: IR Generation ---\n");
    IRInst *ir_list = generate_ir_from_ast(root);

    if (!ir_list) {
        printf("No IR generated\n");
        if (root) free_ast(root);
        fclose(file);
        return 0;
    }
    
    printf("\n--- Phase 4b: Optimization ---\n");
    optimize_ir(ir_list);
    printf("Optimized IR:\n");
    print_ir_list(ir_list); 

    printf("\n--- Phase 5: MIPS Code Generation ---\n");
    
    char *output_file;
    if (argc >= 3) {
        output_file = strdup(argv[2]);
    } else {
        output_file = get_output_filename(argv[1]);
    }
    generate_mips(ir_list, output_file);
    printf("MIPS Assembly generated\n");

    free(output_file);
    free_ir_list(ir_list);
    if (root) free_ast(root);
    fclose(file);

    return 0;
}