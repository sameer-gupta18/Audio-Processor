#include "../processor.h"
#include "decode.h"
#include "../execute/execute.h"

#define HALT 0x8a000000
#define SIMM_19_MASK 0x40000

typedef enum{
    DPI_Immediate = 4,
    DPI_Register = 5, 
    BRANCH = 5
    } SWITCH;

// Fetching instruction at current PC
uint32_t instruction_fetch(CPU_state *state) {
  // Checking whether PC is out of bounds. 
  if (state->pc + 4 > MEMORY_SIZE) {
    fprintf(stderr, "Address 0x%08lx is out of bounds\n", state->pc);
    exit(EXIT_FAILURE);
  }
  return load32(state->memory, state->pc);
}

// N.b. instruction_decode does not modify state
int instruction_decode(CPU_state *state, uint32_t instruction){
    if (instruction == HALT){
       return DECODE_HALT; 
    }
    else if ( // Matching with op0 = 100x
         mask_instr_bits(instruction, 28 , 26 ) == DPI_Immediate 
        )
    {  
        uint8_t sf = mask_instr_bits(instruction, 31 , 31 );
        uint8_t opc = mask_instr_bits(instruction, 30 , 29 );
        uint8_t opi = mask_instr_bits(instruction, 25 , 23 );
        uint32_t operand = mask_instr_bits(instruction, 22, 5);
        uint8_t rd = mask_instr_bits(instruction, 4 , 0 );
        return data_processing_immediate(state,sf , opc, opi, operand, rd);
    } else if ( // Matching with op0 = x101
        mask_instr_bits(instruction,27, 25) == DPI_Register
    )
    { 
        uint8_t sf = mask_instr_bits(instruction, 31 , 31 );
        uint8_t opc = mask_instr_bits(instruction, 30 , 29 );
        uint8_t m = mask_instr_bits(instruction, 28, 28);
        uint8_t opr = mask_instr_bits(instruction, 24, 21);
        uint8_t rm = mask_instr_bits(instruction, 20,16);
        uint32_t operand = mask_instr_bits(instruction, 15, 10);
        uint8_t rn = mask_instr_bits(instruction, 9, 5);
        uint8_t rd = mask_instr_bits(instruction, 4, 0);
        return data_processing_register(state,sf , opc, m, rm,opr, operand, rn, rd);
    } else if( // Matching with 31 =1 & op0 = x1x0
        (mask_instr_bits(instruction, 31, 31)==1) && 
        (mask_instr_bits(instruction, 27, 27)==1) && 
        (mask_instr_bits(instruction, 25, 25)==0)
    )
    {
        uint8_t sf = mask_instr_bits(instruction, 30, 30);
        uint8_t u = mask_instr_bits(instruction, 24, 24);
        uint8_t l = mask_instr_bits(instruction, 22, 22);
        uint16_t offset = mask_instr_bits(instruction, 21, 10);;
        uint8_t xn = mask_instr_bits(instruction, 9, 5);;
        uint8_t rt = mask_instr_bits(instruction,4, 0);

        return single_data_transfer(state, sf, u, l, offset, xn, rt);
    } else if ( // Matching with 31 = 0 & op0 = x1x0
        (mask_instr_bits(instruction, 31, 31)==0) && 
        (mask_instr_bits(instruction, 27, 27)==1) && 
        (mask_instr_bits(instruction, 25, 25)==0)
    ){
        uint8_t sf = mask_instr_bits(instruction, 30, 30);
        uint32_t simm19_u = mask_instr_bits(instruction, 23, 5);
        // Must sign-extend simm19
        int64_t simm19 = sgx(simm19_u, SIMM_19_MASK); 
        uint8_t rt = mask_instr_bits(instruction, 4, 0);
        return load_literal(state,sf, simm19, rt);
    } else if( // Matching with op0 = 101x
        (mask_instr_bits(instruction,28, 26)==BRANCH) 
    )
    {
        uint32_t operand = mask_instr_bits(instruction, 25, 0);
        uint8_t type = mask_instr_bits(instruction, 31, 30);
        // returns 1 if branched
        return branch(state, type, operand);
    }
        
    fprintf(stderr, "Cannot detect instruction 0x%08x\n", instruction);
    return DECODE_FAIL;
} 
