#include "header.h"
#include "utils.h"
#include "str.h"

// input  file name
char * in = nil;
// output file name
char * out = nil;

void parse_input(int argc, const char * const argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            assertf(i + 1 < argc, "Failed to parse input file name!");
            in = strdup(argv[i+1]);
            assertf(strloc(in, ".as") != -1, "%s is not a valid input file name, expected with .as extension!", in);
            return;
        }
    }
    abortf("Failed to parse input file name");
}
void parse_output(int argc, const char * const argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            assertf(i + 1 < argc, "Failed to parse output file name!");
            out = strdup(argv[i+1]);
            return;
        }
    }
    out = strdup("a.out");
}

int main(int argc, const char * const argv[]) {
    parse_input(argc, argv);
    parse_output(argc, argv);

    if (in) {
        free(in);
        in = nil;
    }
    if (out) {
        free(out);
        out = nil;
    }
    return 0;
}
