#include <stdint.h>
#include <stdio.h>
#include "encode.h"
#include "parserIR.h"
#include "symtable.h"


int first_pass(
    Sym_Table* sym_table,
    Parser_Instruction* instruction_list,
    size_t list_length
){
    for (size_t i = 0; i < list_length; i++){
        if (instruction_list[i].mnemonic == LABEL && instruction_list[i].num_fields == 1){
            insert_address(
                sym_table,
                //can add asserts here to check if the data is valid
                instruction_list[i].fields[0].field_data.address.address_data.literal.data.label,
                instruction_list[i].address
            );
        } 
        else{
            continue;
        }
    }

    return ENCODE_OK;
}


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


int second_pass(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output
){
    first_pass(sym_table,instrs,list_length);
    size_t curr_idx = 0;

    for (size_t i = 0; i < list_length; i++){
        bool out;
        // switch(instrs[i].mnemonic){
        //     case DIRECTIVE:{
        //         out = handle_directives(instrs,i,output,curr_idx);
        //         break;
        //     }
        // }

        if (out == ENCODE_FAIL){
            return ENCODE_FAIL;
        }
        curr_idx++;
    }


    return ENCODE_OK;
}