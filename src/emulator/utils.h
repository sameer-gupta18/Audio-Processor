#include <stdint.h>
#include "memory.h"
// Select parts of the instruction register
extern uint32_t mask_instr_bits(uint32_t instruction, uint8_t start, uint8_t end);

extern uint32_t load32(Sys_Memory *mem, uint64_t address);
