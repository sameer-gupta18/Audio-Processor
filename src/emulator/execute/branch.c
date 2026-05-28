#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "execute.h"
#include "../utils.h"
#include "execute_utils.h"
#define SIMM_26_MASK 0x2000000
#define SIMM_19_MASK 0x40000
#define OFFSET_MUL 4
#define MAX_REG_SIZE 32

typedef enum {EQ=0, NE = 1, GE = 10, LT = 11, GT = 12, LE = 13, AL = 14} encoding;
typedef enum {UNCOND=0, REG= 3, COND= 1} type;

static int unconditional_jump(CPU_state* state, uint32_t operand){
    int64_t simm = sgx(operand, SIMM_26_MASK);
    state->pc = state->pc + simm * OFFSET_MUL;
    return DECODE_BRANCH;
}


static int register_jump(CPU_state* state, uint8_t xn){
    assert(xn < MAX_REG_SIZE);
    if (xn == XZR){
        return DECODE_OK;
    }
    state->pc = state->registers[xn];
    return DECODE_BRANCH;
}

static int cond_jump(CPU_state* state, uint8_t cond, int64_t simm19){
    bool flag;

    switch (cond) {
        case EQ: 
            flag = state->pstate.Z; 
            break;
        case NE: 
            flag = !state->pstate.Z;
            break;
        case GE:
            flag = state->pstate.N == state->pstate.V;
            break;
        case LT:
            flag = state->pstate.N != state->pstate.V;
            break;
        case GT:
            flag = state->pstate.Z == 0 && state->pstate.N == state->pstate.V;
            break;
        case LE:
            flag = !(state->pstate.Z == 0 && state->pstate.N == state->pstate.V);
            break;
        case AL:
            flag = true;
            break;
        default:
            fprintf(stderr, "Could not decode cond %d in BRANCH",cond);
            return DECODE_FAIL;
    }
    if(flag){
            state->pc += simm19*OFFSET_MUL;
            return DECODE_BRANCH;
        }

    return DECODE_OK;
}

extern int branch(CPU_state* state, uint8_t type, uint32_t operand){
    uint8_t xn;
    uint8_t cond;
    int64_t simm19;
    //Determining type of branch
    switch(type){
        case UNCOND: 
            return unconditional_jump(state, operand);
            break;
        case REG:
            xn = mask_instr_bits(operand, 9, 5);
            return register_jump(state, xn);
        case COND:
            cond = mask_instr_bits(operand, 3, 0);
            simm19 = sgx(mask_instr_bits(operand, 23, 5), SIMM_19_MASK);
            return cond_jump(state, cond, simm19);
        default:
            fprintf(stderr,"Error branching with type %d and operand %d", type, operand);
            return DECODE_FAIL; 
    }
}
