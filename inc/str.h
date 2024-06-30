#ifndef __RASM_STRING_H
#define __RASM_STRING_H
#include "rasm.h"

// To prepend @param {s} to the front of @param {str}
// NOTE: @param {str} must a from heap allocated string!!
char * strpre(char * str, const char * const s);
// To append @param {s} to the rear of @param {str}
// NOTE: @param {str} must a from heap allocated string!!
char * strapp(char * str, const char * const s);
// To locate @param {s} inside the @param {str}
int strloc(const char * str, const char * s);
// To convert string format to integer, 
// automatically detects if @param {str} has '0x', '0b' prefix or not.
int strtoi(const char * str);
// To convert integer to binary string format,
// NOTE: return value must be manually freed!!
char * itostr(int n, int bit);
// To reverse the @param {str}
void strrev(char * str);

#endif // __RASM_STRING_H
