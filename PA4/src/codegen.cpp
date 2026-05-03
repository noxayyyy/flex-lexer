#include "codegen.h"
#include "ir.h"
#include "symbol_table.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>

std::string generate_data(IRInst* ir_head);
std::string generate_text(IRInst* ir_head);

void generate_mips(IRInst* ir_head, char* output_filename) {
	FILE* out = fopen(output_filename, "w");

	fprintf(out, ".data\n");

	// Loop through IR, find all variables/temps, print "name: .word 0"
	const std::string data_segment = generate_data(ir_head);
	fprintf(out, "%s\n", data_segment.c_str());

	fprintf(out, ".text\n");
	fprintf(out, ".globl main");

	// Loop through the IR again to generate instructions
	const std::string text_segment = generate_text(ir_head);
	fprintf(out, "%s\n", text_segment.c_str());

	// exit syscall
	fprintf(out, "li $v0, 10\n");
	fprintf(out, "syscall\n");

	fclose(out);

	// out = fopen(output_filename, "r");
	//
	// int ch;
	// while ((ch = fgetc(out)) != EOF) {
	// 	putchar(ch);
	// }
	// std::cout << '\n';
	//
	// fclose(out);
}

bool is_var(const char* str) {
	return str ? std::isalpha(str[0]) || str[0] == '_' : false;
}

std::string safe_var(const std::string& var) {
	return "var_" + var;
}

void load_arg(std::string& out, const char* arg, const std::string& reg) {
	if (!arg) return;

	out += (is_var(arg) ? "lw " : "li ") + reg + ", " + safe_var(arg) + "\n";
}

void store_result(std::string& out, const char* result, const std::string& reg) {
	if (!result) return;

	out += "sw " + reg + ", " + safe_var(result) + "\n";
}

void three_arg_inst(
	std::string& out, const char* op, const char* arg1, const char* arg2, const char* result
) {
	load_arg(out, arg1, "$t0");
	load_arg(out, arg2, "$t1");
	out += std::string(op) + " $t2, $t0, $t1\n";
	store_result(out, result, "$t2");
}

void insert_if_var(std::set<std::string>& vars, const char* arg) {
	if (is_var(arg)) {
		vars.insert(arg);
	}
}

std::string generate_data(IRInst* ir_head) {
	std::set<std::string> vars;
	std::unordered_set<std::string> labels;

	for (IRInst* curr = ir_head; curr; curr = curr->next) {
		switch (curr->op) {
		case IR_LABEL:
		case IR_FUNC_START:
			labels.insert(curr->result);
			break;
		case IR_GOTO:
			break;
		case IR_IFZ:
			insert_if_var(vars, curr->arg1);
			break;
		case IR_CALL:
			insert_if_var(vars, curr->result);
			break;
		case IR_ARRAY_LOAD:
		case IR_ARRAY_STORE:
		default:
			insert_if_var(vars, curr->arg1);
			insert_if_var(vars, curr->arg2);
			insert_if_var(vars, curr->result);
			break;
		}
	}

	std::string data_segment;
	for (const auto& v : vars) {
		if (labels.find(v) != labels.end()) continue;
		data_segment += safe_var(v) + ": .word 0\n";
	}

	return data_segment;
}

std::string generate_text(IRInst* ir_head) {
	std::string text_segment;

	for (IRInst* curr = ir_head; curr; curr = curr->next) {
		switch (curr->op) {
		// Arithmetic
		case IR_ADD:
			three_arg_inst(text_segment, "add", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_SUB:
			three_arg_inst(text_segment, "sub", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_MUL:
			three_arg_inst(text_segment, "mul", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_DIV:
			load_arg(text_segment, curr->arg1, "$t0");
			load_arg(text_segment, curr->arg2, "$t1");
			text_segment += "div $t0, $t1\nmflo $t2\n";
			store_result(text_segment, curr->result, "$t2");
			break;
		case IR_MOD:
			load_arg(text_segment, curr->arg1, "$t0");
			load_arg(text_segment, curr->arg2, "$t1");
			text_segment += "div $t0, $t1\nmfhi $t2\n";
			store_result(text_segment, curr->result, "$t2");
			break;

		// Relational
		case IR_GT:
			three_arg_inst(text_segment, "sgt", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_LT:
			three_arg_inst(text_segment, "slt", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_GTE:
			three_arg_inst(text_segment, "sge", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_LTE:
			three_arg_inst(text_segment, "sle", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_EQ:
			three_arg_inst(text_segment, "seq", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_NEQ:
			three_arg_inst(text_segment, "sne", curr->arg1, curr->arg2, curr->result);
			break;

		// Logical
		case IR_AND:
			three_arg_inst(text_segment, "and", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_OR:
			three_arg_inst(text_segment, "or", curr->arg1, curr->arg2, curr->result);
			break;
		case IR_NOT:
			load_arg(text_segment, curr->arg1, "$t0");
			text_segment += "seq $t2, $t0, $zero\n";
			store_result(text_segment, curr->result, "$t2");
			break;

		// Assignment
		case IR_ASSIGN:
			load_arg(text_segment, curr->arg1, "$t0");
			store_result(text_segment, curr->result, "$t0");
			break;

		// Branch
		case IR_IFZ:
			load_arg(text_segment, curr->arg1, "$t0");
			text_segment += "beqz $t0, " + std::string(curr->result) + "\n";
			break;
		case IR_GOTO:
			text_segment += "j " + std::string(curr->result) + "\n";
			break;
		case IR_LABEL:
			text_segment += std::string(curr->result) + ":\n";
			break;

		// Function
		case IR_FUNC_START:
			text_segment += "\n" + std::string(curr->result) + ":\n";
			break;
		case IR_PARAM:
			load_arg(text_segment, curr->arg1, "$t0");
			text_segment += "sub $sp, $sp, 4\nsw $t0, ($sp)\n";
			break;
		case IR_POP_PARAM:
			text_segment += "lw $t0, ($sp)\nadd $sp, $sp, 4\n";
			store_result(text_segment, curr->result, "$t0");
			break;
		case IR_CALL:
			text_segment += "jal " + std::string(curr->arg1) + "\n";
			store_result(text_segment, curr->result, "$v0");
			break;
		case IR_RET:
			load_arg(text_segment, curr->result, "$v0");
			text_segment += "jr $ra\n";
			break;

		// Array
		case IR_ARRAY_LOAD:
			load_arg(text_segment, curr->arg2, "$t0");
			text_segment += "sll $t0, $t0, 2\n";
			text_segment += "la $t1, " + safe_var(curr->arg1) + "\n";
			text_segment += "add $t1, $t1, $t0\n";
			text_segment += "lw $t2, ($t1)\n";
			store_result(text_segment, curr->result, "$t2");
			break;
		case IR_ARRAY_STORE:
			load_arg(text_segment, curr->arg1, "$t0");
			text_segment += "sll $t0, $t0, 2\n";
			text_segment += "la $t1, " + safe_var(curr->arg1) + "\n";
			text_segment += "add $t1, $t1, $t0\n";
			load_arg(text_segment, curr->arg2, "$t2");
			text_segment += "sw $t2, ($t1)\n";
			break;

		// I/O
		case IR_READ:
			break;
		case IR_PRINT:
			text_segment += "li $v0, 1\n";
			load_arg(text_segment, curr->arg1, "$a0");
			text_segment += "syscall\n";
			break;

		default:
			break;
		}
	}

	return text_segment;
}
