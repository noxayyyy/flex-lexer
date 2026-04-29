#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Translates the IR linked list into MIPS assembly and writes it to the specified file.
 * * @param ir_head: The head of the Intermediate Representation list
 * @param output_filename: The name of the file to write MIPS code to (e.g., "output.s")
 */
void generate_mips(IRInst *ir_head, char *output_filename);

#ifdef __cplusplus
}
#endif

#endif