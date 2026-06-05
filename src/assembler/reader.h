#ifndef READER_H
#define READER_H
#include "parserIR.h"
#include "symtable.h"
#include <stdint.h>
#include <stdio.h>


extern uint64_t num_lines(FILE* input);
extern int read_to_parse(
    FILE* input, 
    Parser_Instruction* parsed_list, 
    Sym_Table* sym_table,
    uint64_t num_lines
);

#endif
