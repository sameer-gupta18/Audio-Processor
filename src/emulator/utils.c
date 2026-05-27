#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "utils.h"
#include "memory.h"


// Mask instruction bits
extern uint32_t mask_instr_bits(
    uint32_t instruction, 
    uint8_t start, 
    uint8_t end
){
    uint8_t diff = start-end+1;
    uint32_t shifted = (uint32_t)(instruction >> end);
    uint32_t mask = (((uint32_t)1<<(diff))-1);
    uint32_t res = shifted & mask;
    return res;
}

// Load 32bit word from main memory.
uint32_t load32(Sys_Memory *mem, uint64_t address) {
  // Checking for possible access errors.
  if (address + 4 > MEMORY_SIZE) {
    fprintf(stderr, "Address 0x%08lx is out of bounds\n", address);
    exit(EXIT_FAILURE);
  }
  // We access data according to little-endian ordering.
  uint32_t res = (uint32_t)mem->data[address] |
                 (uint32_t)mem->data[address + 1] << 8 |
                 (uint32_t)mem->data[address + 2] << 16 |
                 (uint32_t)mem->data[address + 3] << 24;

  return res;
}


extern int64_t sgx(uint64_t num, uint64_t mask){
    return (num ^ mask) - mask;
}
