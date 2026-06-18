#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"


//literal handling helper function with symbol table reference
int64_t handle_literal(Address_Literal address, Sym_Table* sym_table){
    int64_t addr; 
    if (address.literal_kind == LIT_ADDR){  //address case
        addr = (int64_t)address.data.int_address;
    }
    else{   //label case
        uint64_t found_addr;
        if (!search_label(sym_table,address.data.label,&found_addr)){
            fprintf(stderr, "Label %s is not found\n", address.data.label);
            return ENCODE_FAIL; 
        }
        addr = (int64_t)found_addr;
    }
    return addr;
}

//calculating the branch offset

int64_t branch_offset(int64_t target_addr, int64_t curr_addr) {
    int64_t byte_offset = target_addr - curr_addr;
    if (byte_offset % INSTRUCTION_BYTES != 0) {
        fprintf(stderr, "Branch target is not 4-byte aligned\n");
        exit(1);
    }
    return byte_offset / INSTRUCTION_BYTES;  
}