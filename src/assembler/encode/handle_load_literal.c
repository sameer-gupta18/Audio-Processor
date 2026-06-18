#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handle the loading of literals

int handle_load_literal(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
){
    Load_Literal instruction;
    uint64_t addr = handle_literal(instr[i].f1.field_data.address.address_data.literal,sym_table);  
    int64_t offset = (int64_t)addr - (int64_t)instr[i].address;

    if (addr % INSTRUCTION_BYTES == 0 && offset < MB && offset > -MB){  //checking address validity
        instruction.simm19 = offset >> 2;
    } else{
        fprintf(stderr, "Invalid Offset");
        return ENCODE_FAIL;
    }

    //setting fields

    instruction.sf = instr[i].f0.field_data.reg.sf;
    instruction.rt = instr[i].f0.field_data.reg.index;

    //storing instruction in output

    Assembled_Instruction assembled_instr = {LOAD,.instruction_data.load_literal = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}