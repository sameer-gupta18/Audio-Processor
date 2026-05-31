#ifndef ENCODE_H
#define ENCODE_H
#include "assembleIR.h"
#include "symtable.h"
#include "parserIR.h"

int encoder(
    Sym_Table *sym_table, 
    Parser_Instruction* parserIR, 
    Assembled_Instruction* assembled_list // points to the cuurrent empty slot (not basal)
);

#endif
