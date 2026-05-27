#include <stdint.h>
#include "memory.h"
#define XZR 32
// Select parts of the instruction register
extern uint32_t mask_instr_bits(uint32_t instruction, uint8_t start, uint8_t end);

extern uint32_t load32(Sys_Memory *mem, uint64_t address);

extern int64_t sgx(uint64_t num, uint64_t mask);
