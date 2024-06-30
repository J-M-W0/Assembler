#include "table.h"

extern u64 Section;
extern u64 Location;

TTable * Table_Init(void) {
    TTable * table = malloc(sizeof(TTable));
    assertf(table != nil, "Failed at malloc @func {Table_Init}");
    table->head = nil;
    return table;
}

static void TNode_Free(TNode * node) {
    if (!node) {
        return;
    }
    if (node->name) {
        free(node->name);
        node->name = nil;
    }
    TNode_Free(node->next);
    free(node);
}

void Table_Free(TTable * table) {
    if (!table) {
        return;
    }
    TNode_Free(table->head);
    free(table);
}

bool Table_Contain(TTable * table, const char * name) {
    assertf(table != nil);
    assertf(name != nil);
    

    if (strcmp(name, "$") == 0) {
        return true;
    }


    TNode * curr = table->head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return true;
        }
        curr = curr->next;
    }


    return false;
}

u64 Table_Get(TTable * table, const char * name) {
    assertf(table != nil);
    assertf(name != nil);


    if (strcmp(name, "$") == 0) {
        u64 address = (Section << 16) + Location / 2;
        return address;
    }


    TNode * curr = table->head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr->addr;
        }
        curr = curr->next;
    }


    abortf("Error: %s is not inside table!", name);
    exit(EXIT_FAILURE);
}

void Table_Set(TTable * table, const char * name, const u64 addr) {
    assertf(!Table_Contain(table, name), "Error: %s is already inside the table!", name);
    TNode * node = malloc(sizeof(TNode));
    assertf(node != nil);
    node->name = strdup(name);
    node->addr = addr;
    node->next = table->head;
    table->head = node;
}

void Table_Display(TTable * table) {
    if (!table) {
        return;
    }

    TNode * curr = table->head;
    fprintf(stdout, "\nTable Content:\n");
    while (curr) {
        fprintf(stdout, "\tname: %s, value: 0x%lx:0x%lx\n", 
        curr->name, 
        (curr->addr >> 16) & 0xff,
        (curr->addr & 0xffff) / 2);
        curr = curr->next;
    }
    fprintf(stdout, "\n\n");
}
