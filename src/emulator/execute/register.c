#include <stdio.h>
#include "execute.h"
#include "../utils.h"
#include "execute_utils.h"
#define WREG 32
#define ZR 0

typedef enum {LSL = 0, LSR = 1, ASR = 2, ROR = 3} shift_encodings;

int arithmetic_logic(
    CPU_state* state,
    bool sf, 
    uint8_t opc, 
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
){
    bool id_bit = mask_instr_bits(opr, 3,3);
    uint8_t shift = mask_instr_bits(opr,2,1);
    bool n = mask_instr_bits(opr,0,0);
    uint64_t rm_val = rm<WREG - 1?state->registers[rm]:ZR;
    uint64_t rn_val = rn<WREG - 1?state->registers[rn]:ZR;


    if (!sf && operand>=WREG){
        fprintf(stderr,"Operand out of range");
        return DECODE_FAIL;
    }


    //Shifting

    switch(shift){
        case LSL:
            rm_val <<= operand;
            break;
        case LSR:
            rm_val >>= operand;
            break;
        case ASR:
            rm_val = (int8_t)rm_val>>operand;
            break;
        case ROR:
            rm_val = (rm_val >> (operand % WREG)) | (rm_val >> (WREG-(operand % WREG)));
            break;
        default:
            fprintf(stderr, "Could not decode the shift operation");
            return DECODE_FAIL;
    }

    uint64_t op2 = rm_val;
    if (n){
        op2 = ~rm_val;
    }

    uint64_t result;


    if (id_bit && !n){ //Arithmetic Operation
        if (shift == ROR){
            fprintf(stderr, "Invalid Shift Operation (canno have rotate right with an arithmetic operation)");
            return DECODE_FAIL;
        }
        switch(opc){
            case 0:
                result = rn_val + op2;
                break;
            case 1:
                result = rn_val+op2;
                update_flags(state,rn_val,op2,result,sf,true);
            case 2:
                result = rn_val - op2;
                break;
            case 3:
                result = rn_val - op2;
                update_flags(state,rn_val,op2,result,sf,false);
                break;
            default:
                fprintf(stderr,"Invalid Arithmetic Operation");
                return DECODE_FAIL;
            }
    }
    else if(!id_bit){
        switch(opc){
            case 0:
                result = rn_val & op2;
                break;
            case 1:
                result = rn_val | op2;
                break;
            case 2:
                result = rn_val ^ op2;
                break;
            case 3:
                result = rn_val & op2;
                update_flags(state,rn_val,op2,result,sf,false);
                break;
            default:
                fprintf(stderr,"Invalid Logical Operation");
                return DECODE_FAIL;
        }
    } else{
        fprintf(stderr,"Invalid register operation");
        return DECODE_FAIL;
    }

    if (rd < WREG - 1) {   //Handles ZR case
        state->registers[rd] = sf?result:(uint32_t)result;
    }

    return DECODE_OK;

}


int multiply(
    CPU_state* state,
    bool sf, 
    uint8_t opc, 
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
){
    if (!(opc == 0 && opr == 0b1000)){
        fprintf(stderr, "Invalid instruction");
        return DECODE_FAIL;
    }
    bool x = mask_instr_bits(operand, 6,6);
    uint8_t ra = mask_instr_bits(operand, 5,0);

    uint64_t ra_val = ra < WREG - 1?state->registers[ra]:ZR;
    uint64_t rm_val = rm<WREG - 1?state->registers[rm]:ZR;
    uint64_t rn_val = rn<WREG - 1?state->registers[rn]:ZR;

    uint64_t result = 0;

    if (x){
        result = ra_val - (rn_val * rm_val);
    }
    else{
        result = ra_val + (rn_val * rm_val);
    }

    if (rd < WREG - 1){
        state -> registers[rd] = sf?result:(uint32_t)result;
    }


    return DECODE_OK; 
}

int data_processing_register(
    CPU_state* state,
    bool sf, 
    uint8_t opc, 
    bool m,
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
){
    int res;
    if(m){
        res = multiply(state,sf,opc,rm,opr,operand,rn,rd);
    } else{
        res = arithmetic_logic(state,sf,opc,rm,opr,operand,rn,rd);
    }
    return res;//TODO
}