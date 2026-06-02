#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "parserIR.h"
#include "symtable.h"



// int first_pass(
//     Sym_Table* sym_table,
//     Parser_Instruction* instruction_list,
//     size_t list_length
// ){
//     for (size_t i = 0; i < list_length; i++){
//         if (instruction_list[i].mnemonic == LABEL && instruction_list[i].num_fields == 1){
//             insert_address(
//                 sym_table,
//                 //can add asserts here to check if the data is valid
//                 instruction_list[i].fields[0].field_data.address.address_data.literal.data.label,
//                 instruction_list[i].address
//             );
//         } 
//         else{
//             continue;
//         }
//     }

//     return ENCODE_OK;
// }


int handle_directives(
    Parser_Instruction* instrs,
    size_t i,
    Assembled_Instruction* output,
    size_t curr_idx
){
    if (instrs[i].num_fields != 2 || instrs[i].fields[1].field != IMMEDIATE){
        printf(stderr, "Invalid Directive. Follow '.int <simm>'");
        return ENCODE_FAIL;
    }
    Special_Instruction out = {
        instrs[i].fields[1].field_data.immediate
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
                instr[i].num_fields == 4 && instr[i].fields[3].field_data.shift.shift_amount == 12,
                instr[i].fields[2].field_data.immediate,
                instr[i].fields[1].field_data.reg.index
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
                instr[i].fields[2].field_data.shift.shift_amount >> 4,
                instr[i].fields[1].field_data.immediate
            };
            instruction.opc = opc;
            instruction.opi = opi;
            instruction.operand.immediate_widemove = wm_operand;
            break;
        default:
            printf(stderr, "Invalid mnemonic for assembling");
            return ENCODE_FAIL;
    }

    instruction.sf = instr[i].fields[0].field_data.reg.sf;
    instruction.rd = instr[i].fields[0].field_data.reg.index;

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
    //TODO
}

int second_pass(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output
){
    // first_pass(sym_table,instrs,list_length);
    size_t curr_idx = 0;

    for (size_t i = 0; i < list_length; i++){
        bool out;
        switch(instrs[i].mnemonic){
            case DIRECTIVE:
                out = handle_directives(instrs,i,output,curr_idx);
                break;
            case LABEL:
                continue;   //dont have to assemble any binary
            case ADD:
            case ADDS:
            case SUB:
            case SUBS:
                if(instrs[i].fields[2].field == REGISTER){
                    out = handle_register(instrs, i,output,curr_idx);
                }
                else if(instrs[i].fields[2].field == IMMEDIATE){
                    out = handle_immediate(instrs, i,output,curr_idx);
                }
                else{
                    printf(stderr, "Invalid Instruction");
                    return ENCODE_FAIL;
                }
                break;
            case MOVN:
            case MOVZ:
            case MOVK:
                if(instrs[i].fields[1].field == IMMEDIATE){
                    out = handle_immediate(instrs, i,output,curr_idx);
                }
                else{
                    printf(stderr, "Invalid Instruction");
                    return ENCODE_FAIL;
                }
                break;
            default:
                printf(stderr,"Unknown Command");
                return ENCODE_FAIL;
            
        }

        if (out == ENCODE_FAIL){
            return out;
        }
        curr_idx++;
    }


    return ENCODE_OK;
}