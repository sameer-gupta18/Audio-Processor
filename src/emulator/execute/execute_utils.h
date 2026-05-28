#ifndef EXECUTE_UTILS_H
#define EXECUTE_UTILS_H
#include <stdint.h>
#include "../processor.h"
#define FULL_REG_MASK 0xFFFFFFFFFFFFFFFFULL
#define HALF_REG_MASK 0xFFFFFFFFULL
void update_flags(CPU_state* state, uint64_t val1, uint64_t val2, uint64_t res, uint8_t sf, bool is_add);

#endif