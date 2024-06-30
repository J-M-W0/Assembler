#ifndef __RASM_OPERAND_H
#define __RASM_OPERAND_H
#include "rasm.h"

Operand * analyze_operand(void);
void analyze_2_operands(Instruction * instru);
void analyze_1_operand(Instruction * instru);
void analyze_jump_operand(Instruction * instru);
void analyze_byte_reference(Operand * op);
void analyze_word_reference(Operand * op);
void analyze_db(Instruction * instru);
void analyze_dw(Instruction * instru);
void analyze_section(Instruction * instru);
void operandfree(Operand * op);
char * operandstring(Operand * op);

#endif // __RASM_OPERAND_H
