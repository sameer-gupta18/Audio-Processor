#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handle immediate instructions

int handle_immediate(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    DP_Immediate instruction;
    uint8_t opi, opc;

    //setting opcode using fall through cases

    switch(instr[i].mnemonic){
        case ADD:
        case MOVN:
            opc = 0;
            break;
        case ADDS:
            opc = 1;
            break;
        case SUB:
        case MOVZ:
            opc = 2;
            break;
        case SUBS:
        case MOVK:
            opc = 3;
            break;
        default:
            fprintf(stderr, "Invalid mnemonic for assembling");
            return ENCODE_FAIL;
    }

    //setting the operand and opi fields

    switch(instr[i].mnemonic){
        case ADD:
        case ADDS:
        case SUB:
        case SUBS:
            opi = 2;
            Immediate_Arithmetic arith_operand = {
                instr[i].num_fields == NUM_FIELDS && instr[i].f3.field_data.shift.shift_amount == SHIFT_AMT,
                instr[i].f2.field_data.immediate,
                instr[i].f1.field_data.reg.index
            };
            instruction.operand.immediate_arithmetic = arith_operand;
            break;
        case MOVN:
        case MOVZ:
        case MOVK:
            opi = 5;
            Immediate_Wide_Move wm_operand = {
                instr[i].f2.field_data.shift.shift_amount >> NUM_FIELDS,
                instr[i].f1.field_data.immediate
            };
            instruction.operand.immediate_widemove = wm_operand;
            break;
        default:
            fprintf(stderr, "Invalid mnemonic for assembling");
            return ENCODE_FAIL;
    }

    instruction.opc = opc;
    instruction.opi = opi;

    //setting common fields

    instruction.sf = instr[i].f0.field_data.reg.sf;
    instruction.rd = instr[i].f0.field_data.reg.index;

    //setting the output using the output pointer

    Assembled_Instruction assembled_instr = {DPI,.instruction_data.dpi = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}