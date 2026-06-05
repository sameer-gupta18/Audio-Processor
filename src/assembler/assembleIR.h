#include <stdint.h>
#include "parserIR.h"
typedef enum {DPI, DPR, SDT, LOAD, BRANCH, SPECIAL} instruction_kind; 
typedef enum {UNCOND, COND, REG_BRANCH} branch_type; 
typedef enum {REG_OFFSET, INDEX, UNSIGNED} sdt_type; 

typedef struct {
    bool sh;
    uint32_t imm12;
    uint8_t rn;
} Immediate_Arithmetic;

typedef struct {
    uint8_t hw;
    uint32_t imm16;
} Immediate_Wide_Move;

typedef struct {
    uint8_t sf;
    uint8_t opc;
    uint8_t opi; 
    union {
        Immediate_Arithmetic immediate_arithmetic;
        Immediate_Wide_Move immediate_widemove;
    } operand;
    uint8_t rd; 
} DP_Immediate;

typedef struct {
    bool first_bit;
    uint8_t shift;
    bool n; 
} Register_Operation;

typedef struct {
    bool x;
    uint8_t ra;
} Register_Multiply;

typedef struct {
    uint8_t sf;
    uint8_t opc;
    uint8_t M; 
    Register_Operation opr;
    uint8_t rm; 
    union {
        uint8_t arith_logic_operand;
        Register_Multiply multiply_operand;
    } operand; 
    uint8_t rn;
    uint8_t rd; 
} DP_Register; 

typedef struct {
    int16_t simm9;
    uint8_t i;
} Pre_Post_Index;

typedef struct{
    sdt_type mode; 
    union{
        uint8_t xm;
        Pre_Post_Index index; 
        uint16_t imm12; 
    } data;
} Addressing_Mode;

typedef struct{
    uint8_t sf; 
    bool u;
    bool l;
    Addressing_Mode offset;
    uint8_t xn;
    uint8_t rt;
} Single_Data; 

typedef struct {
    uint8_t sf;
    int32_t simm19;
    uint8_t rt; 
} Load_Literal;

typedef struct{
    int32_t simm19;
    conds cond;
} Conditional_Branch; 


typedef struct{
    branch_type mode; 
    union {
        int32_t simm26; //unconditional
        uint8_t xn; //register
        Conditional_Branch conditional; //conditional
    } data; 
} Branch_Instruction; 

typedef struct{
    int32_t value;
} Special_Instruction;

typedef struct{
    instruction_kind kind; 
    union {
        DP_Immediate dpi;
        DP_Register dpr;
        Single_Data sdt;
        Load_Literal load_literal;
        Branch_Instruction branch;
        Special_Instruction value;
    } instruction_data;
} Assembled_Instruction; 
