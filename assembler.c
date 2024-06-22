#include "header.h"
#include "utils.h"
#include "str.h"
#include "table.h"
#include "instru.h"
#include <stdio.h>

extern char * in;
extern char * out;
extern TTable * LabelTable;
extern FILE * In;
extern FILE * Out;

int main(int argc, const char * const argv[]) {
    init(argc, argv);
    process();
    codegen();
    bingen();
    quit();
    return 0;
}
