#include "instru.h"
#include "rasm.h"
#include "str.h"
#include "utils.h"
#include "table.h"
#include "operand.h"
#include <math.h>

extern TTable * Labels;
extern u64 Section;
extern u64 Location;
extern const char * const instru_opcode_list[];

static void instru_setra(const char * reg, char * opcode) {
    assertf(reg != nil);
    assertf(opcode != nil);


    int i = strloc(opcode, "a");
    char * ra = strdup(register_code(reg));
    assertf(i != -1);
    assertf(ra != nil);
    

    for (int j = 0; j < 4; j++) {
        opcode[i + j] = ra[j];
    }
    

    free(ra);
}
static void instru_setrb(const char * reg, char * opcode) {
    assertf(reg != nil);
    assertf(opcode != nil);
    

    int i = strloc(opcode, "b");
    char * rb = strdup(register_code(reg));
    assertf(i != -1);
    assertf(rb != nil);
    

    for (int j = 0; j < 4; j++) {
        opcode[i + j] = rb[j];
    }
    

    free(rb);
}
static void instru_setimm4(const char * imm4, char * opcode) {
    assertf(imm4 != nil);
    assertf(opcode != nil);
    

    int i = strloc(opcode, "x");
    int imm = strtoi(imm4);
    char * code = itostr(imm, 4);
    assertf(i != -1);
    assertf(code != nil);
    

    for (int j = 0; j < 4; j++) {
        opcode[i + j] = code[j];
    }
    

    free(code);
}
static void instru_setimm8(const char * imm8, char * opcode) {
    assertf(imm8 != nil);
    assertf(opcode != nil);
    

    int i = strloc(opcode, "x");
    int imm = strtoi(imm8);
    char * code = itostr(imm, 8);
    assertf(i != -1);
    assertf(code != nil);
    

    for (int j = 0; j < 9; j++) {
        opcode[i + j] = code[j];
    }
    

    free(code);
}
static void instru_setimm16(const char * imm16, char * opcode) {
    assertf(imm16 != nil);
    assertf(opcode != nil);
    

    int i = strloc(opcode, "x");
    int imm = strtoi(imm16);
    char * code = itostr(imm, 16);
    assertf(i != -1);
    assertf(code != nil);
    

    for (int j = 0; j < 19; j++) {
        opcode[i + j] = code[j];
    }
    

    free(code);
}
static void instru_add_ra_imm(const char * ra, int imm) {
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    assertf(ra != nil);
    
    instru->Tag = ADD4;
    instru->next = nil;
    instru->left = malloc(sizeof(Operand));
    instru->right = malloc(sizeof(Operand));
    assertf(instru->left != nil);
    assertf(instru->right != nil);
    
    instru->left->Tag = operand_register;
    instru->left->Register = strdup(ra);
    instru->right->Tag = operand_imm;
    instru->right->IMM = itostr(imm, -1);
    
    instrulyze(instru);
    instrufree(instru);
}
static void instru_mov_ra_imm(const char * ra, int imm) {
    assertf(ra != nil);
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->Tag = MOV;
    instru->next = nil;
    instru->left = malloc(sizeof(Operand));
    instru->right = malloc(sizeof(Operand));
    assertf(instru->left != nil);
    assertf(instru->right != nil);
    instru->left->Tag = operand_register;
    instru->left->Register = strdup(ra);
    instru->right->Tag = operand_imm;
    instru->right->IMM = itostr(imm, -1);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_mov_ra_rb(const char * ra, const char * rb) {
    assertf(ra != nil);
    assertf(rb != nil);
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->Tag = MOV6;
    instru->next = nil;
    instru->left = malloc(sizeof(Operand));
    instru->right = malloc(sizeof(Operand));
    assertf(instru->left != nil);
    assertf(instru->right != nil);
    instru->left->Tag = operand_register;
    instru->left->Register = strdup(ra);
    instru->right->Tag = operand_register;
    instru->right->Register = strdup(rb);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_push_ra(const char * ra) {
    assertf(ra != nil);
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->Tag = PUSH;
    instru->next = nil;
    instru->single = malloc(sizeof(Operand));
    assertf(instru->single != nil);
    instru->single->Tag = operand_register;
    instru->single->Register = strdup(ra);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_pop_ra(const char * ra) {
    assertf(ra != nil);
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->Tag = POP;
    instru->next = nil;
    instru->single = malloc(sizeof(Operand));
    assertf(instru->single != nil);
    instru->single->Tag = operand_register;
    instru->single->Register = strdup(ra);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_pop0(void) {
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->Tag = POP0;
    instru->next = nil;
    instru->single = nil;
    instrulyze(instru);
    instrufree(instru);
}
static void instru_jmp_label(const char * labelname) {
    assertf(labelname != nil, "");
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil, "");
    instru->Tag = JMP2;
    instru->next = nil;
    instru->single = malloc(sizeof(Operand));
    assertf(instru->single != nil, "");
    instru->single->Tag = operand_j_label;
    instru->single->Label = strdup(labelname);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_jmp_ra_rb(const char * ra, const char * rb) {
    assertf(ra != nil, "");
    assertf(rb != nil, "");
    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil, "");
    instru->Tag = JMP2;
    instru->next = nil;
    instru->single = malloc(sizeof(Operand));
    assertf(instru->single != nil, "");
    instru->single->Tag = operand_j_seg_register;
    instru->single->Segment = strdup(ra);
    instru->single->Offset = strdup(rb);
    instrulyze(instru);
    instrufree(instru);
}

void update_location(u64 bsize) {
    assertf(Section <= UINT8_MAX);
    Location += bsize;
    if (Location > UINT16_MAX) {
        Section += 1;
        Location -= UINT16_MAX;
    }
}

void instrulyze(Instruction * instru) {
    if (!instru) {
        return;
    }
    TInstru tag = instru->Tag;
    switch (tag) {
        case ADD:   // add ra, rb
        case AND:   // and ra, rb
        case OR:    // or  ra, rb
        case XOR:   // xor ra, rb
        case CMP:   // cmp ra, rb
        case SUB:   // sub ra, rb
        case ADC:   // adc ra, rb
        case MUL:   // mul ra, rb
        case DIV:   // div ra, rb
        case MOD:   // mod ra, rb
        case MOV6:  // mov ra, rb
        case XCHG:  // xchg ra, rb
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setra(left->Register, opcode);
            instru_setrb(right->Register, opcode);

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case AND2:  // and ra, word [ds:rb]
        case OR2:   // or  ra, word [ds:rb]
        case XOR2:  // xor ra, word [ds:rb]
        case CMP2:  // cmp ra, word [ds:rb]
        case ADC2:  // adc ra, word [ds:rb]
        case MUL2:  // mul ra, word [ds:rb]
        case DIV2:  // div ra, word [ds:rb]
        case MOD2:  // mod ra, word [ds:rb]
        case ADD2:  // add ra, byte [ds:rb]
        case ADD3:  // add ra, word [ds:rb]
        case MOV4:  // mov ra, byte [ds:rb]
        case MOV5:  // mov ra, word [ds:rb]
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setra(left->Register, opcode);
            if (right->Tag == operand_byte_label || right->Tag == operand_word_label) 
            {
                u64 address = Table_Get(Labels, right->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                // push ds
                instru_push_ra("ds");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setrb("r12", opcode);
                address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);

                // pop ds
                instru_pop_ra("ds");
            }
            else if (right->Tag == operand_byte_seg_register || right->Tag == operand_word_seg_register) 
            {
                instru_setrb(right->Offset, opcode);
                
                u64 address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else {
                abortf("");
            }

            free(opcode);
        } break;

        case MOV:   // mov ra, imm16
        case SUB2:  // sub ra, imm16
        case AND3:  // and ra, imm16
        case OR3:   // or  ra, imm16
        case XOR3:  // xor ra, imm16
        case CMP3:  // cmp ra, imm16
        case ADC3:  // adc ra, imm16
        case MUL3:  // mul ra, imm16
        case DIV3:  // div ra, imm16
        case MOD3:  // mod ra, imm16
        case ADD4:  // add ra, imm16
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setra(left->Register, opcode);
            if (right->Tag == operand_imm) {
                instru_setimm16(instru->right->IMM, opcode);
            }
            else if (right->Tag == operand_label_sectionpart) {
                u64 address = Table_Get(Labels, right->Label);
                int upper8 = (address >> 16) & 0xff;
                char * imm16 = itostr(upper8, -1);
                instru_setimm16(imm16, opcode);
                free(imm16);
            }
            else if (right->Tag == operand_label_offsetpart) {
                u64 address = Table_Get(Labels, right->Label);
                int lower16 = (address & 0xffff) / 2;
                char * imm16 = itostr(lower16, -1);
                instru_setimm16(imm16, opcode);
                free(imm16);
            }
            else {
                abortf("");
            }

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case AND4:  // and word [ds:ra], rb
        case OR4:   // or  word [ds:ra], rb
        case XOR4:  // xor word [ds:ra], rb
        case MUL4:  // mul word [ds:ra], rb
        case DIV4:  // div word [ds:ra], rb
        case MOD4:  // mov word [ds:ra], rb
        case ADD5:  // add word [ds:ra], rb
        case MOV8:  // mov word [ds:ra], rb
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setrb(right->Register, opcode);

            if (left->Tag == operand_word_label) {
                u64 address = Table_Get(Labels, left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;

                // push ds
                instru_push_ra("ds");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r12", opcode);
                address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);

                // pop ds
                instru_pop_ra("ds");
            }
            else if (left->Tag == operand_word_seg_register) {
                instru_setra(left->Offset, opcode);
                u64 address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else {
                abortf("");
            }
            
            free(opcode);
        } break;

        case CMP4:  // cmp word [ra:rb], imm16
        case AND5:  // and word [ra:rb], imm16
        case OR5:   // or  word [ra:rb], imm16
        case XOR5:  // xor word [ra:rb], imm16
        case MUL5:  // mul word [ra:rb], imm16
        case DIV5:  // div word [ra:rb], imm16
        case MOD5:  // mod word [ra:rb], imm16
        case ADD6:  // add word [ra:rb], imm16
        case MOV7:  // mov word [ra:rb], imm16
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * left = instru->left;
            Operand * right = instru->right;

            TOperand ltag = left->Tag;
            TOperand rtag = right->Tag;
            if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru_setra(left->Register, opcode);
                instru_setrb(left->Offset, opcode);
                instru_setimm16(right->IMM, opcode);
                
                u64 address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru_setra(left->Register, opcode);
                instru_setrb(left->Offset, opcode);
                
                int imm = Table_Get(Labels, right->Label);
                imm = (imm >> 16) & 0xff;
                char * IMM = itostr(imm, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);
                
                u64 address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru_setra(left->Register, opcode);
                instru_setrb(left->Offset, opcode);
                
                int imm = Table_Get(Labels, right->Label);
                imm = (imm & 0xffff) / 2;
                char * IMM = itostr(imm, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);
                
                u64 address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else if (ltag==operand_word_label && rtag == operand_imm) {
                u64 address = Table_Get(Labels, left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                
                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
                instru_setimm16(right->IMM, opcode);
                
                address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                u64 address = Table_Get(Labels, left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                
                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
                int imm = Table_Get(Labels, right->Label);
                imm = (imm >> 16) & 0xff;
                char * IMM = itostr(imm, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);

                address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                u64 address = Table_Get(Labels, left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                
                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
                int imm = Table_Get(Labels, right->Label);
                imm = (imm & 0xffff) / 2;
                char * IMM = itostr(imm, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);

                address = (Section << 16) + (Location / 2);
                writeln("\t(0x%lx)\n%s\n", address, opcode);
                update_location(bsize);
            }
            else {
                abortf("");
            }

            free(opcode);
        } break;

        case MOV2:  // mov ra, byte [rb:imm16]
        case MOV3:  // mov ra, word [rb:imm16]
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setra(left->Register, opcode);
            instru_setrb(right->Segment, opcode);
            instru_setimm16(right->Offset, opcode);

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case SHR:   // shr ra, imm4
        case SHL:   // shl ra, imm4
        case SAR:   // sar ra, imm4
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setra(left->Register, opcode);
            instru_setimm4(right->IMM, opcode);

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;
        
        case SHR2:  // shr word [ra:rb], imm4
        case SHL2:  // shl word [ra:rb], imm4
        case SAR2:  // sar word [ra:rb], imm4
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            Operand * left = instru->left;
            Operand * right = instru->right;

            instru_setimm4(right->IMM, opcode);
            if (left->Tag == operand_word_seg_register) {
                instru_setra(left->Segment, opcode);
                instru_setrb(left->Offset, opcode);
            }
            else if (left->Tag == operand_word_label) {
                u64 address = Table_Get(Labels, left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                
                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
            }
            else {
                abortf("");
            }

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case FLIP:  // flip ra
        case NEG:   // neg  ra
        case PUSH:  // push ra
        case POP:   // pop  ra
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * op = instru->single;
            instru_setra(op->Register, opcode);

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case FLIP2: // flip word [ra:rb]
        case NEG2:  // neg  word [ra:rb]
        case PUSH3: // push byte [ra:rb]
        case PUSH4: // push word [ra:rb]
        case POP2:  // pop  byte [ra:rb]
        case POP3:  // pop  word [ra:rb]
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * op = instru->single;
            TOperand tag = op->Tag;
            if (tag==operand_word_seg_register || tag==operand_byte_seg_register) {
                instru_setra(op->Segment, opcode);
                instru_setrb(op->Offset, opcode);
            }
            else if (tag==operand_word_label   || tag==operand_byte_label) {
                u64 address = Table_Get(Labels, op->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;
                
                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
            }
            else {
                abortf("");
            }
            
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case PUSH2: // push imm16
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * op = instru->single;
            TOperand tag = op->Tag;
            if (tag == operand_imm) {
                instru_setimm16(op->IMM, opcode);
            }
            else if (tag == operand_label_sectionpart) {
                int imm = Table_Get(Labels, op->Label);
                int upper8 = (imm >> 16) & 0xff;
                char * IMM = itostr(upper8, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);
            }
            else if (tag == operand_label_offsetpart) {
                int imm = Table_Get(Labels, op->Label);
                int lower16 = (imm & 0xffff) / 2;
                char * IMM = itostr(lower16, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);
            }
            else {
                abortf("");
            }

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case JMP:
        case JA:
        case JNA:
        case JB:
        case JNB:
        case JEQ:
        case JNEQ:
        case JC:
        case JNC:
        case JEV:
        case JNEV:
        case JP:
        case JNP:
        case JZ:
        case JNZ:
        case JR:
        case JNR:
        {
            u64 bsize = 4;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * op = instru->single;
            instru_setra(op->Segment, opcode);
            instru_setimm16(op->Offset, opcode);

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case JMP2:
        case JA2:
        case JNA2:
        case JB2:
        case JNB2:
        case JEQ2:
        case JNEQ2:
        case JC2:
        case JNC2:
        case JEV2:
        case JNEV2:
        case JP2:
        case JNP2:
        case JZ2:
        case JNZ2:
        case JR2:
        case JNR2:
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);

            Operand * op = instru->single;
            TOperand tag = op->Tag;
            if (tag == operand_j_label) {
                u64 address = Table_Get(Labels, op->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = (address & 0xffff) / 2;

                // mov r11, upper8
                instru_mov_ra_imm("r11", upper8);
                // mov r12, lower16
                instru_mov_ra_imm("r12", lower16);

                instru_setra("r11", opcode);
                instru_setrb("r12", opcode);
            }
            else if (tag == operand_j_seg_register) {
                instru_setra(op->Segment, opcode);
                instru_setrb(op->Offset, opcode);
            }
            else {
                abortf("");
            }

            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case CALL:  // call <label>
        {
            Operand * op = instru->single;
            instru_push_ra("cs");
            instru_mov_ra_rb("r11", "ip");
            instru_add_ra_imm("r11", 8);
            instru_push_ra("r11");
            instru_jmp_label(op->Label);
        } break;

        case CALL2:
        {
            abortf("call ra:rb is depreciated!!");
        } break;

        case RET:   // ret
        {
            instru_pop_ra("r12");
            instru_pop_ra("r11");
            instru_jmp_ra_rb("r11", "r12");
        } break;

        case RETN:  // retn <N>
        {
            Operand * op = instru->single;
            int N = strtoi(op->IMM);
            assertf(N >= 0, "Error: retn <N> must have a not negative number of operand! Not %d", N);
            instru_pop_ra("r11");
            instru_pop_ra("r12");
            for (int i = 0; i < N; i++) {
                instru_pop0();
            }
            instru_jmp_ra_rb("r11", "r12");
        } break;

        case POP0:
        case NOP:
        case END:
        {
            u64 bsize = 2;
            char * opcode = strdup(instru_opcode_list[tag]);
            
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);
            
            free(opcode);
        } break;

        case DB:
        {
            u64 bsize = 0;
            const char * byte1 = "\txxxx xxxx";
            const char * byte2 = " xxxx xxxx\n";
            char * opcode = nil;
            
            Operand * op = instru->single;

            int count = op->DCOUNT;
            for (int i = 0; i < count; i++) {
                if (i % 2 == 0) {
                    opcode = strapp(opcode, byte1);
                }
                else {
                    opcode = strapp(opcode, byte2);
                }

                i8 imm = (i8) op->DB[i];
                char * IMM = itostr(imm, -1);
                instru_setimm8(IMM, opcode);
                free(IMM);
            }

            if (count % 2 != 0) {
                opcode = strapp(opcode, " 0000 0000\n");
                count += 1;
            }

            bsize = count;
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);
            
            free(opcode);
        } break;

        case DW:
        {
            u64 bsize = 0;
            const char * word = "\txxxx xxxx xxxx xxxx\n";
            char * opcode = nil;
            
            Operand * op = instru->single;
            int count = op->DCOUNT;
            for (int i = 0; i < count; i++) {
                opcode = strapp(opcode, word);
                i16 imm = (i16) op->DW[i];
                char * IMM = itostr(imm, -1);
                instru_setimm16(IMM, opcode);
                free(IMM);
            }

            bsize = count * 2;
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case SECTION:
        {
            Section = instru->single->imm;
            Location = 0;
        } break;

        case RESB:
        {
            u64 bsize = 0;
            Operand * op = instru->single;
            char * opcode = nil;
            
            int count = strtoi(op->IMM);
            if (count % 2 != 0) {
                count += 1;
            }
            for (int i = 0; i < count; i += 2) {
                opcode = strapp(opcode, "\t0000 0000 0000 0000\n");
            }
            
            bsize = count;
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;

        case RESW:
        {
            u64 bsize = 0;
            Operand * op = instru->single;
            char * opcode = nil;
            
            int count = strtoi(op->IMM);
            for (int i = 0; i < count; i++) {
                opcode = strapp(opcode, "\t0000 0000 0000 0000\n");
            }

            bsize = count * 2;
            u64 address = (Section << 16) + (Location / 2);
            writeln("\t(0x%lx)\n%s\n", address, opcode);
            update_location(bsize);

            free(opcode);
        } break;
    }
    instrulyze(instru->next);
}
void instrufree(Instruction * instru) {
    if (!instru) {
        return;
    }
    TInstru tag = instru->Tag;
    switch (tag) {
        case ADD:
        case ADD2:
        case ADD3:
        case ADD4:
        case ADD5:
        case ADD6:
        case SUB:
        case SUB2:
        case ADC:
        case ADC2:
        case ADC3:
        case CMP:
        case CMP2:
        case CMP3:
        case CMP4:
        case AND:
        case AND2:
        case AND3:
        case AND4:
        case AND5:
        case OR:
        case OR2:
        case OR3:
        case OR4:
        case OR5:
        case XOR:
        case XOR2:
        case XOR3:
        case XOR4:
        case XOR5:
        case MUL:
        case MUL2:
        case MUL3:
        case MUL4:
        case MUL5:
        case DIV:
        case DIV2:
        case DIV3:
        case DIV4:
        case DIV5:
        case MOD:
        case MOD2:
        case MOD3:
        case MOD4:
        case MOD5:
        case MOV:
        case MOV2:
        case MOV3:
        case MOV4:
        case MOV5:
        case MOV6:
        case MOV7:
        case MOV8:
        case XCHG:
        case SHR:
        case SHR2:
        case SHL:
        case SHL2:
        case SAR:
        case SAR2:
        {
            operandfree(instru->left);
            operandfree(instru->right);
            instru->left = nil;
            instru->right = nil;
        } break;

        case FLIP:
        case FLIP2:
        case NEG:
        case NEG2:
        case PUSH:
        case PUSH2:
        case PUSH3:
        case PUSH4:
        case POP:
        case POP2:
        case POP3:
        case RETN:
        case RESB:
        case RESW:
        case JMP:
        case JMP2:
        case JA:
        case JA2:
        case JNA:
        case JNA2:
        case JB:
        case JB2:
        case JNB:
        case JNB2:
        case JEQ:
        case JEQ2:
        case JNEQ:
        case JNEQ2:
        case JC:
        case JC2:
        case JNC:
        case JNC2:
        case JEV:
        case JEV2:
        case JNEV:
        case JNEV2:
        case JP:
        case JP2:
        case JNP:
        case JNP2:
        case JZ:
        case JZ2:
        case JNZ:
        case JNZ2:
        case JR:
        case JR2:
        case JNR:
        case JNR2:
        case CALL:
        case DB:
        case DW:
        {
            operandfree(instru->single);
            instru->single = nil;
        } break;

        default:
        {
        } break;
    }
    instrufree(instru->next);
    free(instru);
}

