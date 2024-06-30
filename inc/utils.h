#ifndef __RASM_UTILS_H
#define __RASM_UTILS_H
#include "rasm.h"
#include "str.h"

bool is_upper(const char c);
bool is_lower(const char c);
bool is_letter(const char c);
bool is_alpha(const char c);
bool is_alnum(const char c);
bool is_label(const char c);
bool is_digit(const char c);
bool is_hexdigit(const char c);
bool is_octdigit(const char c);
bool is_bindigit(const char c);
bool is_EOF(const char c);
bool is_newline(const char c);
bool is_whitespace(const char c);

void skipwhite(void);
void skipnewline(void);

void readin(void);
void tokenfree(void);
void scan(void);
int search(const char * const table[], const char * value);

bool is_register(const char * reg);
bool is_token_register(TToken token);
const char * register_code(const char * reg);

void tabline(const char * fmt, ...);
void writeln(const char * fmt, ...);
int hex2int(const char c);

#endif // __RASM_UTILS_H
