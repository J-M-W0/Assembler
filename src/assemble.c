#include "assemble.h"
#include "rasm.h"
#include "str.h"
#include "utils.h"
#include "operand.h"
#include "instru.h"
#include "table.h"

extern Instruction * Instru;
extern TToken Token;
extern String Symbol;
extern char * Label;
extern TTable * Labels;
extern u64 Section;
extern u64 Location;


void Process(void) {
    Instru = assemble();
    Instruction * instruction = Instru;
    while (Token!=TokenEOF) {
        instruction->next = assemble();
        instruction = instruction->next;
    }
}

Instruction * assemble(void) {
begin:
    if (Token==TokenEOF) {
        return nil;
    }

    Instruction * instru = malloc(sizeof(Instruction));
    assertf(instru != nil);
    instru->next = nil;

    if (Token==TokenIdent) {
        if (Symbol[0]=='.') {
            assertf(Label != nil);
            assertf(Symbol[1] != '\0');
            Symbol = strpre(Symbol, Label);
        }
        else {
            Free(Label);
            Label = strdup(Symbol);
        }
        Table_Set(Labels, Symbol, (Section << 16) + Location);
        scan();
        assertf(Token==TokenColon);
        scan();
        goto begin;
    }
    else if (Token==TokenADD || 
             Token==TokenSHR || 
             Token==TokenSHL || 
             Token==TokenAND || 
             Token==TokenOR  || 
             Token==TokenXOR ||
             Token==TokenCMP || 
             Token==TokenSAR || 
             Token==TokenSUB ||
             Token==TokenMUL || 
             Token==TokenDIV || 
             Token==TokenMOD ||
             Token==TokenMOV || 
             Token==TokenXCHG) 
    {
        TToken token = Token;
        scan();
        
        analyze_2_operands(instru);
        Operand * left = instru->left;
        Operand * right = instru->right;
        assertf(left != nil);
        assertf(right != nil);

        TOperand ltag = left->Tag;
        TOperand rtag = right->Tag;

        if (token==TokenADD) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = ADD;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_byte_seg_register) {
                instru->Tag = ADD2;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_byte_label) {
                instru->Tag = ADD2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = ADD3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = ADD3;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = ADD4;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = ADD4;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = ADD4;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = ADD5;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = ADD5;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = ADD6;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = ADD6;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = ADD6;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = ADD6;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = ADD6;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = ADD6;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: add %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenSHR) {
            if (rtag==operand_imm) {
                if (ltag==operand_register) {
                    instru->Tag = SHR;
                    update_location(4);
                }
                else if (ltag==operand_word_seg_register) {
                    instru->Tag = SHR2;
                    update_location(4);
                }
                else if (ltag==operand_word_label) {
                    instru->Tag = SHR2;
                    update_location(4 + 4 + (4));
                }
                else {
                    abortf("Error: shr %s, %s", operandstring(left), operandstring(right));
                }
            }
            else {
                abortf("Error: shr %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenSHL) {
            if (rtag==operand_imm) {
                if (ltag==operand_register) {
                    instru->Tag = SHL;
                    update_location(4);
                }
                else if (ltag==operand_word_seg_register) {
                    instru->Tag = SHL2;
                    update_location(4);
                }
                else if (ltag==operand_word_label) {
                    instru->Tag = SHL2;
                    update_location(4 + 4 + (4));
                }
                else {
                    abortf("Error: shl %s, %s", operandstring(left), operandstring(right));
                }
            }
            else {
                abortf("Error: shl %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenSAR) {
            if (rtag==operand_imm) {
                if (ltag==operand_register) {
                    instru->Tag = SAR;
                    update_location(4);
                }
                else if (ltag==operand_word_seg_register) {
                    instru->Tag = SAR2;
                    update_location(4);
                }
                else if (ltag==operand_word_label) {
                    instru->Tag = SAR2;
                    update_location(4 + 4 + (4));
                }
                else {
                    abortf("Error: sar %s, %s", operandstring(left), operandstring(right));
                }
            }
            else {
                abortf("Error: sar %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenAND) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = AND;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = AND2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = AND2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = AND3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = AND3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = AND3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = AND4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = AND4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = AND5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = AND5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = AND5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = AND5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = AND5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = AND5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: and %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenOR) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = OR;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = OR2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = OR2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = OR3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = OR3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = OR3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = OR4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = OR4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = OR5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = OR5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = OR5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = OR5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = OR5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = OR5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: or %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenXOR) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = XOR;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = XOR2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = XOR2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = XOR3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = XOR3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = XOR3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = XOR4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = XOR4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = XOR5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = XOR5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = XOR5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = XOR5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = XOR5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = XOR5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: xor %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenCMP) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = CMP;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = CMP2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = CMP2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = CMP3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = CMP3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = CMP3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = CMP4;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = CMP4;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = CMP4;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = CMP4;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = CMP4;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = CMP4;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: cmp %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenADC) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = ADC;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = ADC2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = ADC2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = ADC3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = ADC3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = ADC3;
                update_location(4);
            }
            else {
                abortf("Error: adc %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenSUB) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = SUB;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = SUB2;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = SUB2;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = SUB2;
                update_location(4);
            }
            else {
                abortf("Error: sub %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenMUL) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = MUL;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = MUL2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = MUL2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = MUL3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = MUL3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = MUL3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = MUL4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = MUL4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = MUL5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = MUL5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = MUL5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = MUL5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = MUL5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = MUL5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: mul %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenDIV) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = DIV;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = DIV2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = DIV2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = DIV3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = DIV3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = DIV3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = DIV4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = DIV4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = DIV5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = DIV5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = DIV5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = DIV5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = DIV5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = DIV5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: div %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenMOD) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = MOD;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = MOD2;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = MOD2;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = MOD3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = MOD3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = MOD3;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = MOD4;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = MOD4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = MOD5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = MOD5;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = MOD5;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = MOD5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = MOD5;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = MOD5;
                update_location(4 + 4 + (4));
            }
            else {
                abortf("Error: mod %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenMOV) {
            if (ltag==operand_register && rtag==operand_imm) {
                instru->Tag = MOV;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_sectionpart) {
                instru->Tag = MOV;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_label_offsetpart) {
                instru->Tag = MOV;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_byte_seg_imm16) {
                instru->Tag = MOV2;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_imm16) {
                instru->Tag = MOV3;
                update_location(4);
            }
            else if (ltag==operand_register && rtag==operand_byte_seg_register) {
                instru->Tag = MOV4;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_byte_label) {
                instru->Tag = MOV4;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_word_seg_register) {
                instru->Tag = MOV5;
                update_location(2);
            }
            else if (ltag==operand_register && rtag==operand_word_label) {
                instru->Tag = MOV5;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = MOV6;
                update_location(2);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_imm) {
                instru->Tag = MOV7;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_sectionpart) {
                instru->Tag = MOV7;
                update_location(4);
            }
            else if (ltag==operand_word_seg_register && rtag==operand_label_offsetpart) {
                instru->Tag = MOV7;
                update_location(4);
            }
            else if (ltag==operand_word_label && rtag==operand_imm) {
                instru->Tag = MOV7;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_sectionpart) {
                instru->Tag = MOV7;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_label && rtag==operand_label_offsetpart) {
                instru->Tag = MOV7;
                update_location(4 + 4 + (4));
            }
            else if (ltag==operand_word_seg_register && rtag==operand_register) {
                instru->Tag = MOV8;
                update_location(2);
            }
            else if (ltag==operand_word_label && rtag==operand_register) {
                instru->Tag = MOV8;
                update_location(2 + 4 + 4 + (2) + 2);
            }
            else {
                abortf("Error: mov %s, %s", operandstring(left), operandstring(right));
            }
        }
        else if (token==TokenXCHG) {
            if (ltag==operand_register && rtag==operand_register) {
                instru->Tag = XCHG;
                update_location(2);
            }
            else {
                abortf("Error: xhcg %s, %s", operandstring(left), operandstring(right));
            }
        }
    }
    else if (Token==TokenFLIP|| 
            Token==TokenNEG  || 
            Token==TokenRETN || 
            Token==TokenPUSH || 
            Token==TokenPOP  ||
            Token==TokenINC  || 
            Token==TokenDEC  ||
            Token==TokenRESB || 
            Token==TokenRESW) 
    {
        TToken token = Token;
        scan();
        analyze_1_operand(instru);

        Operand * op = instru->single;
        TOperand tag = op->Tag;

        if (token==TokenFLIP) {
            if (tag==operand_register) {
                instru->Tag = FLIP;
                update_location(2);
            }
            else if (tag==operand_word_seg_register) {
                instru->Tag = FLIP2;
                update_location(2);
            }
            else if (tag==operand_word_label) {
                instru->Tag = FLIP2;
                update_location(4 + 4 + (2));
            }
            else {
                abortf("Error: flip %s", operandstring(op));
            }
        }
        else if (token==TokenNEG) {
            if (tag==operand_register) {
                instru->Tag = NEG;
                update_location(2);
            }
            else if (tag==operand_word_seg_register) {
                instru->Tag = NEG2;
                update_location(2);
            }
            else if (tag==operand_word_label) {
                instru->Tag = NEG2;
                update_location(4 + 4 + (2));
            }
            else {
                abortf("Error: neg %s", operandstring(op));
            }
        }
        else if (token==TokenRETN) {
            if (tag==operand_imm) {
                instru->Tag = RETN;
                int n = strtoi(op->IMM);
                assertf(n >= 0);
                update_location(2 + 2 + 2 + n);
            }
            else {
                abortf("Error: retn %s", operandstring(op));
            }
        }
        else if (token==TokenPUSH) {
            if (tag==operand_register) {
                instru->Tag = PUSH;
                update_location(2);
            }
            else if (tag==operand_imm) {
                instru->Tag = PUSH2;
                update_location(4);
            }
            else if (tag==operand_byte_seg_register) {
                instru->Tag = PUSH3;
                update_location(2);
            }
            else if (tag==operand_byte_label) {
                instru->Tag = PUSH3;
                update_location(4 + 4 + 2);
            }
            else if (tag==operand_word_seg_register) {
                instru->Tag = PUSH4;
                update_location(2);
            }
            else if (tag==operand_word_label) {
                instru->Tag = PUSH4;
                update_location(4 + 4 + 2);
            }
            else {
                abortf("Error: push %s", operandstring(op));
            }
        }
        else if (token==TokenPOP) {
            if (tag==operand_register) {
                instru->Tag = POP;
                update_location(2);
            }
            else if (tag==operand_byte_seg_register) {
                instru->Tag = POP2;
                update_location(2);
            }
            else if (tag==operand_byte_label) {
                instru->Tag = POP2;
                update_location(4 + 4 + 2);
            }
            else if (tag==operand_word_seg_register) {
                instru->Tag = POP3;
                update_location(2);
            }
            else if (tag==operand_word_label) {
                instru->Tag = POP3;
                update_location(4 + 4 + 2);
            }
            else {
                abortf("Error: pop %s", operandstring(op));
            }
        }
        else if (token==TokenINC) {
            if (tag==operand_register) {
                Operand * left = malloc(sizeof(Operand));
                Operand * right = malloc(sizeof(Operand));
                assertf(left != nil);
                assertf(right != nil);

                left->Tag = operand_register;
                left->Register = strdup(op->Register);

                right->Tag = operand_imm;
                right->IMM = strdup("1");

                operandfree(op);
                
                instru->Tag = ADD4;
                instru->left = left;
                instru->right = right;

                update_location(4);
            }
            else {
                abortf("Error: inc %s", operandstring(op));
            }
        }
        else if (token==TokenDEC) {
            if (tag==operand_register) {
                Operand * left = malloc(sizeof(Operand));
                Operand * right = malloc(sizeof(Operand));
                assertf(left != nil);
                assertf(right != nil);

                left->Tag = operand_register;
                left->Register = strdup(op->Register);

                right->Tag = operand_imm;
                right->IMM = strdup("1");

                operandfree(op);
                
                instru->Tag = SUB2;
                instru->left = left;
                instru->right = right;

                update_location(4);
            }
            else {
                abortf("Error: dec %s", operandstring(op));
            }
        }
        else if (token==TokenRESB) {
            if (tag==operand_imm) {
                instru->Tag = RESB;
                int n = strtoi(op->IMM);
                if (n % 2 != 0) {
                    update_location(n + 1);
                }
                else {
                    update_location(n);
                }
            }
            else {
                abortf("Error: resb %s", operandstring(op));
            }
        }
        else if (token==TokenRESW) {
            if (tag==operand_imm) {
                instru->Tag = RESW;
                int n = strtoi(op->IMM);
                update_location(n * 2);
            }
            else {
                abortf("Error: resw %s", operandstring(op));
            }
        }
    }
    else if (Token==TokenJMP ||
            Token==TokenCALL ||
            Token==TokenJA   || 
            Token==TokenJNA  || 
            Token==TokenJB   || 
            Token==TokenJNB  ||
            Token==TokenJC   || 
            Token==TokenJNC  ||
            Token==TokenJP   || 
            Token==TokenJNP  ||
            Token==TokenJZ   || 
            Token==TokenJNZ  ||
            Token==TokenJR   || 
            Token==TokenJNR  ||
            Token==TokenJEV  || 
            Token==TokenJNEV ||
            Token==TokenJEQ  || 
            Token==TokenJNEQ) 
    {
        TToken token = Token;
        scan();

        analyze_jump_operand(instru);
        Operand * op = instru->single;
        TOperand tag = op->Tag;

        if (token==TokenJMP) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JMP;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JMP2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JMP2;
                update_location(2);
            }
            else {
                abortf("Error: jmp %s", operandstring(op));
            }
        }
        else if (token==TokenJA) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JA;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JA2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JA2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNA) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNA;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNA2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNA2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJB) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JB;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JB2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JB2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNB) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNB;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNB2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNB2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJEQ) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JEQ;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JEQ2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JEQ2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNEQ) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNEQ;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNEQ2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNEQ2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJC) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JC;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JC2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JC2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNC) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNC;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNC2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNC2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJEV) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JEV;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JEV2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JEV2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNEV) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNEV;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNEV2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNEV2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJP) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JP;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JP2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JP2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNP) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNP;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNP2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNP2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJZ) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JZ;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JZ2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JZ2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNZ) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNZ;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNZ2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNZ2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJR) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JR;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JR2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JR2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenJNR) {
            if (tag==operand_j_seg_imm16) {
                instru->Tag = JNR;
                update_location(4);
            }
            else if (tag==operand_j_label) {
                instru->Tag = JNR2;
                update_location(4 + 4 + (2));
            }
            else if (tag==operand_j_seg_register) {
                instru->Tag = JNR2;
                update_location(2);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
        else if (token==TokenCALL) {
            if (tag==operand_j_label) {
                instru->Tag = CALL;
                update_location(2 + 2 + 4 + 4 + 10);
            }
            else {
                abortf("Error: %s", operandstring(op));
            }
        }
    }
    else if (Token==TokenRET) {
        scan();
        instru->Tag = RET;
        update_location(2 + 2 + 2);
    }
    else if (Token==TokenPOP0) {
        scan();
        instru->Tag = POP0;
        update_location(2);
    }
    else if (Token==TokenNOP) {
        scan();
        instru->Tag = NOP;
        update_location(2);
    }
    else if (Token==TokenEND) {
        scan();
        instru->Tag = END;
        update_location(2);
    }
    else if (Token==TokenDB) {
        scan();
        instru->Tag = DB;
        analyze_db(instru);

        int count = instru->single->DCOUNT;
        if (count % 2 != 0) {
            update_location(count + 1);
        }
        else {
            update_location(count);
        }
    }
    else if (Token==TokenDW) {
        scan();
        instru->Tag = DW;
        analyze_dw(instru);
        int count = instru->single->DCOUNT;
        update_location(count * 2);
    }
    else if (Token==TokenSECTION) {
        scan();
        analyze_section(instru);
    }
    else if (Token==TokenPercentage) {
        // For macros...
        // More feature in the future...
    }
    else {
        abortf("invalid token: %d, invalid symbol: %s", Token, Symbol);
    }

    return instru;
}


