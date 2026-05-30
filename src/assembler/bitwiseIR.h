#include <stdint.h>

typedef enum {DPI, DPR, SDT, LOAD, BRANCH} instruction_kind; 
typedef enum {UNCOND, COND, REG_BRANCH} branch_type; 
typedef enum {REG_OFFSET, INDEX, UNSIGNED} sdt_type; 

typedef struct {
    uint8_t sf;
    uint8_t opc;
    uint8_t opi; 
    uint32_t operand; 
    uint8_t rd; 
} DP_Immediate;

typedef struct {
    uint8_t sf;
    uint8_t opc;
    uint8_t M; 
    uint8_t opr;
    uint8_t rm; 
    uint8_t operand; 
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
    uint8_t u;
    uint8_t l;
    Addressing_Mode operand;
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
    uint8_t cond;
} Conditional_Branch; 


typedef struct{
    branch_type mode; 
    union {
        int32_t simm26;
        uint8_t xn;
        Conditional_Branch conditional;
    } data; 
} Branch_Instruction; 

typedef struct{
    instruction_kind kind; 
    union {
        DP_Immediate dpi;
        DP_Register dpr;
        Single_Data sdt;
        Load_Literal load_literal;
        Branch_Instruction branch;
    } instruction_data;
} Bitwise_Instruction; 
