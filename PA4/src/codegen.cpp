#include <string>
#include <cctype>
#include "codegen.h"
#include "ir.h"
#include "symbol_table.h" 

using namespace std;

void generate_mips(IRInst *ir_head, char *output_filename)
{
    FILE *out = fopen(output_filename, "w");

    fprintf(out, ".data\n");

    // Loop through IR, find all variables/temps, print "name: .word 0"
    
    fprintf(out, ".text\n");
    fprintf(out, ".globl main\n");

    // Loop through the IR again to generate instructions

    // exit syscall
    fprintf(out, "li $v0, 10\n");
    fprintf(out, "syscall\n");

    fclose(out);
}