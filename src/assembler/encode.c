#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "encode.h"
#include "parserIR.h"
#include "symtable.h"
#include "utils.h"

// Macros for alias instruction IR
#define ALIAS_CMX(MNEM) {\
                    (MNEM),\
                    RZR,\
                    instrs[i].f0,\
                    instrs[i].f1,\
                    .num_fields = 3,\
                    instrs[i].address,\
                    instrs[i].cond\
                };
#define ALIAS_NEGX(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    RZR,\
                    instrs[i].f1,\
                    .num_fields = 3,\
                    instrs[i].address,\
                    instrs[i].cond\
                };
#define ALIAS_MOVE(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    RZR,\
                    instrs[i].f1,\
                    .num_fields = 3,\
                    instrs[i].address,\
                    instrs[i].cond\
                };

#define ALIAS_ARITH(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    instrs[i].f1,\
                    instrs[i].f2,\
                    RZR,\
                    4,\
                    instrs[i].address,\
                    instrs[i].cond\
                };

#define ALIAS_ENCODE(IR) encode(\
                    sym_table,\
                    &(IR),\
                    1,\
                    output,\
                    curr_idx\
                );

#define ALIAS_TST(){\
                    ANDS,\
                    RZR,\
                    instrs[i].f1,\
                    instrs[i].f2,\
                    .num_fields = 3,\
                    instrs[i].address,\
                    instrs[i].cond\
                };

static Parser_Field RZR = {
                    REGISTER,
                    .field_data.reg = {
                        1,
                        XZR
                    }
                };

int handle_directives(
    Parser_Instruction* instrs,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    if (instrs[i].num_fields != 2 || instrs[i].f1.field != IMMEDIATE){
        printf(stderr, "Invalid Directive. Follow '.int <simm>'");
        return ENCODE_FAIL;
    }
    Special_Instruction out = {
        instrs[i].f1.field_data.immediate
    };
    Assembled_Instruction assembled_instr = {SPECIAL,.instruction_data.value=out};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}

int handle_immediate(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    DP_Immediate instruction;
    uint8_t opi, opc;

    switch(instr[i].mnemonic){
        case ADD:
        case MOVN:
            opc = 0;
            break;
        case ADDS:
            opc = 1;
            break;
        case SUB:
        case MOVZ:
            opc = 2;
            break;
        case SUBS:
        case MOVK:
            opc = 3;
            break;
        default:
            printf(stderr, "Invalid mnemonic for assembling");
            return ENCODE_FAIL;
    }

    switch(instr[i].mnemonic){
        case ADD:
        case ADDS:
        case SUB:
        case SUBS:
            opi = 2;
            Immediate_Arithmetic arith_operand = {
                instr[i].num_fields == 4 && instr[i].f3.field_data.shift.shift_amount == 12,
                instr[i].f2.field_data.immediate,
                instr[i].f1.field_data.reg.index
            };
            instruction.opc = opc;
            instruction.opi = opi;
            instruction.operand.immediate_arithmetic = arith_operand;
            break;
        case MOVN:
        case MOVZ:
        case MOVK:
            opi = 5;
            Immediate_Wide_Move wm_operand = {
                instr[i].f2.field_data.shift.shift_amount >> 4,
                instr[i].f1.field_data.immediate
            };
            instruction.opc = opc;
            instruction.opi = opi;
            instruction.operand.immediate_widemove = wm_operand;
            break;
        default:
            printf(stderr, "Invalid mnemonic for assembling");
            return ENCODE_FAIL;
    }

    instruction.sf = instr[i].f0.field_data.reg.sf;
    instruction.rd = instr[i].f0.field_data.reg.index;

    Assembled_Instruction assembled_instr = {DPI,.instruction_data.dpi = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}

int handle_register(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    uint8_t opc;
    bool n = 0;
    Register_Operation opr;
    DP_Register instruction;
    switch(instr[i].mnemonic){
        case ADD:
        case ADDS:
        case SUB:
        case SUBS:
            instruction.M = 0;
            Register_Operation arith_opr = {
                1,
                instr[i].f3.field_data.shift.kind,
            };
            opr = arith_opr;
            instruction.operand.arith_logic_operand = instr[i].f3.field_data.shift.shift_amount;
            break;
        case AND:
        case BIC:
        case ORR:
        case ORN:
        case EOR:
        case EON:
        case ANDS:
        case BICS:
            instruction.M = 0;
            Register_Operation logic_opr = {
                0,
                instr[i].f3.field_data.shift.kind,
            };
            instruction.operand.arith_logic_operand = instr[i].f3.field_data.shift.shift_amount;
            opr = logic_opr;
            break;
        case MADD:
        case MSUB:
            instruction.M = 1;
            Register_Operation multiply_opr = {
                1,0,0
            };
            break;
        default:
            printf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;   
    }

    switch(instr[i].mnemonic){
        case BIC:
            n = 1;
        case ADD:
        case AND:
            opc = 0;
            break;
        case ORN:
            n = 1;
        case ADDS:
        case ORR:
            opc = 1;
            break;
        case EON:
            n = 1;
        case SUB:
        case EOR:
            opc = 2;
            break;
        case BICS:
            n = 1;
        case ANDS:
        case SUBS:
            opc = 3;
            break;
        case MSUB:
            opc = 0;
            Register_Multiply reg_mult_msub = {
                1, instr[i].f3.field_data.reg.index
            };
            instruction.operand.multiply_operand=reg_mult_msub;
            break;
        case MADD:
            opc = 0;
            Register_Multiply reg_mult_madd = {
                0, instr[i].f3.field_data.reg.index
            };
            instruction.operand.multiply_operand=reg_mult_madd;
            break;
        default:
            printf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;   
    }

    instruction.opc = opc;
    opr.n = n;
    instruction.opr = opr;
    instruction.rm = instr[i].f2.field_data.reg.index;
    instruction.rn = instr[i].f1.field_data.reg.index;
    instruction.rd = instr[i].f0.field_data.reg.index;

    instruction.sf = instr[i].f0.field_data.reg.sf;

    Assembled_Instruction assembled_instr = {DPR, .instruction_data.dpr=instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;

}

uint64_t handle_literal(Address_Literal address, Sym_Table* sym_table){
    uint64_t addr; 
    if (address.literal_kind == LIT_ADDR){
        addr = address.data.int_address;
    }
    else{
        search_label(sym_table,address.data.label,&addr);
    }
    return addr;
}

int handle_load_literal(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
){
    Load_Literal instruction;
    instruction.sf = instr[i].f0.field_data.reg.sf;
    instruction.rt = instr[i].f0.field_data.reg.index;
    uint64_t addr = handle_literal(instr[i].f1.field_data.address.address_data.literal,sym_table); 

    if (addr % 4 == 0 && (instr[i].address >= addr ? (instr[i].address) - addr : addr - (instr[i].address)) < MB){
        instruction.simm19 = addr >> 2;
    } else{
        printf(stderr, "Invalid Offset");
        return ENCODE_FAIL;
    }
    Assembled_Instruction assembled_instr = {LOAD,.instruction_data.load_literal = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}

int handle_sdt(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    Single_Data instruction;

    bool u = 0;

    instruction.sf = instr[i].f0.field_data.reg.sf;
    instruction.xn = instr[i].f1.field_data.reg.index;
    switch(instr[i].mnemonic){
        case LDR:
            instruction.l = 1;
        case STR:
            instruction.l = 0;
        default:
            printf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;
    }

    Addressing_Mode offset;

    switch(instr[i].f1.field_data.address.kind){
        case UNSIGNED_OFFSET:
            u = 1;
            offset.mode = UNSIGNED;
            offset.data.imm12 = 
                instruction.sf ? 
                instr[i].f1.field_data.address.address_data.unsigned_offset.imm >> 3:
                instr[i].f1.field_data.address.address_data.unsigned_offset.imm >> 2;
            break;
        case PRE_INDEXED:
            offset.mode = INDEX;
            offset.data.index.i = 1;
            offset.data.index.simm9 = instr[i].f1.field_data.address.address_data.pre_post_index.simm;
            break;
        case POST_INDEXED:
            offset.mode = INDEX;
            offset.data.index.i = 0;
            offset.data.index.simm9 = instr[i].f1.field_data.address.address_data.pre_post_index.simm;
            break;
        case REGISTER_OFFSET:
            offset.mode = REG_OFFSET;
            offset.data.xm = instr[i].f1.field_data.address.address_data.register_offset.xm.index;
            break;
        case LITERAL:
            printf(stderr, "Logical Error. Never Should have happened.");
            return ENCODE_FAIL;
    }
    instruction.offset = offset;
    instruction.u = u;
    instruction.rt = instr[i].f0.field_data.reg.index;
    Assembled_Instruction assembled_instr = {SDT, .instruction_data.sdt=instruction};
    output[curr_idx] = assembled_instr;

    return ENCODE_OK;
}

int handle_branch(
    Parser_Instruction* instr,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx,
    Sym_Table* sym_table
){
    Branch_Instruction instruction;
    switch(instr[i].mnemonic){
        case B:
            instruction.mode = UNCOND;
            instruction.data.simm26 = 
                handle_literal(instr[i].f0.field_data.address.address_data.literal,sym_table);
            break;
        case BR:
            instruction.mode = REG_BRANCH;
            instruction.data.xn = instr[i].f0.field_data.reg.index;
            break;
        case BCOND:
            instruction.mode = COND;
            instruction.data.conditional.cond = instr[i].cond;
            instruction.data.conditional.simm19 = 
                handle_literal(instr[i].f0.field_data.address.address_data.literal,sym_table);
        default:
            printf(stderr, "Invalid Instruction");
            return ENCODE_FAIL;
    }

    Assembled_Instruction assembled_instr = {BRANCH,.instruction_data.branch = instruction};
    output[curr_idx] = assembled_instr;
    return ENCODE_OK;
}

int encode(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output,
    size_t* curr_idx //pass pointer to 0 counter initially
){    

    for (size_t i = 0; i < list_length; i++){
        bool out;
        switch(instrs[i].mnemonic){
            case DIRECTIVE:
                out = handle_directives(instrs,i,output,*curr_idx);
                break;
            case LABEL:
                continue;   //dont have to assemble any binary
            case ADD:
            case ADDS:
            case SUB:
            case SUBS:
                if(instrs[i].f2.field == REGISTER){
                    out = handle_register(instrs, i,output,*curr_idx);
                }
                else if(instrs[i].f2.field == IMMEDIATE){
                    out = handle_immediate(instrs, i,output,*curr_idx);
                }
                else{
                    printf(stderr, "Invalid Instruction");
                    return ENCODE_FAIL;
                }
                break;
            case MOVN:
            case MOVZ:
            case MOVK:
                if(instrs[i].f1.field == IMMEDIATE){
                    out = handle_immediate(instrs, i,output,*curr_idx);
                }
                else{
                    printf(stderr, "Invalid Instruction");
                    return ENCODE_FAIL;
                }
                break;
            case AND:
            case BIC:
            case ORR:
            case ORN:
            case EOR:
            case EON:
            case ANDS:
            case BICS:
            case MADD:
            case MSUB:
                out = handle_register(instrs,i,output,*curr_idx);
                break;
            case LDR:
            case STR:
                if (instrs[i].f1.field_data.address.kind != LITERAL){
                    out = handle_sdt(instrs,i,output,*curr_idx);
                }
                else if(instrs[i].mnemonic == LDR) {
                    out = handle_load_literal(instrs,i,output,*curr_idx,sym_table);
                }
                else{
                    printf(stderr,"Invalid Instruction: Store cannot take a literal");
                    return ENCODE_FAIL;
                }
                break;
            case B:
            case BCOND:
            case BR:
                out = handle_branch(instrs,i,output,*curr_idx,sym_table);
                break;
            case CMP: {
                Parser_Instruction alias_cmp = ALIAS_CMX(SUBS); 
                out = ALIAS_ENCODE(alias_cmp); 
                (*curr_idx)--;
                break;
            }
            case CMN: {
                Parser_Instruction alias_cmn = ALIAS_CMX(ADDS); 
                out = ALIAS_ENCODE(alias_cmn); 
                (*curr_idx)--;
                break;
            }
            case NEG: {
                Parser_Instruction alias_neg = ALIAS_NEGX(SUB);
                out = ALIAS_ENCODE(alias_neg); 
                (*curr_idx)--;
                break;
            }
            case NEGS: {
                Parser_Instruction alias_negs = ALIAS_NEGX(SUBS);
                out = ALIAS_ENCODE(alias_negs);
                (*curr_idx)--;
                break;
            }
            case TST: {
                Parser_Instruction alias_tst = ALIAS_TST();
                out = ALIAS_ENCODE(alias_tst);
                (*curr_idx)--;
                break;
            }
            case MVN: {
                Parser_Instruction alias_mvn = ALIAS_MOVE(ORN); 
                out = ALIAS_ENCODE(alias_mvn);
                (*curr_idx)--;
                break;
            }
            case MOV: {
                Parser_Instruction alias_mov = ALIAS_MOVE(ORR); 
                out = ALIAS_ENCODE(alias_mov);
                (*curr_idx)--;
                break;
            }
            case MUL: {
                Parser_Instruction alias_mul = ALIAS_ARITH(MADD); 
                out = ALIAS_ENCODE(alias_mul);
                (*curr_idx)--;
                break;
            }
            case MNEG: {
                Parser_Instruction alias_mneg = ALIAS_ARITH(MNEG); 
                out = ALIAS_ENCODE(alias_mneg); 
                (*curr_idx)--; 
                break;
            }
            default:
                printf(stderr,"Unknown Mnemonic");
                return ENCODE_FAIL;
        }

        if (out == ENCODE_FAIL){
            return out;
        }
        (*curr_idx)++;
    }

    return ENCODE_OK;
}
