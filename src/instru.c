#include "instru.h"
#include "header.h"
#include "str.h"
#include "table.h"
#include "utils.h"

extern TTable * LabelTable;
extern char * Label;
extern int CharIn;
extern TToken Token;
extern String Symbol;
extern u64 Section;
extern u64 Location;
extern Instruction * Instru;
extern const char * const opcode_list[];

Instruction * instruction;

static void instru_push_ra(const char * ra);
static void instru_pop_ra(const char * ra);
static void instru_mov_ra_imm(const char * ra, int imm);
static void instru_setra(char * REG, char * OPCODE);
static void instru_setrb(char * REG, char * OPCODE);
static void instru_setimm4(char * IMM4, char * OPCODE);
static void instru_setimm8(char * IMM8, char * OPCODE);
static void instru_setimm16(char * IMM16, char * OPCODE);

void update_location(u64 byte_size) {
    assertf(Section <= UINT8_MAX, "Section Overflow: 0x%lx", Section);
    Location += byte_size;
    if (Location > UINT16_MAX) {
        Section += 1;
        Location -= UINT16_MAX;
    }
}

void instrulyze(Instruction * instru) {
    if (!instru) {
        return;
    }

    TInstru Tag = instru->Tag;
    switch (Tag) {
        case ADD:
        case AND:
        case OR:
        case XOR:
        case CMP:
        case SUB:
        case ADC:
        case MUL:
        case DIV:
        case MOD:
        case XCHG:
        case MOV6:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            
            instru_setra(instru->left->Register, opcode);
            instru_setrb(instru->right->Register, opcode);
            
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case AND2:
        case OR2:
        case XOR2:
        case CMP2:
        case ADC2:
        case MUL2:
        case DIV2:
        case MOD2:
        case ADD2:
        case ADD3:
        case MOV4:
        case MOV5:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            instru_setra(instru->left->Register, opcode);
            if (instru->right->Tag == operand_byte_label ||
                instru->right->Tag == operand_word_label) 
            {
                u64 address = Table_Get(LabelTable, instru->right->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);

                instru_setrb("r0", opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);

                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->right->Tag == operand_byte_seg_register ||
                    instru->right->Tag  == operand_word_seg_register) 
            {
                instru_setrb(instru->right->Offset, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
        } break;

        case MOV:
        case SUB2:
        case AND3:
        case OR3:
        case XOR3:
        case CMP3:
        case ADC3:
        case MUL3:
        case DIV3:
        case MOD3:
        case ADD4:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            instru_setra(instru->left->Register, opcode);
            if (instru->right->Tag == operand_imm) {
                instru_setimm16(instru->right->IMM, opcode);
            }
            else if (instru->right->Tag == operand_label_sectionpart) {
                u64 address = Table_Get(LabelTable, instru->right->Label);
                int upper8 = (address >> 16) & 0xff;
                char * imm = itostr(upper8, -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }
            else if (instru->right->Tag == operand_label_offsetpart) {
                u64 address = Table_Get(LabelTable, instru->right->Label);
                int lower16 = address & 0xffff;
                char * imm = itostr(lower16, -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case AND4:
        case OR4:
        case XOR4:
        case MUL4:
        case DIV4:
        case MOD4:
        case ADD5:
        case MOV8:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            instru_setrb(instru->right->Register, opcode);
            if (instru->left->Tag == operand_word_label) {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);

                instru_setra("r0", opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);

                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->left->Tag == operand_word_seg_register) {
                instru_setra(instru->left->Offset, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
        } break;

        case CMP4:
        case AND5:
        case OR5:
        case XOR5:
        case MUL5:
        case DIV5:
        case MOD5:
        case ADD6:
        case MOV7:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            if (instru->left->Tag == operand_word_label && 
                instru->right->Tag == operand_label_sectionpart) 
            {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                u64 section = (Table_Get(LabelTable, instru->right->Label) >> 16) & 0xff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);

                instru_setra("ds", opcode);
                instru_setrb("r0", opcode);
                char * imm = itostr(section, -1);
                instru_setimm16(imm, opcode);
                free(imm);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
                
                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->left->Tag == operand_word_label && 
                    instru->right->Tag == operand_label_offsetpart) 
            {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                int offset = Table_Get(LabelTable, instru->right->Label) & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);

                instru_setra("ds", opcode);
                instru_setrb("r0", opcode);
                char * imm = itostr(offset, -1);
                instru_setimm16(imm, opcode);
                free(imm);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);

                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->left->Tag == operand_word_label && 
                    instru->right->Tag == operand_imm) 
            {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);
                
                instru_setra("ds", opcode);
                instru_setrb("r0", opcode);
                instru_setimm16(instru->right->IMM, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
                
                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->left->Tag == operand_word_seg_register && 
                    instru->right->Tag == operand_label_sectionpart) 
            {
                u64 address = Table_Get(LabelTable, instru->right->Label);
                int upper8 = (address >> 16) & 0xff;
                instru_setra(instru->left->Segment, opcode);
                instru_setrb(instru->left->Offset, opcode);
                char * imm = itostr(upper8, -1);
                instru_setimm16(imm, opcode);
                free(imm);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
            else if (instru->left->Tag == operand_word_seg_register && 
                    instru->right->Tag == operand_label_offsetpart) 
            {
                u64 address = Table_Get(LabelTable, instru->right->Label);
                int lower16 = address & 0xffff;
                instru_setra(instru->left->Segment, opcode);
                instru_setrb(instru->left->Offset, opcode);
                char * imm = itostr(lower16, -1);
                instru_setimm16(imm, opcode);
                free(imm);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
            else if (instru->left->Tag == operand_word_seg_register && 
                    instru->right->Tag == operand_imm) 
            {
                instru_setra(instru->left->Segment, opcode);
                instru_setrb(instru->left->Offset, opcode);
                instru_setimm16(instru->right->IMM, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
        } break;

        case MOV2:
        case MOV3:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            instru_setra(instru->left->Register, opcode);
            instru_setrb(instru->right->Offset, opcode);
            instru_setimm16(instru->right->IMM, opcode);
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case SHR:
        case SHL:
        case SAR:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            instru_setra(instru->left->Register, opcode);
            instru_setimm4(instru->right->IMM, opcode);

            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case SHR2:
        case SHL2:
        case SAR2:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            if (instru->left->Tag == operand_word_label) {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);
                
                // shr word [ds:r0], imm4
                // shl word [ds:r0], imm4
                // sar word [ds:r0], imm4
                instru_setra("ds", opcode);
                instru_setrb("r0", opcode);
                instru_setimm4(instru->right->IMM, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);

                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->left->Tag == operand_word_seg_register) {
                instru_setra(instru->left->Segment, opcode);
                instru_setrb(instru->left->Offset, opcode);
                instru_setimm4(instru->right->IMM, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
        } break;

        case FLIP:
        case NEG:
        case PUSH:
        case POP:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            instru_setra(instru->left->Segment, opcode);
            instru_setrb(instru->left->Offset, opcode);
            instru_setimm4(instru->right->IMM, opcode);

            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case PUSH2:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;
            if (instru->operand->Tag == operand_imm) {
                instru_setimm16(instru->operand->IMM, opcode);
            }
            else if (instru->operand->Tag == operand_label_sectionpart) {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                char * imm = itostr(upper8, -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }
            else if (instru->operand->Tag == operand_label_offsetpart) {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int lower16 = address & 0xffff;
                char * imm = itostr(lower16, -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case FLIP2:
        case NEG2:
        case PUSH3:
        case PUSH4:
        case POP2:
        case POP3:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            if (instru->operand->Tag == operand_byte_label ||
                instru->operand->Tag == operand_word_label) 
            {
                u64 address = Table_Get(LabelTable, instru->left->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                // push ds
                instru_push_ra("ds");
                // push r0
                instru_push_ra("r0");
                // mov ds, upper8
                instru_mov_ra_imm("ds", upper8);
                // mov r0, lower16
                instru_mov_ra_imm("r0", lower16);

                // flip word [label] => flip word [ds:r0]
                // neg word [label]  => neg word [ds:r0]
                instru_setra("ds", opcode);
                instru_setrb("r0", opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);

                // pop r0
                instru_pop_ra("r0");
                // pop ds
                instru_pop_ra("ds");
            }
            else if (instru->operand->Tag == operand_byte_seg_register ||
                    instru->operand->Tag == operand_word_seg_register) 
            {
                instru_setra(instru->operand->Segment, opcode);
                instru_setrb(instru->operand->Offset, opcode);
                writeln("\t"
                "(0x%lx)"
                "\n"
                "%s"
                "\n", 
                (Section << 16) + (Location / 2), 
                opcode);
                update_location(byte_size);
                free(opcode);
            }
        } break;

        case RETN:
        {
            char * opcode = strdup(opcode_list[Tag]);
            
            int n = strtoi(instru->operand->IMM);
            for (int i = 0; i < n; i += 2) {
                opcode = strapp(opcode, opcode_list[POP0]);
            }

            u64 byte_size = 4 + n;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);

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
        case CALL:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 4;

            if (instru->operand->Tag == operand_j_label) {
                u64 address = Table_Get(LabelTable, instru->operand->Label);
                int upper8 = (address >> 16) & 0xff;
                int lower16 = address & 0xffff;
                
                // mov cs, upper8
                instru_mov_ra_imm("cs", upper8);

                instru_setra("cs", opcode);
                char * imm = itostr(lower16, -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }   
            else if (instru->operand->Tag == operand_j_seg_imm16) {
                instru_setra(instru->operand->Segment, opcode);
                instru_setimm16(instru->operand->Offset, opcode);
            }
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
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
        case CALL2:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            
            instru_setra(instru->operand->Segment, opcode);
            instru_setrb(instru->operand->Offset, opcode);

            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case SECTION:
        {
            Section = instru->operand->section;
            Location = 0;
        } break;

        case POP0:
        case RET:
        case NOP:
        case END:
        {
            char * opcode = strdup(opcode_list[Tag]);
            u64 byte_size = 2;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case DB:
        {
            const char * byte1 = "\txxxx xxxx";
            const char * byte2 = " xxxx xxxx\n";
            char * opcode = nil;
            int count = instru->operand->DCOUNT;
            for (int i = 0; i < count; i++) {
                if (i % 2 == 0) {
                    opcode = strapp(opcode, byte1);
                }
                else {
                    opcode = strapp(opcode, byte2);
                }
                char * imm = itostr(instru->operand->DB[i], -1);
                instru_setimm8(imm, opcode);
                free(imm);
            }
            if (count % 2 != 0) {
                opcode = strapp(opcode, " 0000 0000\n");
                count += 1;
            }
            u64 byte_size = count;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "\t"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;
        case DW:
        {
            const char * word = "\txxxx xxxx xxxx xxxx\n";
            char * opcode = nil;
            int count = instru->operand->DCOUNT;
            for (int i = 0; i < count; i++) {
                opcode = strapp(opcode, word);
                char * imm = itostr(instru->operand->DW[i], -1);
                instru_setimm16(imm, opcode);
                free(imm);
            }
            u64 byte_size = count * 2;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "\t"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        case RESB:
        {
            int count = strtoi(instru->operand->IMM);
            if (count % 2 != 0) {
                count += 1;
            }
            char * opcode = nil;
            for (int i = 0; i < count; i += 2) {
                opcode = strapp(opcode, "\t0000 0000 0000 0000\n");
            }
            u64 byte_size = count;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;
        case RESW:
        {
            int count = strtoi(instru->operand->IMM);
            char * opcode = nil;
            for (int i = 0; i < count; i++) {
                opcode = strapp(opcode, "\t0000 0000 0000 0000\n");
            }
            u64 byte_size = count * 2;
            writeln("\t"
            "(0x%lx)"
            "\n"
            "%s"
            "\n", 
            (Section << 16) + (Location / 2), 
            opcode);
            update_location(byte_size);
            free(opcode);
        } break;

        default:
        {
            abortf("Error @instrulyze: "
            "invalid Token: %d, invalid Symbol: %s", Token, Symbol);
        } break;
    }

    if (Tag == END) {
        instrufree(instru->next);
        instru->next = nil;
    }
    else {
        instrulyze(instru->next);
    }
}

void instrufree(Instruction * instru) {
    if (!instru) {
        return;
    }

    switch (instru->Tag) {
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
            free_operand(instru->left);
            free_operand(instru->right);
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
        case CALL2:
        case DB:
        case DW:
        {
            free_operand(instru->operand);
        } break;

        case POP0:
        case RET:
        case NOP:
        case END:
        {
        } break;        

        default:
        {
        } break;
    }

    instrufree(instru->next);
    free(instru);
}

void free_operand(Operand * operand) {
    if (!operand) {
        return;
    }

    switch (operand->Tag) {
        case operand_register:
        {
            free(operand->Register);
        } break;

        case operand_imm:
        case operand_resb:
        case operand_resw:
        {
            free(operand->IMM);
        } break;

        case operand_byte_label:
        case operand_word_label:
        case operand_j_label:
        case operand_label_sectionpart:
        case operand_label_offsetpart:
        {
            free(operand->Label);
        } break;

        case operand_byte_seg_imm16:
        case operand_word_seg_imm16:
        case operand_byte_seg_register:
        case operand_word_seg_register:
        case operand_j_seg_imm16:
        case operand_j_seg_register:
        {
            free(operand->Segment);
            free(operand->Offset);
        } break;

        case operand_db:
        {
            free(operand->DB);
        } break;
        case operand_dw:
        {
            free(operand->DW);
        } break;

        case operand_section:
        {
        } break;

        default:
        {
            abortf("Error @func {free_operand}: Unknown tag: %d", operand->Tag);
        } break;
    }
    free(operand);
}

static void instru_push_ra(const char * ra) {
    Instruction * instru = malloc(sizeof(Instruction));
    instru->Tag = PUSH;
    instru->next = nil;
    instru->operand = malloc(sizeof(Operand));
    instru->operand->Tag = operand_register;
    instru->operand->Register = strdup(ra);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_mov_ra_imm(const char * ra, int imm) {
    Instruction * instru = malloc(sizeof(Instruction));
    instru->Tag = MOV;
    instru->next = nil;
    instru->left = malloc(sizeof(Operand));
    instru->right = malloc(sizeof(Operand));
    instru->left->Tag = operand_register;
    instru->left->Register = strdup(ra);
    instru->right->Tag = operand_imm;
    instru->right->IMM = itostr(imm, -1);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_pop_ra(const char * ra) {
    Instruction * instru = malloc(sizeof(Instruction));
    instru->Tag = POP;
    instru->next = nil;
    instru->operand = malloc(sizeof(Operand));
    instru->operand->Tag = operand_register;
    instru->operand->Register = strdup(ra);
    instrulyze(instru);
    instrufree(instru);
}
static void instru_setra(char * REG, char * OPCODE) {
    assertf(REG != nil, "Error @func {%s}, REG is (nil)", __FUNCTION__);
    assertf(OPCODE != nil, "Error @func {%s}: OPCODE is (nil)", __FUNCTION__);

    int index = strloc(OPCODE, "a");
    assertf(index != -1, "");

    char * code = strdup(register_code(REG));
    assertf(code != nil, "");
    for (int i = 0; i < 4; i++) {
        OPCODE[index + i] = code[i];
    }
    free(code);
}
static void instru_setrb(char * REG, char * OPCODE) {
    assertf(REG != nil, "Error @func {%s}, REG is (nil)", __FUNCTION__);
    assertf(OPCODE != nil, "Error @func {%s}: OPCODE is (nil)", __FUNCTION__);

    int index = strloc(OPCODE, "b");
    assertf(index != -1, "");

    char * code = strdup(register_code(REG));
    assertf(code != nil, "");
    for (int i = 0; i < 4; i++) {
        OPCODE[index + i] = code[i];
    }
    free(code);
}
static void instru_setimm4(char * IMM4, char * OPCODE) {
    assertf(IMM4 != nil, "Error @func {%s}, IMM4 is (nil)", __FUNCTION__);
    assertf(OPCODE != nil, "Error @func {%s}: OPCODE is (nil)", __FUNCTION__);

    int index = strloc(OPCODE, "x");
    assertf(index != -1, "");

    int imm = strtoi(IMM4);
    char * binstr = itostr(imm, 4);
    assertf(binstr != nil, "");
    for (int i = 0; i < 4; i++) {
        OPCODE[index + i] = binstr[i];
    }
    free(binstr);
}
static void instru_setimm8(char * IMM8, char * OPCODE) {
    assertf(IMM8 != nil, "Error @func {%s}, IMM8 is (nil)", __FUNCTION__);
    assertf(OPCODE != nil, "Error @func {%s}: OPCODE is (nil)", __FUNCTION__);

    int index = strloc(OPCODE, "x");
    assertf(index != -1, "");

    int imm = strtoi(IMM8);
    char * binstr = itostr(imm, 8);
    assertf(binstr != nil, "");
    for (int i = 0; i < 9; i++) {
        OPCODE[index + i] = binstr[i];
    }
    free(binstr);
}
static void instru_setimm16(char * IMM16, char * OPCODE) {
    assertf(IMM16 != nil, "Error @func {%s}, IMM16 is (nil)", __FUNCTION__);
    assertf(OPCODE != nil, "Error @func {%s}: OPCODE is (nil)", __FUNCTION__);

    int index = strloc(OPCODE, "x");
    assertf(index != -1, "");

    int imm = strtoi(IMM16);
    char * binstr = itostr(imm, 16);
    assertf(binstr != nil, "");
    for (int i = 0; i < 19; i++) {
        OPCODE[index + i] = binstr[i];
    }
    free(binstr);
}



char * operand_string(Operand * operand) {
    assert(operand != nil);
    switch (operand->Tag) {
        case operand_register:
        {
            return "register";
        } break;

        case operand_imm:
        {
            return "imm";
        } break;

        case operand_label_sectionpart:
        {
            return "label section part";
        } break;
        
        case operand_label_offsetpart:
        {
            return "label offset part";
        } break;
        
        case operand_byte_label:
        {
            return "byte [label]";
        } break;

        case operand_byte_seg_imm16:
        {
            return "byte [seg:imm16]";
        } break;

        case operand_byte_seg_register:
        {
            return "byte [seg:reg]";
        } break;

        case operand_word_label:
        {
            return "word [label]";
        } break;

        case operand_word_seg_imm16:
        {
            return "word [seg:imm16]";
        } break;

        case operand_word_seg_register:
        {
            return "word [seg:reg]";
        } break;

        case operand_db:
        {
            return "db";
        } break;

        case operand_dw:
        {
            return "dw";
        } break;

        case operand_resb:
        {
            return "resb";
        } break;

        case operand_resw:
        {
            return "resw";
        } break;
        
        case operand_j_label:
        {
            return "label";
        } break;

        case operand_j_seg_imm16:
        {
            return "seg:imm16";
        } break;

        case operand_j_seg_register:
        {
            return "seg:reg";
        } break;

        default:
        {
            return "";
        } break;
    }
    return "";
}

void parse_db(Operand * operand) {
    if (Token==TokenInteger) {
        operand->DCOUNT += 1;
        operand->DB = realloc(operand->DB, sizeof(char) * operand->DCOUNT);
        int index = operand->DCOUNT - 1;
        operand->DB[index] = (char) strtoi(Symbol);
    }
    else if (Token==TokenQuote) {
        assert(CharIn != '\'');
        while (CharIn != '\'') {
            operand->DCOUNT += 1;
            operand->DB = realloc(operand->DB, sizeof(char) * operand->DCOUNT);
            int index = operand->DCOUNT - 1;
            operand->DB[index] = (char) CharIn;
            readin();
        }
        scan();
        assert(Token==TokenQuote);
    }
    else {
        abortf("Error @parse_define_byte: "
        "invalid Token: %d, invalid Symbol: %s",
        Token, Symbol);
    }
    scan();
}
void parse_define_byte(Instruction * instru) {
    instru->Tag = DB;
    Operand * operand = malloc(sizeof(Operand));
    assert(operand != nil);
    operand->Tag = operand_db;
    operand->DB = nil;
    operand->DCOUNT = 0;
    parse_db(operand);
    while (Token==TokenComma) {
        scan();
        parse_db(operand);
    }
    instru->operand = operand;
}
void parse_dw(Operand * operand) {
    if (Token==TokenInteger) {
        operand->DCOUNT += 1;
        operand->DW = realloc(operand->DW, sizeof(int) * operand->DCOUNT);
        int index = operand->DCOUNT - 1;
        operand->DW[index] = (int) strtoi(Symbol);
    }
    else if (Token==TokenQuote) {
        assert(CharIn != '\'');
        while (CharIn != '\'') {
            operand->DCOUNT += 1;
            operand->DW = realloc(operand->DW, sizeof(int) * operand->DCOUNT);
            int index = operand->DCOUNT - 1;
            operand->DW[index] = (int) CharIn;
            readin();
        }
        scan();
        assert(Token==TokenQuote);
    }
    else {
        abortf("Error @parse_define_byte: "
        "invalid Token: %d, invalid Symbol: %s",
        Token, Symbol);
    }
    scan();
}
void parse_define_word(Instruction * instru) {
    instru->Tag = DW;
    Operand * operand = malloc(sizeof(Operand));
    assert(operand != nil);
    operand->Tag = operand_dw;
    operand->DW = nil;
    operand->DCOUNT = 0;
    parse_dw(operand);
    while (Token==TokenComma) {
        scan();
        parse_dw(operand);
    }
    instru->operand = operand;
}
void parse_section(Instruction * instru) {
    assert(Token==TokenLabel);
    char * section = strdup(Symbol);
    scan();
    assert(Token==TokenInteger);
    Section = strtoi(Symbol);
    Location = 0;
    Table_Set(LabelTable, section, Section);
    instru->Tag = SECTION;
    instru->operand = malloc(sizeof(Operand));
    instru->operand->Tag = operand_section;
    instru->operand->section = Section;
    free(section);
    scan();
}

Operand * parse_operand(void) {
    Operand * operand = malloc(sizeof(Operand));
    assert(operand != nil);

    if (Token==TokenByte) {
        scan();
        parse_byte_reference(operand);
    }
    else if (Token==TokenWord) {
        scan();
        parse_word_reference(operand);
    }
    else if (Token==TokenLBracket) {
        parse_word_reference(operand);
    }
    else if (Token==TokenInteger) {
        operand->Tag = operand_imm;
        operand->IMM = strdup(Symbol);
    }
    else if (Token==TokenWRT) {
        scan();
        assert(Token==TokenLabel);
        operand->Tag = operand_label_sectionpart;
        if (Symbol[0] == '.') {
            assertf(Label != nil, "");
            Symbol = strpre(Symbol, Label);
        }
        operand->Label = strdup(Symbol);
    }
    else if (Token==TokenLabel) {
        operand->Tag = operand_label_offsetpart;
        if (Symbol[0] == '.') {
            assert(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        operand->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        operand->Tag = operand_register;
        operand->Register = strdup(Symbol);
    }
    else {
        abortf("Error @parse_operand: "
        "invalid Token: %d, invalid Symbol: %s!", 
        Token, Symbol);
    }   

    scan();
    return operand;
}
void parse_two_operands(Instruction * instru) {
    instru->left = parse_operand();
    assert(Token==TokenComma);
    scan();
    instru->right = parse_operand();
}
void parse_only_operand(Instruction * instru) {
    instru->operand = parse_operand();
}
void parse_jump_family(Instruction * instru) {
    Operand * operand = malloc(sizeof(Operand));
    assert(operand != nil);

    if (Token==TokenLabel) {
        operand->Tag = operand_j_label;
        if (Symbol[0] == '.') {
            assert(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        operand->Label = strdup(Symbol);
        scan();
    }
    else if (Token==TokenInteger) {
        operand->Tag = operand_j_seg_imm16;
        operand->Segment = strdup("cs");
        operand->Offset = strdup(Symbol);
        scan();
    }
    else if (is_token_register(Token)) {
        char * Register = strdup(Symbol);
        scan();
        if (Token==TokenColon) {
            scan();
            if (is_token_register(Token)) {
                operand->Tag = operand_j_seg_register;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else if (Token==TokenInteger) {
                operand->Tag = operand_j_seg_imm16;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else {
                abortf("Error @parse_jump_family: "
                "invalid Token: %d, invalid Symbol: %s!", 
                Token, Symbol);
            }
            scan();
        }
        else {
            operand->Tag = operand_j_seg_register;
            operand->Segment = strdup("cs");
            operand->Offset  = Register;
        }
    }
    else {
        abortf("Error @parse_jump_family: "
        "invalid Token: %d, Symbol: %s", 
        Token, Symbol);
    }

    instru->operand = operand;
}

void parse_byte_reference(Operand * operand) {
    assert(Token==TokenLBracket);
    scan();
    if (Token==TokenInteger) {
        operand->Tag = operand_byte_seg_imm16;
        operand->Segment = strdup("ds");
        operand->Offset = strdup(Symbol);
    }
    else if (Token==TokenLabel) {
        operand->Tag = operand_byte_label;
        if (Symbol[0] == '.') {
            assert(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        operand->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        char * Register = strdup(Symbol);
        scan();
        if (Token==TokenColon) {
            scan(); 
            if (Token==TokenInteger) {
                operand->Tag = operand_byte_seg_imm16;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else if (is_token_register(Token)) {
                operand->Tag = operand_byte_seg_register;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else {
                abortf("Error @parse_byte_reference: "
                "invalid Token: %d, invalid Symbol: %s",
                Token, Symbol);
            }
            scan();
        }
        else {
            operand->Tag = operand_byte_seg_register;
            operand->Segment = strdup("ds");
            operand->Offset = Register;
        }
    }
    scan();
    assert(Token==TokenRBracket);
}
void parse_word_reference(Operand * operand) {
    assert(Token==TokenLBracket);
    scan();
    if (Token==TokenInteger) {
        operand->Tag = operand_word_seg_imm16;
        operand->Segment = strdup("ds");
        operand->Offset = strdup(Symbol);
    }
    else if (Token==TokenLabel) {
        operand->Tag = operand_word_label;
        if (Symbol[0] == '.') {
            assert(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        operand->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        char * Register = strdup(Symbol);
        scan();
        if (Token==TokenColon) {
            scan();
            if (Token==TokenInteger) {
                operand->Tag = operand_word_seg_imm16;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else if (is_token_register(Token)) {
                operand->Tag = operand_word_seg_register;
                operand->Segment = Register;
                operand->Offset = strdup(Symbol);
            }
            else {
                abortf("Error @parse_word_reference: "
                "invalid Token: %d, invalid Symbol: %s",
                Token, Symbol);
            }
            scan();
        }
        else {
            operand->Tag = operand_word_seg_register;
            operand->Segment = strdup("ds");
            operand->Offset = Register;
        }
    }
    scan();
    assert(Token==TokenRBracket);
}

Instruction * assemble(void) {
    if (Token==TokenEOF) {
        return nil;
    }

    Instruction * instru = malloc(sizeof(Instruction));
    instru->next = nil;
begin:
    if (Token==TokenLabel) {
        // Label
        char * label = nil;
        if (Symbol[0] == '.') {
            // Sublabel
            assert(Label != nil);
            label = strdup(Label);
            label = strapp(label, Symbol);
        }
        else {
            if (Label) {
                free(Label);
                Label = nil;
            }
            Label = strdup(Symbol);
            label = strdup(Symbol);
        }
        scan();
        assert(Token==TokenColon);
        Table_Set(LabelTable, label, (Section << 16) + Location);
        free(label);
        scan();
        goto begin;
    }
    else if (Token==TokenADD || Token==TokenSHR || Token==TokenSHL || 
            Token==TokenAND  || Token==TokenOR  || Token==TokenXOR ||
            Token==TokenCMP  || Token==TokenSAR || Token==TokenSUB ||
            Token==TokenMUL  || Token==TokenDIV || Token==TokenMOD ||
            Token==TokenMOV  || Token==TokenXCHG) 
    {
        TToken token = Token;
        scan();
        parse_two_operands(instru);
        Operand * left = instru->left;
        Operand * right = instru->right;
        switch (token) {
            case TokenADD:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = ADD;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_byte_seg_register) {
                    instru->Tag = ADD2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_byte_label) {
                    instru->Tag = ADD2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = ADD3;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = ADD3;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = ADD4;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = ADD4;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = ADD4;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = ADD5;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = ADD5;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = ADD6;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = ADD6;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = ADD6;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = ADD6;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = ADD6;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = ADD6;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "add %s, %s", left, right);
                }
            } break;

            case TokenSHR:
            {
                if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = SHR;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = SHR2;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = SHR2;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "shr %s, %s", left, right);
                }
            } break;

            case TokenSHL:
            {
                if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = SHL;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = SHL2;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = SHL2;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "shl %s, %s", left, right);
                }
            } break;

            case TokenSAR:
            {
                if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = SAR;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = SAR2;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = SAR2;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "sar %s, %s", left, right);
                }
            } break;

            case TokenAND:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = AND;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = AND2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = AND2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = AND3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = AND3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = AND3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = AND4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = AND4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = AND5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = AND5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = AND5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = AND5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = AND5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = AND5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "and %s, %s", left, right);
                }
            } break;

            case TokenOR:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = OR;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = OR2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = OR2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = OR3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = OR3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = OR3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = OR4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = OR4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = OR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = OR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = OR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = OR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = OR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = OR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "or %s, %s", left, right);
                }
            } break;

            case TokenXOR:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = XOR;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) 
                {
                    instru->Tag = XOR2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = XOR2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = XOR3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = XOR3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = XOR3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = XOR4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = XOR4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = XOR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = XOR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = XOR5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = XOR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = XOR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = XOR5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "xor %s, %s", left, right);
                }
            } break;

            case TokenCMP:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = CMP;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = CMP2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = CMP2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = CMP3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = CMP3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = CMP3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = CMP4;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = CMP4;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = CMP4;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = CMP4;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = CMP4;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = CMP4;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "cmp %s, %s", left, right);
                }
            } break;

            case TokenSUB:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = SUB;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = SUB2;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = SUB2;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = SUB2;
                    update_location(4);
                }
                else {
                    abortf("Error @assemble: "
                    "sub %s, %s", left, right);
                }
            } break;

            case TokenMUL:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = MUL;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = MUL2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = MUL2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = MUL3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MUL3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MUL3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = MUL4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = MUL4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = MUL5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MUL5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MUL5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = MUL5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MUL5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MUL5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "mul %s, %s", left, right);
                }
            } break;

            case TokenDIV:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = DIV;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = DIV2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = DIV2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = DIV3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = DIV3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = DIV3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = DIV4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = DIV4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = DIV5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = DIV5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = DIV5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = DIV5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = DIV5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = DIV5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "div %s, %s", left, right);
                }
            } break;

            case TokenMOD:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = MOD;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = MOD2;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = MOD2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_imm) {
                    instru->Tag = MOD3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MOD3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MOD3;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = MOD4;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = MOD4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = MOD5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MOD5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MOD5;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = MOD5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MOD5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MOD5;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "mod %s, %s", left, right);
                }
            } break;

            case TokenMOV:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = MOV;
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_byte_seg_imm16) {
                    instru->Tag = MOV2;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_imm16) {
                    instru->Tag = MOV3;
                    update_location(4);
                }
                else if (left->Tag == operand_register && right->Tag == operand_byte_seg_register) {
                    instru->Tag = MOV4;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_byte_label) {
                    instru->Tag = MOV4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_seg_register) {
                    instru->Tag = MOV5;
                    assert(strcmp(right->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_word_label) {
                    instru->Tag = MOV5;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = MOV6;
                    update_location(2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_imm) {
                    instru->Tag = MOV7;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MOV7;
                    update_location(4);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MOV7;
                    update_location(4);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_imm) {
                    instru->Tag = MOV7;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_sectionpart) {
                    instru->Tag = MOV7;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_label_offsetpart) {
                    instru->Tag = MOV7;
                    update_location(2 + 2 + 4 + 4 + (4) + 2 + 2);
                }
                else if (left->Tag == operand_word_seg_register && right->Tag == operand_register) {
                    instru->Tag = MOV8;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2);
                }
                else if (left->Tag == operand_word_label && right->Tag == operand_register) {
                    instru->Tag = MOV8;
                    assert(strcmp(left->Segment, "ds") == 0);
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "mov %s, %s", left, right);
                }
            } break;

            case TokenXCHG:
            {
                if (left->Tag == operand_register && right->Tag == operand_register) {
                    instru->Tag = XCHG;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "xchg %s, %s", left, right);
                }
            } break;
            
            default:
            {
                abortf("Error @assemble!");
            } break;
        }
    }
    else if (Token==TokenFLIP|| 
            Token==TokenNEG  || 
            Token==TokenRETN || 
            Token==TokenPUSH || Token==TokenPOP ||
            Token==TokenINC  || Token==TokenDEC ||
            Token==TokenRESB || Token==TokenRESW) 
    {
        TToken token = Token;
        scan();
        parse_only_operand(instru);
        Operand * operand = instru->operand;
        switch (token) {
            case TokenFLIP:
            {
                if (operand->Tag == operand_register) {
                    instru->Tag = FLIP;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_seg_register) {
                    instru->Tag = FLIP2;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_label) {
                    instru->Tag = FLIP2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "flip %s", operand_string(operand));
                }
            } break;

            case TokenNEG:
            {
                if (operand->Tag == operand_register) {
                    instru->Tag = NEG;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_seg_register) {
                    instru->Tag = NEG2;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_label) {
                    instru->Tag = NEG2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "neg %s", operand_string(operand));
                }
            } break;

            case TokenRETN:
            {
                if (operand->Tag == operand_imm) {
                    instru->Tag = RETN;
                    int imm = strtoi(operand->IMM);
                    assert(imm % 2 == 0);
                    update_location(4 + imm);
                }
                else {
                    abortf("Error @assemble: "
                    "retn %s", operand_string(operand));
                }
            } break;

            case TokenPUSH:
            {
                if (operand->Tag == operand_register) {
                    instru->Tag = PUSH;
                    update_location(2);
                }
                else if (operand->Tag == operand_imm) {
                    instru->Tag = PUSH2;
                    update_location(4);
                }
                else if (operand->Tag == operand_label_sectionpart) {
                    instru->Tag = PUSH2;
                    update_location(4);
                }
                else if (operand->Tag == operand_label_offsetpart) {
                    instru->Tag = PUSH2;
                    update_location(4);
                }
                else if (operand->Tag == operand_byte_seg_register) {
                    instru->Tag = PUSH3;
                    update_location(2);
                }
                else if (operand->Tag == operand_byte_label) {
                    instru->Tag = PUSH3;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (operand->Tag == operand_word_seg_register) {
                    instru->Tag = PUSH4;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_label) {
                    instru->Tag = PUSH4;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "push %s", operand_string(operand));
                }
            } break;

            case TokenPOP:
            {
                if (operand->Tag == operand_register) {
                    instru->Tag = POP;
                    update_location(2);
                }
                else if (operand->Tag == operand_byte_seg_register) {
                    instru->Tag = POP2;
                    update_location(2);
                }
                else if (operand->Tag == operand_byte_label) {
                    instru->Tag = POP2;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else if (operand->Tag == operand_word_seg_register) {
                    instru->Tag = POP3;
                    update_location(2);
                }
                else if (operand->Tag == operand_word_label) {
                    instru->Tag = POP3;
                    update_location(2 + 2 + 4 + 4 + (2) + 2 + 2);
                }
                else {
                    abortf("Error @assemble: "
                    "pop %s", operand_string(operand));
                }
            } break;

            case TokenINC:
            {
                if (operand->Tag == operand_register) {
                    Operand * left = malloc(sizeof(Operand));
                    Operand * right = malloc(sizeof(Operand));
                    assert(left != nil);
                    assert(right != nil);
                    
                    left->Tag = operand_register;
                    left->Register = strdup(operand->Register);
                    right->Tag = operand_imm;
                    right->IMM= strdup("1");
                    free_operand(instru->operand);
                    
                    instru->Tag = ADD4;
                    instru->left = left;
                    instru->right = right;

                    update_location(4);
                }
                else {
                    abortf("Error @assemble: "
                    "inc %s", operand_string(operand));
                }
            } break;

            case TokenDEC:
            {
                if (operand->Tag == operand_register) {
                    Operand * operand = instru->operand;
                    Operand * left = malloc(sizeof(Operand));
                    Operand * right = malloc(sizeof(Operand));
                    assert(left != nil);
                    assert(right != nil);
                    
                    left->Tag = operand_register;
                    left->Register = strdup(operand->Register);
                    right->Tag = operand_imm;
                    right->IMM= strdup("1");
                    free_operand(instru->operand);
                    
                    instru->Tag = SUB2;
                    instru->left = left;
                    instru->right = right;
                    
                    update_location(4);
                }
                else {
                    abortf("Error @assemble: "
                    "dec %s", operand_string(operand));
                }
            } break;

            case TokenRESB:
            {
                if (operand->Tag == operand_imm) {
                    instru->Tag = RESB;
                    int imm = strtoi(operand->IMM);
                    if (imm % 2 != 0) {
                        imm += 1;
                    }
                    update_location(imm);
                }
                else {
                    abortf("Error @assemble: "
                    "resb %s", operand_string(operand));
                }
            } break;
            case TokenRESW:
            {
                if (operand->Tag == operand_imm) {
                    instru->Tag = RESW;
                    update_location(2 * strtoi(operand->IMM));
                }
                else {
                    abortf("Error @assemble: "
                    "resw %s", operand_string(operand));
                }
            } break;

            default:
            {
                abortf("Error @assemble!");
            } break;
        }
    }
    else if (Token==TokenJMP ||
            Token==TokenCALL ||
            Token==TokenJA || Token==TokenJNA || 
            Token==TokenJB || Token==TokenJNB ||
            Token==TokenJC || Token==TokenJNC ||
            Token==TokenJP || Token==TokenJNP ||
            Token==TokenJZ || Token==TokenJNZ ||
            Token==TokenJR || Token==TokenJNR ||
            Token==TokenJEV || Token==TokenJNEV||
            Token==TokenJEQ || Token==TokenJNEQ) 
    {
        TToken token = Token;
        scan();
        parse_jump_family(instru);
        Operand * operand = instru->operand;
        switch (token) {
            case TokenJMP:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JMP;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JMP;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JMP2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jmp %s", operand_string(operand));
                }
            } break;

            case TokenCALL:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = CALL;
                    update_location(6);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = CALL;
                    update_location(4 + (6));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = CALL2;
                    update_location(4);
                }
                else {
                    abortf("Error @assemble: "
                    "call %s", operand_string(operand));
                }
            } break;

            case TokenJA:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JA;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JA;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JA2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "ja %s", operand_string(operand));
                }
            } break;

            case TokenJNA:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNA;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNA;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNA2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jna %s", operand_string(operand));
                }
            } break;

            case TokenJB:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JB;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JB;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JB2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jb %s", operand_string(operand));
                }
            } break;

            case TokenJNB:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNB;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNB;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNB2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnb %s", operand_string(operand));
                }
            } break;

            case TokenJEQ:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JEQ;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JEQ;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JEQ2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jeq %s", operand_string(operand));
                }
            } break;

            case TokenJNEQ:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNEQ;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNEQ;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNEQ2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jneq %s", operand_string(operand));
                }
            } break;
            
            case TokenJC:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JC;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JC;
                    update_location(4 + 4);
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JC2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jc %s", operand_string(operand));
                }
            } break;

            case TokenJNC:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNC;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNC;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNC2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnc %s", operand_string(operand));
                }
            } break;

            case TokenJEV:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JEV;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JEV;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JEV2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jev %s", operand_string(operand));
                }
            } break;

            case TokenJNEV:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNEV;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNEV;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNEV2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnev %s", operand_string(operand));
                }
            } break;

            case TokenJP:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JP;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JP;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JP2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jp %s", operand_string(operand));
                }
            } break;

            case TokenJNP:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNP;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNP;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNP2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnp %s", operand_string(operand));
                }
            } break;

            case TokenJZ:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JZ;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JZ;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JZ2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jz %s", operand_string(operand));
                }
            } break;

            case TokenJNZ:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNZ;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNZ;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNZ2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnz %s", operand_string(operand));
                }
            } break;

            case TokenJR:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JR;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JR;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JR2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jr %s", operand_string(operand));
                }
            } break;

            case TokenJNR:
            {
                if (operand->Tag == operand_j_seg_imm16) {
                    instru->Tag = JNR;
                    update_location(4);
                }
                else if (operand->Tag == operand_j_label) {
                    instru->Tag = JNR;
                    update_location(4 + (4));
                }
                else if (operand->Tag == operand_j_seg_register) {
                    instru->Tag = JNR2;
                    update_location(2);
                }
                else {
                    abortf("Error @assemble: "
                    "jnr %s", operand_string(operand));
                }
            } break;

            default:
            {
                abortf("Error @assemble!");
            } break;
        }
    }
    else if (Token==TokenRET || Token==TokenPOP0 || Token==TokenNOP || Token==TokenEND) 
    {
        if (Token==TokenRET) {
            instru->Tag = RET;
            update_location(4);
        }
        else if (Token==TokenPOP0) {
            instru->Tag = POP0;
            update_location(2);
        }
        else if (Token==TokenNOP) {
            instru->Tag = NOP;
            update_location(2);
        }
        else if (Token==TokenEND) {
            instru->Tag = END;
            update_location(2);
        }
        scan();
    }
    else if (Token==TokenDB) {
        scan();
        instru->Tag = DB;
        parse_define_byte(instru);
        int count = instru->operand->DCOUNT;
        if (count % 2 != 0) {
            count += 1;
        }
        update_location(count);
    }
    else if (Token==TokenDW) {
        scan();
        instru->Tag = DW;
        parse_define_word(instru);
        update_location(instru->operand->DCOUNT * 2);
    }
    else if (Token==TokenSECTION) {
        scan();
        parse_section(instru);
    }
    else if (Token==TokenPercentage) {
        scan();
    }
    else {
        abortf("Invalid Token: %d, Symbol: %s", Token, Symbol);
    }
    return instru;
}

void process(void) {
    readin();
    scan();
    Instru = assemble();
    instruction = Instru;
    while (Token != TokenEOF) {
        instruction->next = assemble();
        instruction = instruction->next;
    }
}


