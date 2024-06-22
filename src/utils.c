#include "header.h"
#include "table.h"
#include "utils.h"
#include "str.h"
#include "instru.h"

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
extern TTable * LabelTable;

extern char * in;
extern char * out;

void parse_input(int argc, const char * const argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            assertf(i + 1 < argc, "Failed to parse input file name!");
            in = strdup(argv[i+1]);
            assertf(strloc(in, ".as") != -1, "%s is not a valid input file name, expected with .as extension!", in);
            return;
        }
    }
    abortf("Failed to parse input file name");
}
void parse_output(int argc, const char * const argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            assertf(i + 1 < argc, "Failed to parse output file name!");
            out = strdup(argv[i+1]);
            return;
        }
    }
    out = strdup("a.out");
}
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

static void parse_ident(void) {
    assert(is_label(CharIn));

    while (is_label(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }
    skipnewline();
}

static void parse_hex_digit(void) {
    assert(is_hexdigit(CharIn));

    while (is_hexdigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }
    skipnewline();
}
static void parse_oct_digit(void) {
    assert(is_octdigit(CharIn));
    
    while (is_octdigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }
    skipnewline();
}
static void parse_bin_digit(void) {
    assert(is_bindigit(CharIn));
    
    while (is_bindigit(CharIn)) {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
    }
    skipnewline();
}
static void parse_digit(void) {
    assert(is_digit(CharIn));

    if (CharIn == '0') {
        Symbol = strapp(Symbol, (char *) &CharIn);
        readin();
        Symbol = strapp(Symbol, (char *) &CharIn);
        if (CharIn == 'x' || CharIn == 'X') {
            readin();
            parse_hex_digit();
        }
        else if (CharIn == 'b' || CharIn == 'B') {
            readin();
            parse_bin_digit();
        }
        else if (is_octdigit(CharIn)) {
            readin();
            parse_oct_digit();
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
    clear();
    skipnewline();
    if (is_alpha(CharIn) || CharIn == '.') {
        parse_ident();
        int index = search(kwlist, Symbol);
        if (index == -1) {
            Token = TokenLabel;
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

void codegen(void) {
    Section = 0;
    Location = 0;
    instrulyze(Instru);
    instrufree(Instru);

    if (In) {
        assertf(fclose(In) == 0, "");
        In = nil;
    }
    if (Out) {
        assertf(fclose(Out) == 0, "");
        Out = nil;
    }
}
void bingen(void) {
    FILE * output = fopen(out, "wb");
    FILE * file   = fopen("a.txt", "w");
    assertf(output != nil, "");
    assertf(file != nil, "");

    u16 * content = calloc(0xffffff, sizeof(u16));
    assertf(content != nil, "");
    char * text = malloc(4 * 0xffffff * sizeof(char));
    assertf(text != nil, "");

    memset(text, '0', 4 * 0xffffff);



    In = fopen("temp.hex", "r");
    assertf(In != nil, "");
    readin();
    scan();
    do {
        assertf(Token==TokenLParen, "Expecting '(' but received '%s'", Symbol);

        scan();
        assertf(Token==TokenInteger, "");
        int loc = strtoi(Symbol);
        int loc2 = loc * 4;

        scan();
        assertf(Token==TokenRParen, "");

        scan();
        while (Token==TokenInteger) {
            u16 h = 0;
            for (int i = 0; i < 4; i++) {
                assertf(Token==TokenInteger, "");
                char * hex = nil;
                
                Symbol = strpre(Symbol, "0b");
                hex = itostr(strtoi(Symbol), -2);
                assertf(hex != nil, "");
                assertf(is_hexdigit(hex[0]) && hex[1] == '\0', "Hex: %s, Symbol: %s", hex, Symbol);
                text[loc2++] = hex[0];

                h += (hex2int(hex[0]) << (4 * i));
                free(hex);

                scan();
            }
            content[loc++] = h;
        }
    } while (Token != TokenEOF);
    assertf(fclose(In) == 0, "");



    int written;
    written = fwrite(content, sizeof(u16), 0xffffff, output);
    assertf(written == 0xffffff, "");
    fprintf(file, "v2.0 raw\n");
    for (int row = 0; row < 0xffffff / 16; row++) {
        fprintf(file, "# 0x%06x\n", row * 16);
        for (int i = 0; i < 64; i++) {
            int loc = row * 64 + i;
            fprintf(file, "%c", text[loc]);
            if ((i+1) % 4 == 0) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }

    assertf(fclose(output) == 0, "");
    assertf(fclose(file) == 0, "");
}

void init(int argc, const char * const argv[]) {
    parse_input(argc, argv);
    parse_output(argc, argv);

    In = fopen(in, "r");
    assertf(In != nil, "Failed to read file %s", in);
    Out = fopen("temp.hex", "w");

    LabelTable = Table_Init();
}
void quit(void) {
    if (in) {
        free(in);
        in = nil;
    }
    if (out) {
        free(out);
        out = nil;
    }
    Table_Free(LabelTable);
    LabelTable = nil;
    clear();
    assertf(remove("temp.hex") == 0, "");
}
