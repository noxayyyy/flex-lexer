#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ir.h"
#include "optimizer.h"

void optimize_ir(IRInst* head) {
	int total_changes = 1;
	int dead_code;
	int strength;
	int licm;
	int propogated;
	int folding;

	int g_total_changes = 0;
	int g_dead_code = 0;
	int g_strength = 0;
	int g_licm = 0;
	int g_propogated = 0;
	int g_folding = 0;

	int pass_count = 1;
	while (total_changes > 0) {
		printf("Pass#%d\n", pass_count);

		dead_code = dead_code_elimination(head);
		printf("\tDead Code Eliminated: %d\n", dead_code);
		g_dead_code += dead_code;

		strength = strength_reduction(head);
		printf("\tStrength Reduced: %d\n", strength);
		g_strength += strength;

		licm = loop_invariant_code_motion(head);
		printf("\tLICM: %d\n", licm);
		g_licm += licm;

		propogated = constant_propagation(head);
		printf("\tConstants Propogated: %d\n", propogated);
		g_propogated += propogated;

		folding = constant_folding(head);
		printf("\tConstants Folded: %d\n", folding);
		g_folding += folding;

		total_changes = dead_code + strength + licm + propogated + folding;
		printf("\tTotal Changes: %d\n\n", total_changes);
		g_total_changes += total_changes;

		pass_count++;
	}

	printf("Total\n");
	printf("\tDead Code Eliminated: %d\n", g_dead_code);
	printf("\tStrength Reduced: %d\n", g_strength);
	printf("\tLICM: %d\n", g_licm);
	printf("\tConstants Propogated: %d\n", g_propogated);
	printf("\tConstants Folded: %d\n", g_folding);
	printf("\tTotal Changes: %d\n\n", g_total_changes);

	return;
}

int constant_folding(IRInst* head) {
	return 0;
}

std::unordered_map<std::string, IRInst*> build_lookup(IRInst*& head) {
	std::unordered_map<std::string, IRInst*> label_lookup;

	for (IRInst* curr = head; curr; curr = curr->next) {
		if (curr->op != IR_LABEL || !curr->arg1) continue;
		label_lookup[curr->arg1] = curr;
	}

	return label_lookup;
}

std::unordered_set<IRInst*>
get_reachable(IRInst*& head, const std::unordered_map<std::string, IRInst*> label_lookup) {
	if (!head) {
		return std::unordered_set<IRInst*>();
	}

	std::unordered_set<IRInst*> reachable;
	std::stack<IRInst*> work_stack;

	reachable.insert(head);
	work_stack.push(head);

	auto mark_and_push = [&](IRInst* target) {
		if (!target || reachable.count(target)) return;
		reachable.insert(target);
		work_stack.push(target);
	};

	while (!work_stack.empty()) {
		IRInst* curr = work_stack.top();
		work_stack.pop();

		switch (curr->op) {
		case IR_GOTO:
			mark_and_push(label_lookup.at(curr->arg1));
			break;
		case IR_IFZ:
			mark_and_push(label_lookup.at(curr->result));
			mark_and_push(curr->next);
			break;
		case IR_RET:
			break;
		default:
			mark_and_push(curr->next);
			break;
		}
	}

	return reachable;
}

int sweep_unreachable(IRInst*& head, std::unordered_set<IRInst*> reachable) {
	IRInst** curr_ptr = &head;
	int changes_made = 0;

	while (*curr_ptr) {
		IRInst* curr = *curr_ptr;
		if (reachable.count(curr)) {
			curr_ptr = &(*curr_ptr)->next;
			continue;
		}
		*curr_ptr = curr->next;
		delete curr;
		changes_made++;
	}

	return changes_made;
}

std::unordered_map<std::string, int> calculate_uses(IRInst*& head) {
	std::unordered_map<std::string, int> use_counts;

	for (IRInst* curr = head; curr; curr = curr->next) {
		if (curr->arg1 && (*curr->arg1 != '\0')) {
			use_counts[curr->arg1]++;
		}
		if (curr->arg2 && (*curr->arg2 != '\0')) {
			use_counts[curr->arg2]++;
		}

		if (curr->result && (*curr->result != '\0') &&
			(curr->op == IR_IFZ || curr->op == IR_PRINT || curr->op == IR_PARAM ||
			 curr->op == IR_ARRAY_STORE)) {
			use_counts[curr->result]++;
		}
	}

	return use_counts;
}

bool has_side_effects(IRInst* inst) {
	switch (inst->op) {
	case IR_IFZ:
	case IR_GOTO:
	case IR_LABEL:
	case IR_FUNC_START:
	case IR_PARAM:
	case IR_POP_PARAM:
	case IR_CALL:
	case IR_RET:
	case IR_ARRAY_STORE:
	case IR_READ:
	case IR_PRINT:
		return true;
	default:
		return false;
	}
}

int sweep_dead_assignments(IRInst*& head, std::unordered_map<std::string, int> use_counts) {
	IRInst** curr_ptr = &head;
	int changes_made = 0;

	while (*curr_ptr != nullptr) {
		IRInst* curr = *curr_ptr;

		if (has_side_effects(curr) || (!curr->result || (*curr->result == '\0')) ||
			use_counts[curr->result]) {
			curr_ptr = &(*curr_ptr)->next;
			continue;
		}

		*curr_ptr = curr->next;
		delete curr;
		changes_made++;
	}

	return changes_made;
}

int dead_code_elimination(IRInst* head) {
	if (!head) {
		return 0;
	}

	int changes_made = 0;

	std::unordered_map<std::string, IRInst*> label_lookup = build_lookup(head);
	std::unordered_set<IRInst*> reachable = get_reachable(head, label_lookup);
	changes_made += sweep_unreachable(head, reachable);

	int assignments_changed = 1;
	while (assignments_changed) {
		std::unordered_map<std::string, int> use_counts = calculate_uses(head);
		assignments_changed = sweep_dead_assignments(head, use_counts);
		changes_made += assignments_changed;
	}

	return changes_made;
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
