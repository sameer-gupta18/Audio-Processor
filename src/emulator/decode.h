#ifndef DECODE_H
#define DECODE_H 
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "processor.h"

typedef enum{
     DECODE_OK= 0,
     DECODE_BRANCH = 1, 
     DECODE_FAIL = -1,
     DECODE_HALT = 2
    } STATUS;

extern CPU_state state_init(Sys_Memory* memory);
extern uint32_t instruction_fetch(CPU_state* state);
extern int instruction_decode(CPU_state* state, uint32_t instruction);


#endif 
