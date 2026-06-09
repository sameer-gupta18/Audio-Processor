#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define INIT_CAPACITY 32 
#define GROWTH_RATE 2 
#define LOAD_FACTOR 0.75
typedef struct Node {
    char* label;
    uint32_t address; 
    struct Node *next;
} Node; 

typedef struct{
    Node **buckets;
    int capacity; 
    int size; 
} Sym_Table; 

bool insert_address(Sym_Table* sym_table, char* label, uint32_t address);
bool search_label(Sym_Table* sym_table, char* label, uint64_t*out);
Sym_Table* symtable_init(void);
void symtable_free(Sym_Table* st);

#endif
