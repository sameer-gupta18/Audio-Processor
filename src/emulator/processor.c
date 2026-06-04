#include "processor.h"
#include "memory.h"
#include <stdbool.h>
#include <stdint.h>

// Initialising CPU State
CPU_state state_init(Sys_Memory* memory){
    PSTATE pstate = {false, true, false, false};
    CPU_state res = {{0}, 0, pstate, memory};
    return res; 
}

