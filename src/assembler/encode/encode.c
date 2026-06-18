#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "encode.h"
#include "handle.h"
         
static Parser_Field RZR = {
                    REGISTER,
                    .field_data.reg = {
                        1,
                        XZR
                    }
                };

static Parser_Field WZR = {
                    REGISTER,
                    .field_data.reg = {
                        0,
                        XZR
                    }
                };

int encode(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output,
    size_t* curr_idx //pass pointer to 0 counter initially
){    

    //loops over all the instructions

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
                //deal with different types of arithmetic

                if(instrs[i].f2.field == REGISTER){
                    out = handle_register(instrs, i,output,*curr_idx);
                }
                else if(instrs[i].f2.field == IMMEDIATE){
                    out = handle_immediate(instrs, i,output,*curr_idx);
                }
                else{
                    fprintf(stderr, "Invalid Instruction");
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
                    fprintf(stderr, "Invalid Instruction");
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
                //deal with the different kinds of loads and stores

                if (instrs[i].f1.field_data.address.kind != LITERAL){
                    out = handle_sdt(instrs,i,output,*curr_idx);
                }
                else if(instrs[i].mnemonic == LDR) {
                    out = handle_load_literal(instrs,i,output,*curr_idx,sym_table);
                }
                else{
                    fprintf(stderr,"Invalid Instruction: Store cannot take a literal");
                    return ENCODE_FAIL;
                }
                break;
            case B:
            case BCOND:
            case BR:
                out = handle_branch(instrs,i,output,*curr_idx,sym_table);
                break;

            //DEALING WITH ALIASES

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
                Parser_Instruction alias_mneg = ALIAS_ARITH(MSUB); 
                out = ALIAS_ENCODE(alias_mneg); 
                (*curr_idx)--; 
                break;
            }
            default:
                fprintf(stderr,"Unknown Mnemonic");
                return ENCODE_FAIL;
        }

        //catch errors collectively
    
        if (out == ENCODE_FAIL){
            return out;
        }
        (*curr_idx)++;
    }

    return ENCODE_OK;
    
}
