#include "header.h"
#include "utils.h"

extern FILE * In;
extern FILE * Out;
extern int CharIn;
extern TToken Token;
extern String Symbol;
extern u64 Section;
extern u64 Location;
extern const u8 instru_size_list[];
extern const char * const kwlist[];
extern const char * const opcode_list[];
extern const char * const register_name_list[];
extern const char * const register_code_list[];

int search(const char * const table[], const char * value) {
    assertf(table != nil, "Error @func {search}: @param {table} (null)");
    assertf(value != nil, "Value @func {search}: @param {value} (null)");
    int index = 0;
    while (table[index][0] != '\0') {
        if (strcmp(table[index], value) == 0) {
            return index;
        }
        index += 1;
    }
    return -1;
}

void readin(void) {
    CharIn = fgetc(In);
    if (CharIn == '/') {
        CharIn = fgetc(In);
        if (CharIn == '/') {
            // handling line-comment.
            do {
                CharIn = fgetc(In);
            } while (!is_newline(CharIn));
            skipnewline();
        }
        else if (CharIn == '*') {
            // @local {buf} in order to check for "*/",
            // to detect the end of the multi-comment block.
            char buf[3] = {0, 0, 0};
            int i = 0;
            bool multi_comment_end = false;
            do {
                if (strcmp(buf, "*/") == 0) {
                    multi_comment_end = true;
                }
                else {
                    if (i == 2) {
                        buf[0] = buf[1];
                        buf[1] = fgetc(In);
                    }
                    else {
                        buf[i] = fgetc(In);
                        i += 1;
                    }
                }
            } while (!multi_comment_end);
            CharIn = fgetc(In);
            skipnewline();
        }
        else {
            // not a comment, rewind the process.
            int ret = fseek(In, -1, SEEK_CUR);
            assert(ret == 0);
            CharIn = '/';
        }
    }
    else if (CharIn == '(') {
        CharIn = fgetc(In);
        if (CharIn == '*') {
            // @local {buf} in order to check for "*/",
            // to detect the end of the multi-comment block.
            char buf[3] = {0, 0, 0};
            int i = 0;
            bool multi_comment_end = false;
            do {
                if (strcmp(buf, "*)") == 0) {
                    multi_comment_end = true;
                }
                else {
                    if (i == 2) {
                        buf[0] = buf[1];
                        buf[1] = fgetc(In);
                    }
                    else {
                        buf[i] = fgetc(In);
                        i += 1;
                    }
                }
            } while (!multi_comment_end);
            CharIn = fgetc(In);
            skipnewline();
        }
        else {
            // not a comment, rewind the process.
            int ret = fseek(In, -1, SEEK_CUR);
            assert(ret == 0);
            CharIn = '(';
        }
    }
    else if (CharIn == ';') {
        do {
            CharIn = fgetc(In);
        } while (!is_newline(CharIn));
        skipnewline();
    }
}

void clear(void) {
    Token = TokenUnknown;
    if (Symbol) {
        free(Symbol);
        Symbol = nil;
    }
}

void scan(void) {
    clear();
}

bool is_upper(const char c) {
    return c >= 'a' && c <= 'z';
}
bool is_lower(const char c) {
    return c >= 'A' && c <= 'Z';
}
bool is_letter(const char c) {
    return is_upper(c) || is_lower(c);
}
bool is_alpha(const char c) {
    return is_letter(c) || c == '_';
}
bool is_alnum(const char c) {
    return is_alpha(c) || is_digit(c);
}
bool is_label(const char c) {
    return is_alnum(c) || c == '.';
}
bool is_digit(const char c) {
    return c >= '0' && c <= '9';
}
bool is_hexdigit(const char c) {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
bool is_octdigit(const char c) {
    return c >= '0' && c <= '7';
}
bool is_bindigit(const char c) {
    return c == '0' || c == '1';
}
bool is_EOF(const char c) {
    return c == EOF;
}
bool is_newline(const char c) {
    return c == '\r' || c == '\n';
}
bool is_whitespace(const char c) {
    return c == '\t' || c == ' ';
}

void skipwhite(void) {
    while (is_whitespace(CharIn)) {
        readin();
    }
}
void skipnewline(void) {
    while (is_newline(CharIn) || is_whitespace(CharIn)) {
        readin();
    }
}


bool is_register(const char * reg) {
    assertf(reg != nil, "Error @func {is_register}: @param {reg} is (null)");
    return search(register_name_list, reg) != -1;
}
bool is_token_register(TToken token) {
    return (token >= TokenR0 && Token <= TokenIP);
}
const char * register_code(const char * reg) {
    assertf(is_register(reg), "Error: %s is not a register name!", reg);
    return register_code_list[search(register_name_list, reg)];
}

void tabline(const char * fmt, ...) {
    fprintf(Out, "\t");
    va_list args;
    va_start(args, fmt);
    vfprintf(Out, fmt, args);
    va_end(args);
    fprintf(Out, "\n");
}
void writeln(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(Out, fmt, args);
    va_end(args);
}

void update_location(u64 byte_size) {
    assertf(Section <= UINT8_MAX, "Section Overflow: 0x%lx", Section);
    Location += byte_size;
    if (Location > UINT16_MAX) {
        Section += 1;
        Location -= UINT16_MAX;
    }
}

void instrulyze(Instruction * instru) {
}

void free_instru(Instruction * instru) {
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

