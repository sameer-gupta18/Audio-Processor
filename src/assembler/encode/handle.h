#ifndef HANDLE_H
#define HANDLE_H
#include "../assembleIR.h"
#include "../symtable.h"
#include "../parserIR.h"
#include "../types.h"

#define SHIFT_AMT 12

int64_t handle_literal(Address_Literal address, Sym_Table* sym_table);
int64_t branch_offset(int64_t target_addr, int64_t curr_addr);

extern int handle_directives(
    Parser_Instruction* instrs,
    size_t i, 
    Assembled_Instruction* output,
    size_t curr_idx
);

int handle_immediate(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
);

int handle_register(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
);

int handle_load_literal(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
);

int handle_sdt(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
);

int handle_branch(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
);

#endif