#ifndef EXECUTE_H
#define EXECUTE_H
#include "processor.h"

extern void data_processing_immediate(
    CPU_state* state,
    uint8_t s, 
    uint8_t opc, 
    uint8_t opi, 
    uint32_t operand, 
    uint8_t rd
);
// and many more

#endif
