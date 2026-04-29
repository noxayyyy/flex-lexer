#ifndef IR_H
#define IR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef enum {
	// Arithmetic
	IR_ADD,
	IR_SUB,
	IR_MUL,
	IR_DIV,
	IR_MOD,

	// Relational
	IR_GT,
	IR_LT,
	IR_GTE,
	IR_LTE,
	IR_EQ,
	IR_NEQ,

	// Logical
	IR_AND,
	IR_OR,
	IR_NOT,

	IR_ASSIGN, // x = y

	IR_IFZ,   // if (t1 == 0) goto L1
	IR_GOTO,  // goto L1
	IR_LABEL, // L1:

	IR_FUNC_START, // FUNCTION name:
	IR_PARAM,      // param x (argument for call)
	IR_POP_PARAM,  // (popping parameters off the stack in a function)
	IR_CALL,       // t1 = call func, num_args
	IR_RET,        // return x

	IR_ARRAY_LOAD,  // t1 = arr[i]
	IR_ARRAY_STORE, // arr[i] = t1

	IR_READ, // cin >> x --> read x
	IR_PRINT // cout << x; -->print x
} IROp;

typedef struct IRInst {
	IROp op;
	char* arg1;
	char* arg2;
	char* result;
	struct IRInst* next;
} IRInst;

IRInst* create_instruction(IROp op, char* arg1, char* arg2, char* result);
void append_instruction(IRInst* head, IRInst* new_inst);
void print_ir_list(IRInst* head);
void free_ir_list(IRInst* head);

char* new_temp();
char* new_label();

#ifdef __cplusplus
}
#endif

#endif
