#include "operand.h"
#include "rasm.h"
#include "str.h"
#include "utils.h"
#include "table.h"


extern TTable * Labels;
extern TToken Token;
extern String Symbol;
extern u64 Section;
extern u64 Location;
extern char * Label;
extern int CharIn;

static void analyze_definebyte(Operand * op);
static void analyze_defineword(Operand * op);


Operand * analyze_operand(void) {
    Operand * op = malloc(sizeof(Operand));
    assertf(op != nil);

    if (Token==TokenByte) {
        scan();
        analyze_byte_reference(op);
    }
    else if (Token==TokenWord) {
        scan();
        analyze_word_reference(op);
    }
    else if (Token==TokenLBracket) {
        analyze_word_reference(op);
    }
    else if (Token==TokenInteger) {
        op->Tag = operand_imm;
        op->IMM = strdup(Symbol);
    }
    else if (Token==TokenWRT) {
        scan();
        op->Tag = operand_label_sectionpart;
        if (Token==TokenIdent) {
            if (Symbol[0]=='.') {
                assertf(Label != nil);
                Symbol = strpre(Symbol, Label);
            }
        }
        else if (Token==TokenDollar) {
            tokenfree();
            Symbol = strdup("$");
        }
        else {
            abortf("");
        }
        op->Label = strdup(Symbol);
    }
    else if (Token==TokenDollar) {
        op->Tag = operand_label_offsetpart;
        op->Label = strdup("$");
    }
    else if (Token==TokenIdent) {
        op->Tag = operand_label_offsetpart;
        if (Symbol[0] == '.') {
            assertf(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        op->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        op->Tag = operand_register;
        op->Register = strdup(Symbol);
    }
    else {
        abortf("strange token: %d, strange symbol: %s", Token, Symbol);
    }
    
    scan();
    return op;
}
void analyze_byte_reference(Operand * op) {
    assertf(op != nil);
    assertf(Token==TokenLBracket);
    scan();
    if (Token==TokenInteger) {
        op->Tag = operand_byte_seg_imm16;
        op->Segment = strdup("ds");
        op->Offset = strdup(Symbol);
    }
    else if (Token==TokenIdent) {
        op->Tag = operand_byte_label;
        if (Symbol[0]=='.') {
            assertf(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        op->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        char * reg = strdup(Symbol);
        assertf(reg != nil);
        scan();
        if (Token==TokenColon) {
            scan();
            if (Token==TokenInteger) {
                op->Tag = operand_byte_seg_imm16;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else if (is_token_register(Token)) {
                op->Tag = operand_word_seg_register;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else {
                abortf("strange token: %d, strange symbol: %s", Token, Symbol);
            }
            scan();
        }
        else {
            op->Tag = operand_byte_seg_register;
            op->Segment = strdup("ds");
            op->Offset = reg;
        }
    }
    else {
        abortf("strange token: %d, strange symbol: %s", Token, Symbol);
    }
    assertf(Token==TokenRBracket);
}
void analyze_word_reference(Operand * op) {
    assertf(op != nil);
    assertf(Token==TokenLBracket);
    scan();
    if (Token==TokenInteger) {
        op->Tag = operand_word_seg_imm16;
        op->Segment = strdup("ds");
        op->Offset = strdup(Symbol);
    }
    else if (Token==TokenIdent) {
        op->Tag = operand_word_label;
        if (Symbol[0]=='.') {
            assertf(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        op->Label = strdup(Symbol);
    }
    else if (is_token_register(Token)) {
        char * reg = strdup(Symbol);
        assertf(reg != nil);
        scan();
        if (Token==TokenColon) {
            scan();
            if (Token==TokenInteger) {
                op->Tag = operand_word_seg_imm16;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else if (is_token_register(Token)) {
                op->Tag = operand_word_seg_register;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else {
                abortf("strange token: %d, strange symbol: %s", Token, Symbol);
            }
            scan();
        }
        else {
            op->Tag = operand_word_seg_register;
            op->Segment = strdup("ds");
            op->Offset = reg;
        }
    }
    else {
        abortf("strange token: %d, strange symbol: %s", Token, Symbol);
    }
    assertf(Token==TokenRBracket);
}
void analyze_2_operands(Instruction * instru) {
    assertf(instru != nil);
    instru->left = analyze_operand();
    assertf(Token==TokenComma);
    scan();
    instru->right = analyze_operand();
}
void analyze_1_operand(Instruction * instru) {
    assertf(instru != nil);
    instru->single = analyze_operand();
}
void analyze_jump_operand(Instruction * instru) {
    Operand * op = malloc(sizeof(Operand));
    assertf(instru != nil);
    assertf(op != nil);


    if (Token==TokenIdent) {
        op->Tag = operand_j_label;
        if (Symbol[0]=='.') {
            assertf(Label != nil);
            Symbol = strpre(Symbol, Label);
        }
        op->Label = strdup(Symbol);
        scan();
    }
    else if (Token==TokenInteger) {
        op->Tag = operand_j_seg_imm16;
        op->Segment = strdup("cs");
        op->Offset = strdup(Symbol);
        scan();
    }
    else if (is_token_register(Token)) {
        char * reg = strdup(Symbol);
        assertf(reg != nil);
        scan();
        if (Token==TokenColon) {
            scan();
            if (is_token_register(Token)) {
                op->Tag = operand_j_seg_register;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else if (Token==TokenInteger) {
                op->Tag = operand_j_seg_imm16;
                op->Segment = reg;
                op->Offset = strdup(Symbol);
            }
            else {
                abortf("strange token: %d, strange symbol: %s", Token, Symbol);
            }
            scan();
        }
        else {
            op->Tag = operand_j_seg_register;
            op->Segment = strdup("cs");
            op->Offset = reg;
        }
    }
    else {
        abortf("strange token: %d, strange symbol: %s", Token, Symbol);
    }
    instru->single = op;
}

void operandfree(Operand * op) {
    if (!op) {
        return;
    }

    TOperand tag = op->Tag;
    switch (tag) {
        case operand_register:
        {
            free(op->Register);
        } break;

        case operand_imm:
        case operand_resb:
        case operand_resw:
        {
            free(op->IMM);
        } break;

        case operand_byte_label:
        case operand_word_label:
        case operand_j_label:
        case operand_label_sectionpart:
        case operand_label_offsetpart:
        {
            free(op->Label);
        } break;

        case operand_byte_seg_imm16:
        case operand_word_seg_imm16:
        case operand_byte_seg_register:
        case operand_word_seg_register:
        case operand_j_seg_imm16:
        case operand_j_seg_register:
        {
            free(op->Segment);
            free(op->Offset);
        } break;

        case operand_db:
        {
            free(op->DB);
        } break;
        case operand_dw:
        {
            free(op->DW);
        } break;

        case operand_section:
        {
        } break;
    }
    free(op);
}

char * operandstring(Operand * op) {
    assertf(op != nil);
    TOperand tag = op->Tag;
    switch (tag) {        
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

        case operand_section:
        {
            return "section";
        } break;
    }
    return nil;
}

void analyze_definebyte(Operand * op) {
    if (Token==TokenInteger) {
        op->DCOUNT += 1;
        op->DB = realloc(op->DB, sizeof(i8) * op->DCOUNT);
        int i = op->DCOUNT - 1;
        op->DB[i] = (i8) strtoi(Symbol);
    }
    else if (Token==TokenQuote) {
        assertf(CharIn != '\'');
        while (CharIn != '\'') {
            op->DCOUNT += 1;
            op->DB = realloc(op->DB, sizeof(i8) * op->DCOUNT);
            int i = op->DCOUNT - 1;
            op->DB[i] = (i8) CharIn;
            readin();
        }
        scan();
        assertf(Token==TokenQuote);
    }
    else if (Token==TokenQuote2) {
        assertf(CharIn != '\"');
        while (CharIn != '\"') {
            op->DCOUNT += 1;
            op->DB = realloc(op->DB, sizeof(i8) * op->DCOUNT);
            int i = op->DCOUNT - 1;
            op->DB[i] = (i8) CharIn;
            readin();
        }
        scan();
        assertf(Token==TokenQuote2);
    }
    else {
        abortf("invalid token: % d, invalid symbol: %s", Token, Symbol);
    }
    scan();
}

void analyze_defineword(Operand * op) {
    if (Token==TokenInteger) {
        op->DCOUNT += 1;
        op->DW = realloc(op->DW, sizeof(i16) * op->DCOUNT);
        int i = op->DCOUNT - 1;
        op->DW[i] = (i16) strtoi(Symbol);
    }
    else if (Token==TokenQuote) {
        assertf(CharIn != '\'');
        while (CharIn != '\'') {
            op->DCOUNT += 1;
            op->DW = realloc(op->DW, sizeof(i16) * op->DCOUNT);
            int i = op->DCOUNT - 1;
            op->DW[i] = (i16) CharIn;
            readin();
        }
        scan();
        assertf(Token==TokenQuote);
    }
    else if (Token==TokenQuote2) {
        assertf(CharIn != '\"');
        while (CharIn != '\"') {
            op->DCOUNT += 1;
            op->DW = realloc(op->DW, sizeof(i16) * op->DCOUNT);
            int i = op->DCOUNT - 1;
            op->DW[i] = (i16) CharIn;
            readin();
        }
        scan();
        assertf(Token==TokenQuote2);
    }
    else {
        abortf("invalid token: % d, invalid symbol: %s", Token, Symbol);
    }
    scan();
}

void analyze_db(Instruction * instru) {
    assertf(instru != nil);

    Operand * op = malloc(sizeof(Operand));
    assertf(op != nil);
    op->Tag = operand_db;
    op->DB = nil;
    op->DCOUNT = 0;

    analyze_definebyte(op);
    while (Token==TokenComma) {
        scan();
        analyze_definebyte(op);
    }
    
    instru->single = op;
}

void analyze_dw(Instruction * instru) {
    assertf(instru != nil);

    Operand * op = malloc(sizeof(Operand));
    assertf(op != nil);
    op->Tag = operand_dw;
    op->DW = nil;
    op->DCOUNT = 0;

    analyze_defineword(op);
    while (Token==TokenComma) {
        scan();
        analyze_defineword(op);
    }
    
    instru->single = op;
}

void analyze_section(Instruction * instru) {
    
    assertf(Token==TokenIdent);
    char * section = strdup(Symbol);
    
    scan();
    assertf(Token==TokenInteger, "expecting token: %d, unwanted token: %d, unwanted symbol: %s", TokenInteger, Token, Symbol);
    Section = strtoi(Symbol);
    assertf(Section <= 0xff, "The section value ranges from 0 to 0xff, but section %s has value 0x%lx", section, Section);
    Location = 0;
    
    Table_Set(Labels, section, Section);
    
    instru->Tag = SECTION;
    instru->single = malloc(sizeof(Operand));
    assertf(instru->single != nil);
    instru->single->Tag = operand_section;
    instru->single->imm = Section;
    
    free(section);
    scan();
}

