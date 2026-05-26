#include "memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "utils.h"

//Define update flags
int update_flags(CPU_state* state, uint64_t val1, uint64_t val2, uint64_t res, uint8_t sf, bool is_add) {
    //check if regsiters are 32 or 64 bit
    //Mask is either 32 or 64 bit. 
    //Bitwise & with result will ignore the last 32 bits if registers are set to 32
    uint64_t mask = sf ? 0xFFFFFFFFFFFFFFFFULL : 0xFFFFFFFFULL;
    res &= mask;

    uint64_t check_sign = sf ? (1ULL << 63) : (1ULL << 31);
    
    //Set N flag to signed bit of result
    state->pstate.N = (res & check_sign) != 0;

    //Set Z flag to 1 if res == 0
    state->pstate.Z = res == 0;

    //Set C flag
    if (is_add) {
        //If there is an overflow, the set C flag to 1
        state->pstate.C = (val1 + val2) < val1;
    } else {
        //If there is an underflow, set C flag to 0
        state->pstate.C = val1 >= val2;
    }

    //Set V flag to 1 if there is signed overflow
    //Initialise signed variables for val1, 2 and result
    int64_t s1 = (int64_t)val1;
    int64_t s2 = (int64_t)val2;
    int64_t sr = (int64_t)res;

    if (is_add) {
        state->pstate.V =
        //If both arguments are positive but result is negative
        ((s1 > 0 && s2 > 0 && sr < 0) ||
        //If both arguments are negative, but result is positive
        (s1 < 0 && s2 < 0 && sr > 0));
    } else {
        //If both s1 is positive, s2 is negative but result is negative
        ((s1 > 0 && s2 < 0 && sr < 0) ||
        //If both s1 is negative, s2 is positive but result is positive
        (s1 < 0 && s2 > 0 && sr > 0));
    }
    return 0;
}

//Define arithmetic instructions
int dpi_arithmetic(CPU_state* state, uint8_t sf, uint8_t opc, uint32_t operand, uint8_t rd) {

    uint8_t sh = mask_instr_bits(operand, 18, 18);
    uint16_t imm12 = mask_instr_bits(operand, 17, 6);
    uint8_t rn = mask_instr_bits(operand, 5, 0);
    uint32_t op2 = imm12;

    //If rd or rn = 31, encode stack pointer
    if (rd == 31 || rn == 31) {
        //Implement stack
        return 0;
    }

    //If sh = 1, shift left by 12
    if (sh) {
        op2 <<= 12;
    }

    uint64_t rn_val = state->registers[rn];
    uint64_t result = 0;

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
        exit(EXIT_FAILURE);
        break;
    }

    state->registers[rd] = result;
    return 0;
}

//Define wide move
int dpi_wide_move(CPU_state* state, uint8_t sf, uint8_t opc, uint32_t operand, uint8_t rd) {
    uint8_t hw = mask_instr_bits(operand, 17, 16);
    uint16_t imm16 = mask_instr_bits(operand, 15, 0);

    uint64_t op = (uint64_t)imm16 << (hw * 16);
    uint64_t result = 0;

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
        result = (old_val & ~(0xFFFFULL << (hw * 16))) | op;
        break;

    default:
        fprintf(stderr, "Invalid wide move operation");
        exit(EXIT_FAILURE);
        break;
    }

    state->registers[rd] = result;
    return 0;
}


//Define data processing instructions (immediate)
int data_processing_immediate(CPU_state* state, uint8_t sf, uint8_t opc, uint8_t opi, uint32_t operand, uint8_t rd) {
    //If opi = 010, call arithmetic function
    if (opi == 2) {
        dpi_arithmetic(state, sf, opc, operand, rd);
    }
    //If opi = 101, call wide move function
    else if (opi == 5) {
        dpi_wide_move(state, sf, opc, operand, rd);
    }
    else {
        fprintf(stderr, "Instruction is invalid");
        exit(EXIT_FAILURE);
    }
    return 0;
}

