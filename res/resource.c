#include "header.h"
#include "table.h"


// input  file name
char * in = nil;
// output file name
char * out = nil;
TTable * LabelTable = nil;
u64 Section = 0;
u64 Location = 0;
char * Label = nil;
FILE * In = nil;
FILE * Out = nil;
int CharIn;
TToken Token;
String Symbol = nil;
Instruction * Instru = nil;

const u8 instru_size_list[] = {
    2,
    4,
    4,
    2,
    2,
    2,
    2,
    2,
    2,
    4,
    2,
    2,
    2,
    2,
    2,
    4,
    4,
    4,
    2,
    2,
    2,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    4,
    2,
    6,
    4,
    4,
    2,
    2,
    4,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    4,
    4,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    4,
    4,
    4,
    4,
    4,
    4,
    4,

    4, // 4 + n, dynamical size
    2, // NOP
    2, // END
    4,
    4,

    0, // dynamic size, db
    0, // dynamic size, dw
};

const char * const opcode_list[] = {
    "\tbbbb aaaa 0000 0000\n",
    "\t0000 aaaa 0001 0000\n\txxxx 0000 0000 0000\n",
    "\t0000 aaaa 0010 0000\n\txxxx 0000 0000 0000\n",
    "\t0000 aaaa 0011 0000\n",
    "\tbbbb aaaa 0100 0000\n",
    "\tbbbb aaaa 0101 0000\n",
    "\tbbbb aaaa 0110 0000\n",
    "\tbbbb aaaa 0111 0000\n",
    "\tbbbb aaaa 1000 0000\n",
    "\t0000 aaaa 1001 0000\n\txxxx 0000 0000 0000\n",
    "\t0000 aaaa 1011 0000\n",
    "\tbbbb aaaa 1100 0000\n",
    "\tbbbb aaaa 1101 0000\n",
    "\tbbbb aaaa 1110 0000\n",
    "\tbbbb aaaa 1111 0000\n",
    "\t0000 aaaa 0000 0001\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0001 0001\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0010 0001\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0011 0001\n",
    "\tbbbb aaaa 0100 0001\n",
    "\tbbbb aaaa 0101 0001\n",
    "\tbbbb aaaa 0110 0001\n",
    "\tbbbb aaaa 0111 0001\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1000 0001\n",
    "\t0000 aaaa 0001 0010\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0010 0010\n",
    "\t0000 aaaa 0100 0010\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0101 0010\n",
    "\t0000 aaaa 0111 0010\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1000 0010\n",
    "\t0000 aaaa 1010 0010\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1011 0010\n",
    "\t0000 aaaa 1101 0010\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1110 0010\n",
    "\t0000 aaaa 0000 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0001 0011\n",
    "\t0000 aaaa 0011 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0100 0011\n",
    "\t0000 aaaa 0110 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0111 0011\n",
    "\t0000 aaaa 1001 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1010 0011\n",
    "\t0000 aaaa 1100 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1101 0011\n",
    "\t0000 aaaa 1111 0011\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0001 0100\n",
    "\t0000 aaaa 0010 0100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0011 0100\n",
    "\t0000 aaaa 0101 0100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0110 0100\n",
    "\t0000 aaaa 1000 0100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1001 0100\n",
    "\t0000 aaaa 1011 0100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1100 0100\n",
    "\t0000 aaaa 1110 0100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1111 0100\n",
    "\t0000 aaaa 0001 0101\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0010 0101\n",
    "\t0000 1111 0000 1000\n\t0000 aaaa 0001 0010\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 1111 0000 1000\n\tbbbb aaaa 0010 0010\n",
    "\t1101 1000 0010 0010\n\t0000 0000 1011 1000\n",
    "\t0000 aaaa 0000 1000\n",
    "\t0000 aaaa 0001 1000\n",
    "\t0000 0000 0010 1000\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0011 1000\n",
    "\tbbbb aaaa 0100 1000\n",
    "\tbbbb aaaa 1001 1000\n",
    "\tbbbb aaaa 1010 1000\n",
    "\t0000 0000 1011 1000\n",
    "\tbbbb aaaa 0000 1010\n",
    "\tbbbb aaaa 0000 1011\n",
    "\tbbbb aaaa 0001 1011\n\txxxx 0000 0000 0000\n",
    "\tbbbb aaaa 0010 1011\n\txxxx 0000 0000 0000\n",
    "\tbbbb aaaa 0011 1011\n",
    "\tbbbb aaaa 0100 1011\n",
    "\tbbbb aaaa 0101 1011\n",
    "\tbbbb aaaa 0110 1011\n",
    "\tbbbb aaaa 0111 1011\n",
    "\tbbbb aaaa 1000 1011\n",
    "\tbbbb aaaa 1001 1011\n\txxxx 0000 0000 0000\n",
    "\tbbbb aaaa 1011 1011\n",
    "\tbbbb aaaa 1101 1011\n",
    "\tbbbb aaaa 1110 1011\n",
    "\tbbbb aaaa 1111 1011\n",
    "\t0000 aaaa 0000 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 0100 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 0101 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 0110 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 1000 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 1100 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 1101 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 1110 1100\n\txxxx xxxx xxxx xxxx\n",
    "\t0000 aaaa 1111 1100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0000 1101\n",
    "\tbbbb aaaa 0100 1101\n",
    "\tbbbb aaaa 0101 1101\n",
    "\tbbbb aaaa 0111 1101\n",
    "\tbbbb aaaa 1101 1101\n",
    "\tbbbb aaaa 1110 1101\n",
    "\tbbbb aaaa 1111 1101\n",
    "\tbbbb aaaa 0000 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0100 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0101 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0110 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1101 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1110 1110\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 1111 1110\n\txxxx xxxx xxxx xxxx\n",
    "\t1101 1000 0010 0010\n\t0000 0000 1011 1000\n",
    "\t0000 0000 1110 1111\n",
    "\t0000 0000 1111 1111\n",
    "\t0000 aaaa 0111 1100\n\txxxx xxxx xxxx xxxx\n",
    "\tbbbb aaaa 0111 1110\n\txxxx xxxx xxxx xxxx\n",

    "\txxxx xxxx xxxx xxxx\n",
    "\txxxx xxxx xxxx xxxx\n",

    "",
};
const char * const register_name_list[] = {
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",

    "ss",
    "cs",
    "ds",
    "es",
    "fs",
    "sp",
    "bp",
    "ip",

    ""
};
const char * const register_code_list[] = {
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111",

    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111",

    "",
};
const char * const kwlist[] = {
    "add",
    "shr",
    "shl",
    "flip",
    "and",
    "or",
    "xor",
    "cmp",
    "adc",
    "sar",
    "neg",
    "sub",
    "mul",
    "div",
    "mod",
    "mov",
    "xchg",
    "jmp",
    "ja",
    "jna",
    "jb",
    "jnb",
    "jeq",
    "jneq",
    "jc",
    "jnc",
    "jev",
    "jnev",
    "jp",
    "jnp",
    "jz",
    "jnz",
    "jr",
    "jnr",
    "call",
    "ret",
    "retn",
    "push",
    "pop",
    "pop0",
    "nop",
    "END",

    "db",
    "dw",
    "resb",
    "resw",
    "inc",
    "dec",

    "section",

    "define",
    "include",

    "byte",
    "word",
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",
    "ss",
    "cs",
    "ds",
    "es",
    "fs",
    "sp",
    "bp",
    "ip",

    "wrt",

    ":",
    ",",
    "<",
    ">",
    "<=",
    ">=",
    "<>",
    "!=",
    "<<",
    ">>",
    "+",
    "-",
    "*",
    "/",
    "%",
    "!",
    "|",
    "&",
    "^",
    "||",
    "&&",
    "^^",
    "\\",
    "\'",
    "\"",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    "$",

    "",
};

void debug(const char * fmt, ...) {
    time_t now = time(NULL);
    if (now == (time_t) -1) {
        exit(2);
    }

    struct tm * localtm = localtime(&now);
    if (!localtm) {
        exit(3);
    }

    char * timestr = asctime(localtm);
    if (!timestr) {
        exit(4);
    }

    fprintf(stderr, "\t%s\t", timestr);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n\n");
}

void abortf(const char * fmt, ...) {
    fprintf(stderr, "Error: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void errorf(const char * file, const int line, const char * fmt, ...) {
    fprintf(stderr, "%s:%d: ", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

