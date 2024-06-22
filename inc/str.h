#ifndef __STR_H
#define __STR_H
#include "header.h"

// prepend @param {s} to the front of @param {str}
// NOTE: @param {str} must a from heap allocated string!!
char * strpre(char * str, const char * const s);
// append @param {s} to the rear of @param {str}
// NOTE: @param {str} must a from heap allocated string!!
char * strapp(char * str, const char * const s);
// locate @param {s} inside the @param {str}
int strloc(const char * str, const char * s);
// convert string format to integer, 
// automatically detects if @param {str} has '0x', '0b' prefix or not.
int strtoi(const char * str);
// convert integer to binary string format,
// NOTE: return value must be manually freed!!
char * itostr(int n, int bit);

#endif // __STR_H
