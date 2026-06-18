#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handling branch instructions

int handle_branch(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
){
    Branch_Instruction instruction;

    //dealing with the different types of branches

    switch(instr[i].mnemonic){
        case B:
            instruction.mode = UNCOND;
            instruction.data.simm26 =
            branch_offset(
                handle_literal(instr[i].f0.field_data.address.address_data.literal, sym_table),
                (int64_t) instr[i].address
            );
            break;
        case BR:
            instruction.mode = REG_BRANCH;
            instruction.data.xn = instr[i].f0.field_data.reg.index;
            break;
        case BCOND:
            instruction.mode = COND;
            instruction.data.conditional.cond = instr[i].cond;
            instruction.data.conditional.simm19 =
            branch_offset(
                handle_literal(instr[i].f0.field_data.address.address_data.literal, sym_table),
                (int64_t) instr[i].address
            );
            break;
        default:
            fprintf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;
    }

    //setting the instruction to the output

    Assembled_Instruction assembled_instr = {BRANCH,.instruction_data.branch = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}