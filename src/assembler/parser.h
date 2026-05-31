#ifndef PARSER_H
#define PARSER_H
#include "parserIR.h"
#include "symtable.h"
#include <stdlib.h>

// parses raw assembly into Parser IR form and stores in the "parsed_list"
int parser(
    char* assembly_instruction, 
    Sym_Table *symbol_table, 
    Parser_Instruction* parsed_list
);



#endif
