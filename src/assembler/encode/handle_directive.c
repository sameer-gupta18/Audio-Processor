#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handle the directive instructions and store on the output stream                

int handle_directives(
    Parser_Instruction* instrs,
    size_t i, 
    Assembled_Instruction* output,
    size_t curr_idx
){
    if (instrs[i].num_fields != 1 || instrs[i].f0.field != IMMEDIATE){
        fprintf(stderr, "Invalid Directive. Follow the format: '.int <simm>'");
        return ENCODE_FAIL;
    }

    //set instruction to just the immediate value

    Special_Instruction out = {
        instrs[i].f0.field_data.immediate
    };
    Assembled_Instruction assembled_instr = {SPECIAL,.instruction_data.value=out};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}