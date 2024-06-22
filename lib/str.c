#include "str.h"

// prepend @param {s} to the front of @param {str}
char * strpre(char * str, const char * const s) {
    if (!s) {
        return str;
    }
    if (!str) {
        return strdup(s);
    }

    int len = strlen(str) + strlen(s) + 1;
    char * newstr = (char *) malloc(sizeof(char) * len);
    assert(newstr != nil);
    strcpy(newstr, s);
    strcat(newstr, str);
    free(str);
    return newstr;
}

// append @param {s} to the rear of @param {str}
char * strapp(char * str, const char * const s) {
    if (!s) {
        return str;
    }
    if (!str) {
        return strdup(s);
    }

    int len = strlen(str) + strlen(s) + 1;
    char * newstr = (char *) malloc(sizeof(char) * len);
    assert(newstr != nil);
    strcpy(newstr, str);
    strcat(newstr, s);
    free(str);
    return newstr;
}
// locate @param {s} inside the @param {str}
int strloc(const char * str, const char * s) {
    if (!str || !s) {
        return -1;
    }
    int index = -1;
    int lenstr = strlen(str);
    int lens = strlen(s);
    for (int i = 0; i < lenstr; i++) {
        bool found = true;
        for (int j = 0; j < lens; j++) {
            if (str[i + j] != s[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return index;
}
// convert string format to integer, 
// automatically detects if @param {str} has '0x', '0b' prefix or not.
int strtoi(const char * str) {
    int imm;
    if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0) {
        assert(sscanf(str, "%x", &imm) == 1);
    }
    else if (strncmp(str, "0b", 2) == 0 || strncmp(str, "0B", 2) == 0) {
        imm = 0;
        // @local {binstr} skips the '0b', '0B' prefix...
        const char * binstr = str + 2;
        while (*binstr != '\0') {
            imm <<= 1;
            if (*binstr == '1') {
                imm += 1;
            }
            else if (*binstr == '0') {
                imm += 0;
            }
            else {
                abortf("Error @strtoi while converting binary stirng '%s' into numeric value!", str);
            }
            binstr++;
        }
    }
    else if (str[0] == '0' && str[1] != '\0') {
        assert(sscanf(str, "%o", &imm) == 1);
    }
    else {
        assert(sscanf(str, "%d", &imm) == 1);
    }
    return imm;
}

static char * dec2bin_16bit(int n) {
    // make sure that @param {n} is maximal 16-bit large.
    n &= 0xffff;
    char * binstr = (char *) malloc(20);
    assert(binstr != nil);
    memset(binstr, '0', 20);
    binstr[19] = '\0';
    binstr[4] = ' ';
    binstr[9] = ' ';
    binstr[14] = ' ';

    int index = 3;
    while (n != 0) {
        binstr[index--] = (n & 1) ? '1' : '0';
        n >>= 1;

        /*
        if (index == -1) {
            index = 8;
        }
        else if (index == 4) {
            index = 13;
        }
        else if (index == 9) {
            index = 18;
        }
        else if (index == 14) {
            break;
        }
        */

        if (index == -1 || index == 4 || index == 9) {
            index += 9;
        }
    }
    return binstr;
}
static char * dec2bin_8bit(int n) {
    // make sure that @param {n} is maximal 8-bit large.
    n &= 0xff;
    char * binstr = (char *) malloc(10);
    assert(binstr != nil);
    memset(binstr, '0', 10);
    binstr[9] = '\0';
    binstr[4] = ' ';

    int index = 3;
    while (n != 0) {
        binstr[index--] = (n & 1) ? '1' : '0';
        n >>= 1;
        if (index == -1) {
            index = 8;
        }
    }
    return binstr;
}
static char * dec2bin_4bit(int n) {
    // make sure that @param {n} is maximal 4-bit large.
    n &= 0xf;
    char * binstr = (char *) malloc(5);
    assert(binstr != nil);
    memset(binstr, '0', 5);
    binstr[4] = '\0';

    int index = 3;
    while (n != 0 && index >= 0) {
        binstr[index--] = (n & 1) ? '1' : '0';
        n >>= 1;
    }
    return binstr;
}

char * itostr(int n, int bit) {
    if (bit == -2) {
        int num_digits = snprintf(nil, 0, "%x", n);
        char * str = malloc(sizeof(char) * (num_digits + 1));
        assert(str != nil);
        sprintf(str, "%x", n);
        return str;
    }
    else if (bit == -1) {
        int num_digits = snprintf(nil, 0, "0x%x", n);
        char * str = malloc(sizeof(char) * (2 + num_digits + 1));
        assert(str != nil);
        sprintf(str, "0x%x", n);
        return str;
    }
    else if (bit == 0) {
        int num_digits = snprintf(nil, 0, "%d", n);
        char * str = malloc(sizeof(char) * (num_digits + 1));
        assert(str != nil);
        sprintf(str, "%d", n);
        return str;
    } 
    else if (bit == 4) {
        return dec2bin_4bit(n);
    }
    else if (bit == 8) {
        return dec2bin_8bit(n);
    }
    else if (bit == 16) {
        return dec2bin_16bit(n);
    }
    else {
        abortf("Error @itostr: "
        "can not be converting %d to %d bit binary string!",
        n, bit);
        exit(1);
    }
}

