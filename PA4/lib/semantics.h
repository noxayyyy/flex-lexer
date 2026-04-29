#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

#ifdef __cplusplus
extern "C" {
#endif

int semantic_analysis(ASTNode* root);

#ifdef __cplusplus
}
#endif

#endif