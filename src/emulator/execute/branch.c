#include <stdint.h>
#include <stdio.h>
#include "execute.h"
#include "../utils.h"
#include "execute_utils.h"
#define SIMM_26_MASK 0x3FFFFFF
#define SIMM_19_MASK 0x3FFFF
#define OFFSET_MUL 4
#define BRANCH_OK 0
#define BRANCH_FAIL -1 
#define ADD_PC state->pc = state->pc + simm19*4

typedef enum {EQ=1, NE = 2, GE = 10, LT = 11, GT = 12, LE = 13, AL = 14} encoding;

static int unconditional_jump(CPU_state* state, uint32_t operand){
    int64_t simm = sgx(operand, SIMM_26_MASK);
    state->pc = state->pc + simm * OFFSET_MUL;
    return BRANCH_OK;
}


static int register_jump(CPU_state* state, uint8_t xn){
    if (xn == XZR){
        return BRANCH_OK;
    }
    state->pc = state->memory->data[xn];
    return BRANCH_OK;
}

static int cond_jump(CPU_state* state, uint8_t cond, int64_t simm19){
    switch (cond) {
        case EQ: // EQ
            if (state->pstate.Z){
                ADD_PC;
            }
            return BRANCH_OK;
        case NE:
            if(!state->pstate.Z){
                ADD_PC;
            }
            return BRANCH_OK;
        case GE:
            if(state->pstate.N == state->pstate.V){
                ADD_PC;
            }
            return BRANCH_OK;
        case LT:
            if(state->pstate.N == state->pstate.V){
                ADD_PC;
            }
            return BRANCH_OK;
        case GT:
            if(state->pstate.Z == 0 && state->pstate.N == state->pstate.V){
                ADD_PC;
            }
            return BRANCH_OK;
        case LE:
            if(!(state->pstate.Z == 0 && state->pstate.N == state->pstate.V)){
                ADD_PC;
            }
            return BRANCH_OK;
        case AL:
            ADD_PC;
            return BRANCH_OK;
        default:
            fprintf(stderr, "Could not decode cond %d in BRANCH",cond);
            return BRANCH_FAIL;
    }


}
extern int branch(CPU_state* state, uint8_t type, uint32_t operand){
    uint8_t xn;
    uint8_t cond;
    int64_t simm19;

    switch(type){
        case 0: 
            return unconditional_jump(state, operand);
            break;
        case 1:
            xn = mask_instr_bits(operand, 9, 5);
            return register_jump(state, xn);
        case 2:
            cond = mask_instr_bits(operand, 3, 0);
            simm19 = sgx(mask_instr_bits(operand, 23, 5), SIMM_19_MASK);
            return cond_jump(state, cond, simm19);
        default:
            fprintf(stderr,"Error branching with type %d and operand %d", type, operand);
            return -1; 
    }
}
