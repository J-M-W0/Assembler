#include "header.h"
#include "utils.h"
#include "str.h"
#include "table.h"
#include "instru.h"

int main(int argc, const char * const argv[]) {
    init(argc, argv);
    process();
    codegen();
    bingen();
    quit();
    return 0;
}
