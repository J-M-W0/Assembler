#ifndef __UTILS_H
#define __UTILS_H
#include "header.h"

bool upper(const char c);
bool lower(const char c);
bool letter(const char c);
bool alpha(const char c);
bool alnum(const char c);
bool digit(const char c);
bool hexdigit(const char c);
bool octdigit(const char c);
bool bindigit(const char c);
bool isEOF(const char c);
bool newline(const char c);
bool whitespace(const char c);

void skipwhite(void);
void skipnewline(void);

void readin(void);
void clear(void);
void scan(void);
int search(const char * const table[], const char * value);

bool is_register(const char * reg);
bool is_token_register(TToken token);
const char * register_code(const char * reg);

void tabline(const char * fmt, ...);
void writeln(const char * fmt, ...);

void update_location(u64 byte_size);
void instrulyze(Instruction * instru);
void free_instru(Instruction * instru);
void free_operand(Operand * operand);

#endif // __UTILS_H
