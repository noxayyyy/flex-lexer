#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "ast.h"
#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

IRInst *generate_ir_from_ast(ASTNode *root);

#ifdef __cplusplus
}
#endif

#endif