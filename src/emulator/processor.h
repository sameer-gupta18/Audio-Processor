#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "memory.h"

typedef struct{
    bool N;
    bool Z;
    bool C;
    bool V;
} PSTATE;


typedef struct{
    uint64_t registers[31];
    uint64_t pc;
    // uint64_t z; -- Perhaps unecessary. 
    PSTATE pstate;
    Sys_Memory* memory;
} CPU_state;



#endif
