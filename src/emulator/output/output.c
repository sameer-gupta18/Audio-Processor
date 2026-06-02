#include "output.h"
#include "../utils.h"
#include <stdio.h>
#define NUM_REGS 31

void output(CPU_state* state, FILE* output){
   // Priting registers
   fprintf(output,"Registers:\n");
   for(int i = 0; i < NUM_REGS; i++ ){
       fprintf(output,"X%02d = %016lx\n", i, state->registers[i]);
   }

   // Priting PC and PSTATE
   fprintf(output,"PC = %016lx\n", state->pc);
   fprintf(output, "PSTATE: ");
   fprintf(
       output,
       "%c%c%c%c\n",
       state->pstate.N ? 'N' : '-',
       state->pstate.Z ? 'Z' : '-',
       state->pstate.C ? 'C' : '-',
       state->pstate.V ? 'V' : '-'
   );
  
   // Priting non-zero memory
   fprintf(output, "Non-zero memory:\n");
   for(int i = 0; i < MEMORY_SIZE; i+=4){
       uint32_t block = load32(state->memory, i);
       if(block!=0){
           fprintf(output,"0x%08x : 0x%08x\n",i, block);
       }
   }
}


