#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Runs all optimization passes repeatedly until no more changes occur.
 */
void optimize_ir(IRInst *head);

/* --- Individual Passes (You implement these in optimizer.c) --- */

int constant_folding(IRInst *head);
int dead_code_elimination(IRInst *head);
int constant_propagation(IRInst *head);
int loop_invariant_code_motion(IRInst *head);
int strength_reduction(IRInst *head);

#ifdef __cplusplus
}
#endif

#endif