#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>

#include "ir.h"
#include "optimizer.h"

struct ReachIr : public IRInst {
	bool is_reachable = false;
	bool is_dead = false;
	ReachIr* next = nullptr;

	ReachIr(const IRInst& ir) : IRInst(ir) {}

	bool has_side_effects() const {
		switch (op) {
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
};

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

ReachIr* build_reach_ir_ll(IRInst*& head) {
	ReachIr* reach_head = new ReachIr(*head);
	if (head->next) {
		reach_head->next = new ReachIr(*head->next);
	}
	ReachIr* reach_curr = reach_head->next;

	for (IRInst* curr = head->next; curr; curr = curr->next) {
		if (curr->next) {
			reach_curr->next = new ReachIr(*curr->next);
		}
		reach_curr = reach_curr->next;
	}

	return reach_head;
}

std::unordered_map<std::string, ReachIr*> build_lookup(ReachIr*& head) {
	std::unordered_map<std::string, ReachIr*> label_lookup;

	for (ReachIr* curr = head; curr; curr = curr->next) {
		if (curr->op != IR_LABEL) continue;
		label_lookup[curr->arg1] = curr;
	}

	return label_lookup;
}

void mark_reachable(ReachIr*& head, const std::unordered_map<std::string, ReachIr*> label_lookup) {
	if (!head) return;

	std::stack<ReachIr*> work_stack;
	head->is_reachable = true;
	work_stack.push(head);

	auto mark_and_push = [&](ReachIr* target) {
		if (!target || target->is_reachable) return;
		target->is_reachable = true;
		work_stack.push(target);
	};

	while (!work_stack.empty()) {
		ReachIr* curr = work_stack.top();
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
		}
	}
}

int sweep_unreachable(ReachIr*& head) {
	ReachIr** curr_ptr = &head;
	int changes_made = 0;

	while (*curr_ptr) {
		ReachIr* curr = *curr_ptr;
		if (curr->is_reachable) {
			curr_ptr = &(*curr_ptr)->next;
			continue;
		}
		*curr_ptr = curr->next;
		delete curr;
		changes_made++;
	}

	return changes_made;
}

std::unordered_map<std::string, int> calculate_uses(ReachIr*& head) {
	std::unordered_map<std::string, int> use_counts;

	for (ReachIr* curr = head; curr; curr = curr->next) {
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

int sweep_dead_assignments(ReachIr*& head, std::unordered_map<std::string, int> use_counts) {
	ReachIr** curr_ptr = &head;
	int changes_made = 0;

	while (*curr_ptr != nullptr) {
		ReachIr* curr = *curr_ptr;

		if (curr->has_side_effects() || (!curr->result || (*curr->result == '\0')) ||
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

IRInst* rebuild_ir_ll(ReachIr*& reach_head) {
	IRInst* head =
		create_instruction(reach_head->op, reach_head->arg1, reach_head->arg2, reach_head->result);

	for (IRInst* curr_reach = reach_head->next; curr_reach; curr_reach = curr_reach->next) {
		IRInst* curr = create_instruction(
			curr_reach->op, curr_reach->arg1, curr_reach->arg2, curr_reach->result
		);
		append_instruction(head, curr);
	}

	return head;
}

int dead_code_elimination(IRInst* head) {
	if (!head) {
		return 0;
	}

	ReachIr* reach_head = build_reach_ir_ll(head);
	int changes_made = 0;

	std::unordered_map<std::string, ReachIr*> label_lookup = build_lookup(reach_head);
	mark_reachable(reach_head, label_lookup);
	changes_made += sweep_unreachable(reach_head);

	std::unordered_map<std::string, int> use_counts = calculate_uses(reach_head);
	changes_made += sweep_dead_assignments(reach_head, use_counts);

	free_ir_list(head);
	head = rebuild_ir_ll(reach_head);

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
