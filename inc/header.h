#ifndef __HEADER_H
#define __HEADER_H
#if defined(__APPLE__)
    typedef char i8;
    typedef unsigned char u8;
    typedef short i16;
    typedef unsigned short u16;
    typedef int i32;
    typedef unsigned int u32;
    typedef long i64;
    typedef unsigned long u64;
#endif // __APPLE__
#if defined(__SIZEOF_INT128__)
    typedef __int128 i128;
    typedef unsigned __int128 u128;
#endif // __SIZEOF_INT128__
#define error(...) errorf(__FILE__, __LINE__, __VA_ARGS__)
#define assert(expr)                                        \
    do {                                                    \
        if (!(expr)) error("Assertion failed: " #expr);     \
    } while (0)
#define assertf(expr, ...)                                  \
    do {                                                    \
        if (!(expr)) error(__VA_ARGS__);                    \
    } while (0)
#define nil NULL

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <time.h>

typedef long long           ll;
typedef unsigned long       ul;
typedef unsigned long long  ull;
typedef char *              String;

typedef enum TInstru {
    ADD,    // 0. add ra, rb
    SHR,    // 1. shr ra, imm4
    SHL,    // 2. shl ra, imm4
    FLIP,   // 3. flip ra
    AND,    // 4. and ra, rb
    OR,     // 5. or ra, rb
    XOR,    // 6. xor ra, rb
    CMP,    // 7. cmp ra, rb
    ADC,    // 8. adc ra, rb
    SAR,    // 9. sar ra, imm4
    NEG,    // 10. neg ra
    SUB,    // 11. sub ra, rb
    MUL,    // 12. mul ra, rb
    DIV,    // 13. div ra, rb
    MOD,    // 14. mod ra, rb
    MOV,    // 15. mov ra, imm16
    MOV2,   // 16. mov ra, byte [rb:imm16]
    MOV3,   // 17. mov ra, [rb:imm16] 
            //     mov ra, word [rb:imm16]
    MOV4,   // 18. mov ra, byte [rb]
            //     mov ra, byte [ds:rb]
    MOV5,   // 19. mov ra, [rb]
            //     mov ra, [ds:rb]
            //     mov ra, word [rb]
            //     mov ra, word [ds:rb]
    XCHG,   // 20. xchg ra, rb
    MOV6,   // 21. mov ra, rb
    MOV7,   // 22. mov [ra:rb], imm16
            //     mov word [ra:rb], imm16
    MOV8,   // 23. mov [ra], rb
            //     mov [ds:ra], rb
            //     mov word [ra], rb
            //     mov word [ds:ra], rb
    JMP,    // 24. jmp ra:imm16
    JMP2,   // 25. jmp ra:rb
    JA,     // 26. ja ra:imm16
    JA2,    // 27. ja ra:rb
    JNA,    // 28. jna ra:imm16
    JNA2,   // 29. jna ra:rb
    JB,     // 30. jb ra:imm16
    JB2,    // 31. jb ra:rb
    JNB,    // 32. jnb ra:imm16
    JNB2,   // 33. jnb ra:rb
    JEQ,    // 34. jeq ra:imm16
    JEQ2,   // 35. jeq ra:rb
    JNEQ,   // 36. jneq ra:imm16
    JNEQ2,  // 37. jneq ra:rb
    JC,     // 38. jc ra:imm16
    JC2,    // 39. jc ra:rb
    JNC,    // 40. jnc ra:imm16
    JNC2,   // 41. jnc ra:rb
    JEV,    // 42. jev ra:imm16
    JEV2,   // 43. jev ra:rb
    JNEV,   // 44. jnev ra:imm16
    JNEV2,  // 45. jnev ra:rb
    JP,     // 46. jp ra:imm16
    JP2,    // 47. jp ra:rb
    JNP,    // 48. jnp ra:imm16
    JNP2,   // 49. jnp ra:rb
    JZ,     // 50. jz ra:imm16
    JZ2,    // 51. jz ra:rb
    JNZ,    // 52. jnz ra:imm16
    JNZ2,   // 53. jnz ra:rb
    JR,     // 54. jr ra:imm16
    JR2,    // 55. jr ra:rb
    JNR,    // 56. jnr ra:imm16
    JNR2,   // 57. jnr ra:rb
    CALL,   // 58. call ra:imm16
    CALL2,  // 59. call ra:rb
    RET,    // 60. ret
    PUSH,   // 61. push ra
    POP,    // 62. pop ra
    PUSH2,  // 63. push imm16
    PUSH3,  // 64. push byte [ra:rb]
    PUSH4,  // 65. push [ra:rb]
            //     push word [ra:rb]
    POP2,   // 66. pop byte [ra:rb]
    POP3,   // 67. pop [ra:rb]
            //     pop word [ra:rb]
    POP0,   // 68. pop0
    ADD2,   // 69. add ra, byte [rb]
            //     add ra, byte [ds:rb]
    ADD3,   // 70. add ra, [rb]
            //     add ra, [ds:rb]
            //     add ra, word [rb]
            //     add ra, word [ds:rb]
    SHR2,   // 71. shr [ra:rb], imm4
            //     shr word [ra:rb], imm4
    SHL2,   // 72. shl [ra:rb], imm4
            //     shl word [ra:rb], imm4
    FLIP2,  // 73. flip [ra:rb]
            //     flip word [ra:rb]
    AND2,   // 74. and ra, [rb]
            //     and ra, [ds:rb]
            //     and ra, word [rb]
            //     and ra, word [ds:rb]
    OR2,    // 75. or ra, [rb]
            //     or ra, [ds:rb]
            //     or ra, word [rb]
            //     or ra, word [ds:rb]
    XOR2,   // 76. xor ra, [rb]
            //     xor ra, [ds:rb]
            //     xor ra, word [rb]
            //     xor ra, word [ds:rb]
    CMP2,   // 77. cmp ra, [rb]
            //     cmp ra, [ds:rb]
            //     cmp ra, word [rb]
            //     cmp ra, word [ds:rb]
    ADC2,   // 78. adc ra, [rb]
            //     adc ra, [ds:rb]
            //     adc ra, word [rb]
            //     adc ra, word [ds:rb]
    SAR2,   // 79. sar [ra:rb], imm4
            //     sar word [ra:rb], imm4
    NEG2,   // 80. neg [ra:rb]
            //     neg word [ra:rb]
    MUL2,   // 81. mul ra, [rb]
            //     mul ra, [ds:rb]
            //     mul ra, word [rb]
            //     mul ra, word [ds:rb]
    DIV2,   // 82. div ra, [rb]
            //     div ra, [ds:rb]
            //     div ra, word [rb]
            //     div ra, word [ds:rb]
    MOD2,   // 83. mod ra, [rb]
            //     mod ra, [ds:rb]
            //     mod ra, word [rb]
            //     mod ra, word [ds:rb]
    ADD4,   // 84. add ra, imm16
    AND3,   // 85. and ra, imm16
    OR3,    // 86. or ra, imm16
    XOR3,   // 87. xor ra, imm16
    ADC3,   // 88. adc ra, imm16
    SUB2,   // 89. sub ra, imm16
    MUL3,   // 90. mul ra, imm16
    DIV3,   // 91. div ra, imm16
    MOD3,   // 92. mod ra, imm16
    ADD5,   // 93. add [ra], rb
            //     add [ds:ra], rb
            //     add word [ra], rb
            //     add word [ds:ra], rb
    AND4,   // 94. and [ra], rb
            //     and [ds:ra], rb
            //     and word [ra], rb
            //     and word [ds:ra], rb
    OR4,    // 95. or [ra], rb
            //     or [ds:ra], rb
            //     or word [ra], rb
            //     or word [ds:ra], rb
    XOR4,   // 96. xor [ra], rb
            //     xor [ds:ra], rb
            //     xor word [ra], rb
            //     xor word [ds:ra], rb
    MUL4,   // 97. mul [ra], rb
            //     mul [ds:ra], rb
            //     mul word [ra], rb
            //     mul word [ds:ra], rb
    DIV4,   // 98. div [ra], rb
            //     div [ds:ra], rb
            //     div word [ra], rb
            //     div word [ds:ra], rb
    MOD4,   // 99. mod [ra], rb
            //     mod [ds:ra], rb
            //     mod word [ra], rb
            //     mod word [ds:ra], rb
    ADD6,   // 100. add [ra:rb], imm16
            //      add word [ra:rb], imm16
    AND5,   // 101. and [ra:rb], imm16
            //      and word [ra:rb], imm16
    OR5,    // 102. or [ra:rb], imm16
            //      or word [ra:rb], imm16
    XOR5,   // 103. xor [ra:rb], imm16
            //      xor word [ra:rb], imm16
    MUL5,   // 104. mul [ra:rb], imm16
            //      mul word [ra:rb], imm16
    DIV5,   // 105. div [ra:rb], imm16
            //      div word [ra:rb], imm16
    MOD5,   // 106. mod [ra:rb], imm16
            //      mod word [ra:rb], imm16
    
    RETN,   // 107. retn n
    NOP,    // 108. nop
    END,    // 109. END
    CMP3,   // 110. cmp ra, imm16
    CMP4,   // 111. cmp word [ra:rb], imm16

    DB,
    DW,

    RESB,
    RESW,
    SECTION,


    instr_count,
} TInstru;

typedef enum {
    operand_register,
    
    operand_imm,
    operand_resb,
    operand_resw,
    operand_label_sectionpart,
    operand_label_offsetpart,
    
    operand_byte_label,
    operand_byte_seg_imm16,
    operand_byte_seg_register,

    operand_word_label,
    operand_word_seg_imm16,
    operand_word_seg_register,
    
    operand_db,
    operand_dw,
    operand_section,

    operand_j_label,
    operand_j_seg_imm16,
    operand_j_seg_register,
    
    operand_none,

    operand_type_count,
} TOperand;

typedef struct {
    TOperand Tag;

    union {
        char * Register;
        char * IMM;
        
        char * Label;

        struct {
            char * Segment;
            char * Offset;
        };

        struct {
            union {
                char * DB;
                int  * DW;
            };
            int DCOUNT;
        };

        struct {
            u64 section;
        };
    };
} Operand;

typedef struct Instruction {
    TInstru Tag;

    union {
        Operand * operand;
        struct {
            Operand * left;
            Operand * right;
        };
    };
    
    struct Instruction * next;
} Instruction;

typedef enum {
    TokenADD,
    TokenSHR,
    TokenSHL,
    TokenFLIP,
    TokenAND,
    TokenOR,
    TokenXOR,
    TokenCMP,
    TokenADC,
    TokenSAR,
    TokenNEG,
    TokenSUB,
    TokenMUL,
    TokenDIV,
    TokenMOD,
    TokenMOV,
    TokenXCHG,
    TokenJMP,
    TokenJA,
    TokenJNA,
    TokenJB,
    TokenJNB,
    TokenJEQ,
    TokenJNEQ,
    TokenJC,
    TokenJNC,
    TokenJEV,
    TokenJNEV,
    TokenJP,
    TokenJNP,
    TokenJZ,
    TokenJNZ,
    TokenJR,
    TokenJNR,
    TokenCALL,
    TokenRET,
    TokenRETN,
    TokenPUSH,
    TokenPOP,
    TokenPOP0,
    TokenNOP,
    TokenEND,

    TokenDB,
    TokenDW,
    TokenRESB,
    TokenRESW,
    TokenINC,
    TokenDEC,
    TokenSECTION,
    TokenDEFINE,
    TokenINCLUDE,

    TokenByte,
    TokenWord,
    TokenR0,
    TokenR1,
    TokenR2,
    TokenR3,
    TokenR4,
    TokenR5,
    TokenR6,
    TokenR7,
    TokenR8,
    TokenR9,
    TokenR10,
    TokenR11,
    TokenR12,
    TokenR13,
    TokenR14,
    TokenR15,
    TokenSS,
    TokenCS,
    TokenDS,
    TokenES,
    TokenFS,
    TokenSP,
    TokenBP,
    TokenIP,

    TokenWRT,

    TokenColon,
    TokenComma,
    TokenLess,
    TokenGreater,
    TokenLessEq,
    TokenGreaterEq,
    TokenNotEq,
    TokenNotEq2,
    TokenLeftShift,
    TokenRightShift,
    TokenPlus,
    TokenMinus,
    TokenAsterisk,
    TokenSlash,
    TokenPercentage,
    TokenExclamation,
    TokenBar,
    TokenAmpersand,
    TokenCaret,
    TokenBar2,
    TokenAmpersand2,
    TokenCaret2,
    TokenBackSlash,
    TokenQuote,
    TokenQuote2,
    TokenLParen,
    TokenRParen,
    TokenLBracket,
    TokenRBracket,
    TokenLBrace,
    TokenRBrace,
    TokenDollar,

    TokenLabel,
    TokenInteger,
    TokenUnknown,
    TokenEOF,

    TokenCount,
} TToken;

void debug(const char * fmt, ...);
void abortf(const char * fmt, ...);
void errorf(const char * file, const int line, const char * fmt, ...);

#endif // __HEADER_H
