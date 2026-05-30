#ifndef ENCODE_H
#define ENCODE_H
#include "bitwiseIR.h"
#include "symtable.h"
#include "parserIR.h"

Bitwise_Instruction encoder(Sym_Table *sym_table, Parser_Instruction* parserIR);

#endif
