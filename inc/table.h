#ifndef __RASM_TABLE_H
#define __RASM_TABLE_H
#include "rasm.h"

typedef struct TNode {
    char * name;
    u64    addr;
    struct TNode * next;
} TNode;

typedef struct TTable {
    TNode * head;
} TTable;

TTable * Table_Init(void);
void Table_Free(TTable * table);
bool Table_Contain(TTable * table, const char * name);
u64 Table_Get(TTable * table, const char * name);
void Table_Set(TTable * table, const char * name, const u64 addr);
void Table_Display(TTable * table);

#endif // __RASM_TABLE_H
