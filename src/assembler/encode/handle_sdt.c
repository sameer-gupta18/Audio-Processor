#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handling single data transfers

int handle_sdt(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    Single_Data instruction;
    Addressing_Mode offset;
    instruction.sf = instr[i].f0.field_data.reg.sf;
    bool u = 0;

    //setting l bit

    switch(instr[i].mnemonic){
        case LDR:
            instruction.l = 1;
            break;
        case STR:
            instruction.l = 0;
            break;
        default:
            fprintf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;
    }

    //setting the offset according to address kind

    switch(instr[i].f1.field_data.address.kind){
        case UNSIGNED_OFFSET:
            u = 1;
            offset.mode = UNSIGNED;
            offset.data.imm12 = 
                instruction.sf ? 
                instr[i].f1.field_data.address.address_data.unsigned_offset.imm >> 3:
                instr[i].f1.field_data.address.address_data.unsigned_offset.imm >> 2;
            instruction.xn = instr[i].f1.field_data.address.address_data.unsigned_offset.xn.index;
            break;
        case PRE_INDEXED:
            offset.mode = INDEX;
            offset.data.index.i = 1;
            offset.data.index.simm9 = instr[i].f1.field_data.address.address_data.pre_post_index.simm;
            instruction.xn = instr[i].f1.field_data.address.address_data.pre_post_index.xn.index;
            break;
        case POST_INDEXED:
            offset.mode = INDEX;
            offset.data.index.i = 0;
            offset.data.index.simm9 = instr[i].f1.field_data.address.address_data.pre_post_index.simm;
            instruction.xn = instr[i].f1.field_data.address.address_data.pre_post_index.xn.index;
            break;
        case REGISTER_OFFSET:
            offset.mode = REG_OFFSET;
            offset.data.xm = instr[i].f1.field_data.address.address_data.register_offset.xm.index;
            instruction.xn = instr[i].f1.field_data.address.address_data.register_offset.xn.index;
            break;
        case LITERAL:
            fprintf(stderr, "Logical Error. Never Should have happened.");
            return ENCODE_FAIL;
    }

    //setting common fields

    instruction.offset = offset;
    instruction.u = u;

    instruction.rt = instr[i].f0.field_data.reg.index;

    //setting instruction to output

    Assembled_Instruction assembled_instr = {SDT, .instruction_data.sdt=instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}