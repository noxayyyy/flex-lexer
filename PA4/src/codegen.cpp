#include "codegen.h"
#include "ir.h"
#include "symbol_table.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

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

std::string generate_data(IRInst* ir_head) {
	std::string data_segment;

	while (ir_head) {
		if (ir_head->op != IR_ASSIGN) {
			ir_head = ir_head->next;
			continue;
		}

		data_segment += std::string(ir_head->result) + ": .word 0\n";
		ir_head = ir_head->next;
	}

	return data_segment;
}

void assign_inst(std::string& out, const IRInst* inst) {
	if (!inst || !inst->arg1) return;

	std::string arg = inst->arg1;
	if (arg[0] == '-') {
		arg.erase(0, 1);
	}

	if (std::all_of(arg.begin(), arg.end(), [](char a) { return std::isdigit(a); })) {
		out += "li " + std::string(inst->result) + ", " + inst->arg1 + "\n";
		return;
	}
	out += "lw " + std::string(inst->result) + ", " + inst->arg1 + "\n";
}

void three_arg_inst(
	std::string& out, const char* op, const char* arg1, const char* arg2, const char* result
) {
	out += std::string(op) + " " + result + ", " + arg1 + ", " + arg2 + "\n";
}

std::string generate_text(IRInst* ir_head) {
	std::string text_segment;

	while (ir_head) {
		switch (ir_head->op) {
		// Arithmetic
		case IR_ADD:
			three_arg_inst(text_segment, "add", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_SUB:
			three_arg_inst(text_segment, "sub", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_MUL:
			three_arg_inst(text_segment, "mul", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_DIV:
			text_segment += "div " + std::string(ir_head->arg1) + ", " + ir_head->arg2 + "\nmflo " +
							ir_head->result + "\n";
			break;
		case IR_MOD:
			text_segment += "div " + std::string(ir_head->arg1) + ", " + ir_head->arg2 + "\nmfhi " +
							ir_head->result + "\n";
			break;

		// Relational
		case IR_GT:
			three_arg_inst(text_segment, "sgt", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_LT:
			three_arg_inst(text_segment, "slt", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_GTE:
			three_arg_inst(text_segment, "sge", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_LTE:
			three_arg_inst(text_segment, "sle", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_EQ:
			three_arg_inst(text_segment, "seq", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_NEQ:
			three_arg_inst(text_segment, "sne", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;

		// Logical
		case IR_AND:
			three_arg_inst(text_segment, "and", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_OR:
			three_arg_inst(text_segment, "or", ir_head->arg1, ir_head->arg2, ir_head->result);
			break;
		case IR_NOT:
			three_arg_inst(text_segment, "nor", ir_head->arg1, "0", ir_head->result);
			break;

		// Assignment
		case IR_ASSIGN:
			assign_inst(text_segment, ir_head);
			break;

		// Branch
		case IR_IFZ:
			text_segment += "beqz " + std::string(ir_head->arg1) + ", " + ir_head->result + "\n";
			break;
		case IR_GOTO:
			text_segment += "j " + std::string(ir_head->result) + "\n";
			break;
		case IR_LABEL:
			text_segment += std::string(ir_head->result) + ":\n";
			break;

		// Function
		case IR_FUNC_START:
			text_segment += "\n" + std::string(ir_head->result) + ":\n";
			break;
		case IR_PARAM:
			text_segment += "sw " + std::string(ir_head->arg1) + ", ($sp)\n";
			break;
		case IR_POP_PARAM:
			text_segment += "lw " + std::string(ir_head->result) + ", ($sp)\n";
			break;
		case IR_CALL:
			text_segment += "jal " + std::string(ir_head->arg1) + "\n";
			break;
		case IR_RET:
			text_segment += "jr $ra\n";
			break;

		// Array
		case IR_ARRAY_LOAD:
			break;
		case IR_ARRAY_STORE:
			break;

		// I/O
		case IR_READ:
			break;
		case IR_PRINT:
			text_segment += "li $v0, 1\nlw $a0, " + std::string(ir_head->arg1) + "\nsyscall\n";
			break;

		default:
			break;
		}

		ir_head = ir_head->next;
	}

	return text_segment;
}
