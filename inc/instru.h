#ifndef __INSTRU_H
#define __INSTRU_H
#include "header.h"
#include "table.h"
#include "utils.h"
#include "str.h"

void update_location(u64 byte_size);
void instrulyze(Instruction * instru);
void instrufree(Instruction * instru);



Operand * parse_operand(void);
void free_operand(Operand * operand);

void parse_two_operands(Instruction * instru);
void parse_only_operand(Instruction * instru);
void parse_jump_family(Instruction * instru);
void parse_byte_reference(Operand * operand);
void parse_word_reference(Operand * operand);
char * operand_string(Operand * operand);

void parse_db(Operand * operand);
void parse_dw(Operand * operand);
void parse_define_byte(Instruction * instru);
void parse_define_word(Instruction * instru);
void parse_section(Instruction * instru);

Instruction * assemble(void);
void process(void);

#endif // __INSTRU_H
