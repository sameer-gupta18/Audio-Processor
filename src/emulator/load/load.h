#ifndef LOAD_H
#define LOAD_H
#include "../processor.h"
// Possible errors: Cannot open file, not divisible by 32.
extern int load_instructions(CPU_state* state, const char* bin_file);

#endif
