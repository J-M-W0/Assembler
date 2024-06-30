#include "codegen.h"
#include "rasm.h"
#include "str.h"
#include "table.h"
#include "utils.h"
#include "instru.h"

extern TTable * Labels;
extern FILE * In;
extern FILE * Out;

extern char * ifile;
extern char * ofile;
extern char * hexfile;

extern Instruction * Instru;

extern u64 Section;
extern u64 Location;

extern TToken Token;
extern String Symbol;

void Init(int argc, const char * argv[]) {
    Labels = Table_Init();

    assertf(argc >= 2);
    if (argc == 2) {
        ifile = strdup(argv[1]);
        ofile = strdup("a.out");
    }
    else {
        int i = search(argv, "-o");
        assertf(i != -1 && (i+1) < argc, "Couldn't find the output file!");
        
        ifile = strdup(argv[1]);
        ofile = strdup(argv[i+1]);
    }


    int i = strloc(ifile, ".rasm");
    assertf(i != -1, "File extension of input file %s must be of .rasm", ifile);
    hexfile = strdup(ifile);
    hexfile = strapp(hexfile, ".hex");

    In = fopen(ifile, "r");
    Out = fopen(hexfile, "w");

    info("input file name: %s", ifile);
    info("output file name: %s", ofile);

    readin();
    scan();
}

void Hexgen(void) {

    Section = 0;
    Location = 0;

    instrulyze(Instru);
    instrufree(Instru);

    if (In) {
        assertf(fclose(In) == 0);
        In = nil;
    }
    if (Out) {
        assertf(fclose(Out) == 0);
        Out = nil;
    }
}
void Codegen(void) {
    FILE * aout = fopen(ofile ,"wb");
    FILE * atxt = fopen("a.txt", "w");
    assertf(aout != nil);
    assertf(atxt != nil);

    u16  * body = calloc(0xffffff, sizeof(u16));        // for a.out
    char * text = malloc(4 * 0xffffff * sizeof(char));  // for a.txt
    assertf(body != nil);
    assertf(text != nil);
    memset(text, '0', 4 * 0xffffff);

    In = fopen(hexfile, "r");
    assertf(In != nil);

    readin();
    scan();
    do {
        assertf(Token==TokenLParen, "Expecting ( but received %s", Symbol);
        
        scan();
        assertf(Token==TokenInteger);
        int outloc = strtoi(Symbol);
        int txtloc = outloc * 4;

        scan();
        assertf(Token==TokenRParen, "Expecting ) but received %s", Symbol);

        scan();
        while (Token==TokenInteger) {
            u16 hexvalue = 0;
            for (int i = 0; i < 4; i++) {
                assertf(Token==TokenInteger);
                Symbol = strpre(Symbol, "0b");

                char * hex = itostr(strtoi(Symbol), -2);
                assertf(hex != nil);
                assertf(is_hexdigit(hex[0]) && hex[1]=='\0');

                text[txtloc++] = hex[0];
                hexvalue += (hex2int(hex[0]) << (4 * i));

                free(hex);
                
                scan();
            }
            body[outloc++] = hexvalue;
        }
    } while (Token!=TokenEOF);
    assertf(fclose(In)==0, "Failed to close file %s", hexfile);
    In = nil;

    int written;
    written = fwrite(body, sizeof(u16), 0xffffff, aout);
    assertf(written == 0xffffff, "Failed to write file %s", ofile);
    
    fprintf(atxt, "v2.0 raw\n");
    for (int row = 0; row < 0xffffff / 16; row++) {
        fprintf(atxt, "# 0x%06x\n", row * 16);
        for (int i = 0; i < 16; i++) {
            for (int k = 0; k < 4; k++) {
                int loc = row * 64 + i * 4 + 3 - k;
                fprintf(atxt, "%c", text[loc]);
            }
            fprintf(atxt, " ");
        }
        fprintf(atxt, "\n");
    }

    assertf(fclose(aout) == 0, "Failed to close file %s", ofile);
    assertf(fclose(atxt) == 0, "Failed to close file a.txt");
}

void Fin(void) {
    assertf(remove(hexfile) == 0);
    if (In) {
        assertf(fclose(In) == 0);
        In = nil;
    }
    if (Out) {
        assertf(fclose(Out) == 0);
        Out = nil;
    }
    Free(ifile);
    Free(ofile);
    Free(hexfile);
    tokenfree();
    Table_Free(Labels);
}

