#ifndef __RASM_INSTRU_H
#define __RASM_INSTRU_H
#include "rasm.h"
#include "utils.h"

void instrulyze(Instruction * instru);
void instrufree(Instruction * instru);
void update_location(u64 bsize);

#endif // __RASM_INSTRU_H
