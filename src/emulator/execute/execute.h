#ifndef EXECUTE_H
#define EXECUTE_H
#include "../processor.h"

extern int data_processing_immediate(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint8_t opi, 
    uint32_t operand, 
    uint8_t rd
);

extern int dpi_arithmetic(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint32_t operand, 
    uint8_t rd
);

extern int dpi_wide_move(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint32_t operand, 
    uint8_t rd
);

extern int data_processing_register(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
);

extern int arithmetic_logic(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
);

extern int multiply(
    CPU_state* state,
    uint8_t sf, 
    uint8_t opc, 
    uint8_t rm,
    uint8_t opr, 
    uint8_t operand, 
    uint8_t rn,
    uint8_t rd
);

extern int single_data_transfer(
    CPU_state* state,
    uint8_t sf, 
    uint8_t u, 
    uint8_t l, 
    uint16_t offset, 
    uint8_t xn, 
    uint8_t rt
);
extern int load_literal(
    CPU_state* state,
    uint8_t sf, 
    int32_t simm19,
    uint8_t rt
);

extern int branch(
    CPU_state* state,
    uint8_t type,
    uint32_t operand
);


// and many more

#endif
