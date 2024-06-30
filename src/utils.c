#include "rasm.h"
#include "utils.h"

extern FILE * In;
extern FILE * Out;
extern int CharIn;
extern TToken Token;
extern String Symbol;
extern u64 Section;
extern u64 Location;
extern Instruction * Instru;
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
            assertf(ret == 0);
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
            assertf(ret == 0);
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

void tokenfree(void) {
    Token = TokenUnknown;
    Free(Symbol);
}

static void parse_ident(void) {
    assertf(is_label(CharIn));


    while (is_label(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }


    skipnewline();
}
static void parse_hex_digit(void) {
    assertf(is_hexdigit(CharIn));


    while (is_hexdigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }


    skipnewline();
}
static void parse_oct_digit(void) {


    while (is_octdigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }


    skipnewline();
}
static void parse_bin_digit(void) {
    assertf(is_bindigit(CharIn));
    
    while (is_bindigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }
    skipnewline();
}
static void parse_digit(void) {
    assertf(is_digit(CharIn));


    if (CharIn == '0') {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
        if (CharIn == 'x' || CharIn == 'X') {
            Symbol = strapp(Symbol, (char *) &CharIn);
            readin();
            parse_hex_digit();
        }
        else if (CharIn == 'b' || CharIn == 'B') {
            Symbol = strapp(Symbol, (char *) &CharIn);
            readin();
            parse_bin_digit();
        }
        else if (is_octdigit(CharIn)) {
            Symbol = strapp(Symbol, (char *) &CharIn);
            readin();
            parse_oct_digit();
        }
        else if (is_newline(CharIn) || is_whitespace(CharIn)) {
        }
        else {
            abortf("Error: %s is not a valid numeric string!", Symbol);
        }
        return;
    }


    while (is_digit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }


    skipnewline();
}

void scan(void) {
    tokenfree();
    skipnewline();

    if (is_letter(CharIn) || CharIn=='.') {
        parse_ident();
        int index = search(kwlist, Symbol);
        if (index == -1) {
            Token = TokenIdent;
        }
        else {
            Token = (TToken) index;
        }
    }
    else if (is_digit(CharIn)) {
        parse_digit();
        Token = TokenInteger;
    }
    else if (CharIn == ':') {
        Token = TokenColon;
        readin();
        skipnewline();
    }
    else if (CharIn == ',') {
        Token = TokenComma;
        readin();
        skipnewline();
    }
    else if (CharIn == '$') {
        Token = TokenDollar;
        readin();
        skipnewline();
    }
    else if (CharIn == '\'') {
        Token = TokenQuote;
        readin();
    }
    else if (CharIn == '\"') {
        Token = TokenQuote2;
        readin();
    }
    else if (CharIn == '(') {
        Token = TokenLParen;
        readin();
        skipnewline();
    }
    else if (CharIn == ')') {
        Token = TokenRParen;
        readin();
        skipnewline();
    }
    else if (CharIn == '[') {
        Token = TokenLBracket;
        readin();
        skipnewline();
    }
    else if (CharIn == ']') {
        Token = TokenRBracket;
        readin();
        skipnewline();
    }
    else if (CharIn == '{') {
        Token = TokenLBrace;
        readin();
        skipnewline();
    }
    else if (CharIn == '}') {
        Token = TokenRBrace;
        readin();
        skipnewline();
    }
    else if (is_EOF(CharIn)) {
        Token = TokenEOF;
    }
    else {
        abortf("Unknown symbol: %c", CharIn);
    }
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
    if (strcmp(reg, "ss") == 0) {
        return "1000";
    }
    else if (strcmp(reg, "cs") == 0) {
        return "1001";
    }
    else if (strcmp(reg, "ds") == 0) {
        return "1010";
    }
    else if (strcmp(reg, "sp") == 0) {
        return "1101";
    }
    else if (strcmp(reg, "bp") == 0) {
        return "1110";
    }
    else if (strcmp(reg, "ip") == 0) {
        return "1111";
    }
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

int hex2int(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a';
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A';
    }
    return -1;
}
