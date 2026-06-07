#ifndef PARSER_IR_H
#define PARSER_IR_H
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"

typedef enum{
    DIRECTIVE,
    ADD,
    LABEL,
    ADDS,
    SUB,
    SUBS,
    CMP,
    CMN,
    NEG,
    NEGS,
    AND,
    ANDS,
    BIC,
    BICS,
    EOR,
    ORR,
    EON,
    ORN,
    TST,
    MOVK,
    MOVN,
    MOVZ,
    MOV,
    MVN,
    MADD,
    MSUB,
    MUL,
    MNEG,
    B,
    BCOND,
    BR,
    STR,
    LDR
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
    LSL = 0,
    LSR = 1,
    ASR = 2,
    ROR = 3
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
        int32_t immediate;  //Possibly 64 bit
    } field_data;
} Parser_Field;

typedef struct{
    Mnemonic mnemonic;
    Parser_Field f0;
    Parser_Field f1;
    Parser_Field f2;
    Parser_Field f3;
    uint8_t num_fields;
    uint32_t address;
    conds cond; // if b.cond
} Parser_Instruction;

#endif
