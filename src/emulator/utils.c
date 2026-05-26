#include <stdint.h>
#include "utils.h"

extern uint32_t mask_instr_bits(uint32_t instruction, uint8_t start, uint8_t end){
    uint8_t diff = start-end+1;
    uint32_t shifted = (uint32_t)(instruction >> end);
    uint32_t mask = (((uint32_t)1<<(diff))-1);
    uint32_t res = shifted & mask;
    return res;
}