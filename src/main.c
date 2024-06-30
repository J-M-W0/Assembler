#include "rasm.h"
#include "instru.h"
#include "codegen.h"
#include "assemble.h"

int main(int argc, const char * argv[]) {
    Init(argc, argv);
    Process();
    Hexgen();
    Codegen();
    Fin();
    return 0;
}


