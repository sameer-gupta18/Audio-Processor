#ifndef ENCODE_H
#define ENCODE_H
#include "../assembleIR.h"
#include "../symtable.h"
#include "../parserIR.h"
#include "../types.h"

// Macros for alias instruction IR
#define ALIAS_CMX(MNEM) {\
                    (MNEM),\
                    instrs[i].f0.field_data.reg.sf == 1 ? RZR : WZR,\
                    instrs[i].f0,\
                    instrs[i].f1,\
                    instrs[i].f2,\
                    .num_fields = NUM_FIELDS,\
                    instrs[i].address,\
                    instrs[i].cond\
                };
#define ALIAS_NEGX(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    RZR,\
                    instrs[i].f1,\
                    .num_fields = NUM_FIELDS - 1,\
                    instrs[i].address,\
                    instrs[i].cond\
                };
#define ALIAS_MOVE(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    RZR,\
                    instrs[i].f1,\
                    .num_fields = NUM_FIELDS - 1,\
                    instrs[i].address,\
                    instrs[i].cond\
                };

#define ALIAS_ARITH(MNEM) {\
                    (MNEM),\
                    instrs[i].f0,\
                    instrs[i].f1,\
                    instrs[i].f2,\
                    RZR,\
                    NUM_FIELDS,\
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
                    instrs[i].f0.field_data.reg.sf == 1 ? RZR : WZR,\
                    instrs[i].f0,\
                    instrs[i].f1,\
                    instrs[i].f2,\
                    NUM_FIELDS,\
                    instrs[i].address,\
                    instrs[i].cond\
                };

typedef enum {
    ENCODE_OK = 0,
    ENCODE_FAIL = 1
} ENCODE_STATUS;

extern int encode(
    Sym_Table *sym_table, 
    Parser_Instruction* instrs,
    size_t list_length,
    Assembled_Instruction* output,
    size_t* curr_idx
);

#endif
