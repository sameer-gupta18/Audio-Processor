#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "types.h"

typedef struct{
    bool N;
    bool Z;
    bool C;
    bool V;
} PSTATE;

typedef struct{
    uint64_t registers[32];
    uint64_t pc;
    uint64_t z;
    PSTATE pstate;
    Sys_Memory* memory;
} CPU_state;


extern uint32_t instruction_fetch(CPU_state* state);

extern void instruction_decode(CPU_state* state, uint32_t instruction);


#endif
