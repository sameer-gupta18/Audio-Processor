#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../utils.h"
#include "execute_utils.h"
#define LOAD 1

uint64_t transfer_uo(CPU_state* state, uint8_t sf, uint16_t offset, uint8_t xn) {
    
    //offset depends on the width
    uint64_t uoffset = offset * (sf ? 8 : 4);
    uint64_t xn_val = state->registers[xn];

    return xn_val + uoffset;
}

uint64_t register_offset(CPU_state* state, uint8_t sf, uint16_t xm, uint8_t xn) {

    uint64_t xn_val = state->registers[xn];
    uint64_t xm_val = state->registers[xm];

    return xn_val + xm_val;
}

uint64_t indexed(CPU_state* state, uint8_t sf, uint16_t simm9, uint8_t xn, uint8_t i) {

    uint64_t xn_val = state->registers[xn];
    

    if (i) {
        //pre-indexed
        xn_val += simm9;
        state->registers[xn] = xn_val;
        return xn_val;
    } else {
        //post-indexed
        uint64_t old = xn_val;
        state->registers[xn] = xn_val + simm9;
        return old;
    }
}

int load_store(CPU_state* state, uint8_t l, uint64_t address, uint8_t rt, uint8_t sf) {
    if (l) {
        //load
        uint64_t value = 0;

        //byte size depends on width
        int size = sf ? 8 : 4;

        //read the value from memory 1 byte at a time
        for (int i = 0; i < size; i++) {
            //if address exceeds memory, exit
            if (address + i >= MEMORY_SIZE) exit;

            value |= ((uint64_t)state->memory->data[address + i]) << (8 * i);
        }

        state->registers[rt] = value;

    } else {
        //store
        uint64_t value = state->registers[rt];
        int size = sf ? 8 : 4;

        //write to memory the value stores, 1 byte at a time
        for (int i = 0; i < size; i++) {
            if (address + i >= MEMORY_SIZE) exit;

            state->memory->data[address + i] = (value >> (8 * i)) & 0xFF;
        }
    }

    return 0;
}


int single_data_transfer(CPU_state* state, uint8_t sf, uint8_t u, uint8_t l, uint16_t offset, uint8_t xn, uint8_t rt) {
    
    uint64_t transfer_address = 0;
    
    //Determine the addressing mode based on u and offset 
    if (u == 1) {
        //unisgned immediate offset
        transfer_address = transfer_uo(state, sf, offset, xn);
    } 
    
    else {
        uint8_t off1 = mask_instr_bits(offset, 11, 11);

        if (off1 == 1) {
            //register offset
            uint8_t xm = mask_instr_bits(offset, 10, 6);
            transfer_address = register_offset(state, sf, xm, xn);
        }

        else {
            //indexed
            uint8_t i = mask_instr_bits(offset, 1, 1);
            int16_t simm9 = (int16_t)(mask_instr_bits(offset, 10, 2) << 7) >> 7;

            transfer_address = indexed(state, sf, simm9, xn, i);
        }
    }

    load_store(state, l, transfer_address, rt, sf);
    return EXIT_SUCCESS;
}

int load_literal(CPU_state* state, uint8_t sf, uint32_t simm19, uint8_t rt) {

    //calculate address based on current pc and simm19
    int64_t offset = (int32_t)simm19 * 4;
    uint64_t addr = (uint64_t)(offset + state->pc);

    //Pass LOAD cos literals only works with load
    load_store(state, LOAD, addr, rt, sf);

    return EXIT_SUCCESS;
}