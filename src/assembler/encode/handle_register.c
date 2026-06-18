#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"

//handling register instructions

int handle_register(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    

    uint8_t opc;
    Register_Operation opr;
    DP_Register instruction;

    bool n = 0;

    //setting opr, m, and arithemtic operands of the instruction
    
    switch(instr[i].mnemonic){
        case ADD:
        case ADDS:
        case SUB:
        case SUBS:
            instruction.M = 0;
            opr.first_bit = 1;
            opr.shift = instr[i].f3.field_data.shift.kind;
            instruction.operand.arith_logic_operand = instr[i].f3.field_data.shift.shift_amount;
            break;
        case AND:
        case BIC:
        case ORR:
        case ORN:
        case EOR:
        case EON:
        case ANDS:
        case BICS:
            instruction.M = 0;
            opr.first_bit = 0;
            opr.shift = instr[i].f3.field_data.shift.kind;
            instruction.operand.arith_logic_operand = instr[i].f3.field_data.shift.shift_amount;
            break;
        case MADD:
        case MSUB:
            instruction.M = 1;
            opr.first_bit = 1;
            opr.shift = 0;
            opr.n = 0;
            break;
        default:
            fprintf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;   
    }

    //setting n, opc, and multiply operands

    switch(instr[i].mnemonic){
        case BIC:
            n = 1;
        case ADD:
        case AND:
            opc = 0;
            break;
        case ORN:
            n = 1;
        case ADDS:
        case ORR:
            opc = 1;
            break;
        case EON:
            n = 1;
        case SUB:
        case EOR:
            opc = 2;
            break;
        case BICS:
            n = 1;
        case ANDS:
        case SUBS:
            opc = 3;
            break;
        case MSUB:
            opc = 0;
            instruction.operand.multiply_operand.x = 1;
            instruction.operand.multiply_operand.ra = instr[i].f3.field_data.reg.index;
            break;
        case MADD:
            opc = 0;
            instruction.operand.multiply_operand.x = 0;
            instruction.operand.multiply_operand.ra = instr[i].f3.field_data.reg.index;
            break;
        default:
            fprintf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;   
    }

    //setting universal register fields

    instruction.opc = opc;
    opr.n = n;
    instruction.opr = opr;
    instruction.rm = instr[i].f2.field_data.reg.index;
    instruction.rn = instr[i].f1.field_data.reg.index;
    instruction.rd = instr[i].f0.field_data.reg.index;
    instruction.sf = instr[i].f0.field_data.reg.sf;

    //setting the instruction to the output

    Assembled_Instruction assembled_instr = {DPR, .instruction_data.dpr=instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;

}