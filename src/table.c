#include "table.h"
#include "header.h"

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
    assertf(table != nil, "Table is (null) @Table_Contain");
    assertf(name != nil, "name is (null) @Table_Contain");
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
    assertf(table != nil, "Table is (null) @Table_Get");
    assertf(name != nil, "name is (null) @Table_Get");
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
    assertf(Table_Contain(table, name), "Error: %s is not inside table!", name);
    TNode * node = malloc(sizeof(TNode));
    assertf(node != nil, "Error @Table_Set with malloc");
    node->name = strdup(name);
    node->addr = addr;
    node->next = table->head;
    table->head = node;
}
