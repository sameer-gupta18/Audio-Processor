#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <stdint.h>

typedef struct{
    uint8_t temp;
} Sym_Table; 

int insert_address(char* label, uint32_t address);
uint32_t search_label(char* label);


#endif
