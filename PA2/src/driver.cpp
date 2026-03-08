#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ast.h"         
#include "parser.tab.h"  

extern int yyparse();    
extern ASTNode* root;    

#ifdef __cplusplus
}
#endif

int main(int argc, char **argv) {

    int result = yyparse();

    if (result == 0) {
        printf("--- AST Structure ---\n");
        print_ast(root, 0);
        free_ast(root);
    } else {
        printf("--- Parse Failed! ---\n");
    }

    return result;
}