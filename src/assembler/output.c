#include "output.h"
#include <stdint.h>
#define REGISTER_OFFSET_LITERAL 0x1Au
#define LITTLE_ENDIAN_MASK 0xFFu

static void write_word_le(FILE* output, uint32_t word);
static uint32_t assemble_word(Assembled_Instruction *ins);

void output(FILE* output, Assembled_Instruction* assembled_list, size_t count) {

    for (size_t i = 0; i < count; i++) {
        Assembled_Instruction *ins = &assembled_list[i];
        uint32_t word = assemble_word(ins);
        write_word_le(output, word);
    }

}

// Mask simm{n} values
static uint32_t mask_simms(int32_t simm, uint8_t num){
    return (uint32_t)simm & ((1u << num)-1); 
}

// Write word to output file in little-endian format
static void write_word_le(FILE* output, uint32_t word) {
    unsigned char bytes[4];
    bytes[0] = word & LITTLE_ENDIAN_MASK;
    bytes[1] = (word >> 8) & LITTLE_ENDIAN_MASK;
    bytes[2] = (word >> 16) & LITTLE_ENDIAN_MASK;
    bytes[3] = (word >> 24) & LITTLE_ENDIAN_MASK;
    fwrite(bytes, 1, 4, output);
}

// Convert an assembled instruction to binary
static uint32_t assemble_word(Assembled_Instruction *ins) {
    uint32_t word = 0;
    switch (ins->kind) {
        case DPI: {
            DP_Immediate *dpi = &ins->instruction_data.dpi;
            word |= ((uint32_t)dpi->sf << 31);
            word |= ((uint32_t)dpi->opc << 29);
            word |= (4u << 26);
            word |= ((uint32_t)dpi->opi << 23);

            uint32_t operand = 0;
            if (dpi->opi == 2) {
                operand |= ((uint32_t)(dpi->operand.immediate_arithmetic.sh ? 1 : 0) << 17);
                operand |= ((uint32_t)(dpi->operand.immediate_arithmetic.imm12) << 5);
                operand |= (uint32_t)dpi->operand.immediate_arithmetic.rn;
            } else {
                operand |= (uint32_t)(dpi->operand.immediate_widemove.hw) << 16;
                operand |= (uint32_t)(dpi->operand.immediate_widemove.imm16);
            }
            
            word |= (operand << 5);
            word |= (uint32_t)(dpi->rd & 0x1Fu);
            break;
        }

        case DPR: {
            DP_Register *dpr = &ins->instruction_data.dpr;
            word |= ((uint32_t)dpr->sf << 31);
            word |= ((uint32_t)dpr->opc << 29);
            word |= ((uint32_t)dpr->M << 28);
            word |= (5u << 25);

            uint32_t opr = 0;
            opr |= ((uint32_t)(dpr->opr.first_bit ? 1 : 0) << 3);
            opr |= ((uint32_t)dpr->opr.shift << 1);
            opr |= ((uint32_t)(dpr->opr.n ? 1 : 0)); 
            
            word |= (opr << 21);
            word |= ((uint32_t)dpr->rm << 16);

            uint32_t operand = 0;
            if (dpr->M) {
            operand |= (dpr->operand.multiply_operand.x ? 1u : 0u) << 5;
            operand |= dpr->operand.multiply_operand.ra;
            }
            else {
                operand |= (uint32_t)dpr->operand.arith_logic_operand;
            }
            word |= (operand << 10);

            word |= ((uint32_t)dpr->rn << 5);
            word |= (uint32_t)dpr->rd;
            break; 
        }

        case SDT: {
            Single_Data *sdt = &ins->instruction_data.sdt;
            word |= (1u << 31);
            word |= ((uint32_t)sdt->sf << 30);
            word |= (28u << 25);
            word |= ((uint32_t)sdt->u << 24);
            word |= ((uint32_t)sdt->l << 22);

            uint32_t offset = 0;
            switch (sdt->offset.mode) {
                case UNSIGNED: {
                    offset = (uint32_t)sdt->offset.data.imm12;
                    break;
                }
                case REG_OFFSET: {
                    offset = (1u << 11) | ((uint32_t)sdt->offset.data.xm << 6) | REGISTER_OFFSET_LITERAL;
                    break;
                }
                case INDEX: {
                    offset = ((uint32_t)(mask_simms(sdt->offset.data.index.simm9, 9)) << 2)
                            | ((uint32_t)(sdt->offset.data.index.i & 0x1u) << 1)
                            | 1u;
                    break;
                }
            }

            word |= (offset << 10);
            word |= ((uint32_t)sdt->xn << 5);
            word |= (uint32_t)sdt->rt;

            break;
        }

        case LOAD: {
            Load_Literal *ld = &ins->instruction_data.load_literal;
            word |= ((uint32_t)ld->sf << 30);
            word |= (24u << 24);
            word |= ((uint32_t)(mask_simms(ld->simm19, 19)) << 5);
            word |= (uint32_t)ld->rt;
            break;
        }

        case BRANCH: {
            Branch_Instruction *br = &ins->instruction_data.branch;
            uint32_t type_bits = 0; // First three bits of branch instruction
            switch (br->mode) {
                case UNCOND: type_bits = 0u; break;
                case COND: type_bits = 1u; break;
                case REG_BRANCH: type_bits = 3u; break;
            }
            word |= (type_bits << 30);
            word |= (5u << 26);

            switch (br->mode) {
                case UNCOND: {
                    word |= (uint32_t)(mask_simms(br->data.simm26, 26));
                    break;
                }
                case COND: {
                    word |= ((uint32_t)(mask_simms(br->data.conditional.simm19, 19)) << 5);
                    word |= (uint32_t)br->data.conditional.cond;
                    break;
                }
                case REG_BRANCH: {
                    word |= (1u << 25);
                    word |= (0x1Fu << 16); 
                    word |= ((uint32_t)br->data.xn << 5);
                    break;
                }
            }
            break;
        }

        case SPECIAL: {
            word = (uint32_t)ins->instruction_data.value.value;
            break;
        }

        default:
            break;
        
    }

    return word;
}
