#include <stdio.h>
#include "execute.h"
#include "../utils.h"
#include "execute_utils.h"


//Define arithmetic instructions
int dpi_arithmetic(CPU_state* state, uint8_t sf, uint8_t opc, uint32_t operand, uint8_t rd) {

    uint8_t sh = mask_instr_bits(operand, 18, 18);
    uint16_t imm12 = mask_instr_bits(operand, 17, 6);
    uint8_t rn = mask_instr_bits(operand, 5, 0);
    uint32_t op2 = imm12;

    //If rd or rn = 31, encode stack pointer
    if (rd == XZR || rn == XZR) {
        //Implement stack
        return DECODE_OK;
    }

    //If sh = 1, shift left by 12
    if (sh) {
        op2 <<= 12;
    }

    uint64_t rn_val = state->registers[rn];
    uint64_t result;

    switch (opc)
    {
    //Add
    case 0:
        result = rn_val + op2;
        break;
    
    //Add with flags    
    case 1:
        result = rn_val + op2;
        //Set flags
        update_flags(state, rn_val, op2, result, sf, true);
        break;

    //Subtract
    case 2:
        result = rn_val - op2;
        break;

    //Subtract with flags
    case 3:
        result = rn_val - op2;
        //Set flags
        update_flags(state, rn_val, op2, result, sf, false);
        break;

    default:
        fprintf(stderr, "Invalid arithmetic operation");
        return DECODE_FAIL;
    }

    state->registers[rd] = sf?result:(uint32_t)result;
    return DECODE_OK;
}

//Define wide move
int dpi_wide_move(CPU_state* state, uint8_t sf, uint8_t opc, uint32_t operand, uint8_t rd) {
    uint8_t hw = mask_instr_bits(operand, 17, 16);
    uint16_t imm16 = mask_instr_bits(operand, 15, 0);

    uint64_t op = (uint64_t)imm16 << (hw * WREG_SIZE/2);
    uint64_t result;

    //if registers are 32 bit then mask
    if (!sf) {
        op &= 0xFFFFFFFF;
    }

    //Load the current value of rd (required for move keep)
    uint64_t old_val = state->registers[rd];

    switch (opc)
    {
    case 0:
        result = ~op;
        break;
    
    case 2:
        result = op;
        break;

    case 3:
        result = (old_val & ~(0xFFFFULL << (hw * WREG_SIZE/2))) | op;
        break;

    default:
        fprintf(stderr, "Invalid wide move operation");
        return DECODE_FAIL;
    }

    state->registers[rd] = result;
    return DECODE_OK;
}


//Define data processing instructions (immediate)
int data_processing_immediate(CPU_state* state, uint8_t sf, uint8_t opc, uint8_t opi, uint32_t operand, uint8_t rd) {
    //If opi = 010, call arithmetic function
    int result;
    if (opi == 2) {
        result = dpi_arithmetic(state, sf, opc, operand, rd);
    }
    //If opi = 101, call wide move function
    else if (opi == 5) {
        result = dpi_wide_move(state, sf, opc, operand, rd);
    }
    else {
        fprintf(stderr, "Instruction is invalid");
        return DECODE_FAIL;
    }
    return result;
}