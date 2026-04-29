#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "ir.h"
#include "optimizer.h"

void optimize_ir(IRInst* head) {
	int total_changes = 1;
	int dead_code;
	int strength;
	int licm;
	int propogated;
	int folding;

	int pass_count = 1;
	while (total_changes > 0) {
		printf("Pass#%d\n", pass_count);

		dead_code = dead_code_elimination(head);
		printf("\tDead Code Eliminated: %d\n", dead_code);

		strength = strength_reduction(head);
		printf("\tStrength Reduced: %d\n", strength);

		licm = loop_invariant_code_motion(head);
		printf("\tLICM: %d\n", licm);

		propogated = constant_propagation(head);
		printf("\tConstants Propogated: %d\n", propogated);

		folding = constant_folding(head);
		printf("\tConstants Folded: %d\n", folding);

		total_changes = dead_code + strength + licm + propogated + folding;
		printf("\tTotal Changes: %d\n\n", total_changes);

		pass_count++;
	}

	return;
}

int constant_folding(IRInst* head) {
	return 0;
}

int dead_code_elimination(IRInst* head) {
	return 0;
}

int constant_propagation(IRInst* head) {
	return 0;
}

int loop_invariant_code_motion(IRInst* head) {
	return 0;
}

int strength_reduction(IRInst* head) {
	return 0;
}
