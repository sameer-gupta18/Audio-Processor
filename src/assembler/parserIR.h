#ifndef PARSER_IR
#define PARSER_IR
#include <stdbool.h>
#include <stdint.h>

typedef enum{
    ADD,
    ADDS,
    SUB,
    HALT,
    DIRECTIVE
    // and many more
} Mnemonic; 

typedef enum{
    REGISTER, 
    IMMEDIATE, 
    SHIFT,
    ADDRESS
} Field_Type;

typedef struct{
    bool sf; // 1 if 64, 0 is 32
    uint8_t index; // 31 it is rzr
} Parser_Register;

typedef enum{
    LSL,
    LSR,
    ASR,
    ROR
} Shift_Kind;

typedef struct{
    Shift_Kind kind;
    uint32_t shift_amount; 
} Parser_Shift;

typedef enum{
    UNSIGNED_OFFSET,
    PRE_INDEXED,
    POST_INDEXED,
    REGISTER_OFFSET,
    LITERAL
} Address_Kind; 

typedef struct{
    union{
        uint64_t int_address;
        char *label;
    } data;
} Address_Literal;

typedef struct{
    Address_Kind kind; 
    union{
        Address_Literal literal;
        struct{
            Parser_Register xn;
            Parser_Register xm; 
            uint32_t imm; 
            int32_t simm;
        } offset_data; 
    } address_data; 
} Parser_Address;

typedef struct{
    Field_Type field;
    union{
        Parser_Address address;
        Parser_Register reg;
        Parser_Shift shift; 
        int64_t immediate;
    } field_data;
} Parser_Field;

typedef struct{
    Mnemonic mnemonic;
    Parser_Field fields[4];
    uint8_t num_fields;
    uint32_t address;
    uint8_t cond; // if b.cond
} Parser_Instruction;

#endif
