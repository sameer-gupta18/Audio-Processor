#ifndef ENCODE_H
#define ENCODE_H
#include "assembleIR.h"
#include "symtable.h"
#include "parserIR.h"

typedef enum {
    ENCODE_OK = 0,
    ENCODE_FAIL = 1
} ENCODE_STATUS;

// int first_pass(
//     Sym_Table* sym_table,
//     Parser_Instruction* instruction_list,
//     size_t list_length
// );

extern int encode(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output,
    size_t* curr_idx
);

#endif
