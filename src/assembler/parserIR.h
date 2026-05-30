#ifndef PARSER_IR_H
#define PARSER_IR_H
#include <stdbool.h>
#include <stdint.h>

typedef enum{
    ADD,
    ADDS,
    SUB,
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

typedef enum { LIT_LABEL, LIT_ADDR } Literal_Kind;

typedef struct{
    Literal_Kind literal_kind; // tells which field the owner can pick
    union{
        uint64_t int_address;
        char *label;
    } data;
} Address_Literal;

typedef struct{
    Address_Kind kind; 
    union{
        Address_Literal literal;
        struct {Parser_Register xn; uint32_t imm; } unsigned_offset;
        struct {Parser_Register xn; Parser_Register xm;} register_offset;
        struct {Parser_Register xn; int32_t simm;} pre_post_index;
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
