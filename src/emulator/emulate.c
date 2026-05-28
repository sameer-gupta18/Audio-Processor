#include <stdlib.h>
#include <stdio.h>
#include "load.h"
#include "memory.h"
#include "processor.h"
#include "output.h"
#define PC_OFFSET 4


int main(int argc, char **argv) {
   // Checking arguments
   if (argc != 3){
     fprintf(stderr, "Format: emulate {name}.bin {output}.out (optional).\n");
     return EXIT_FAILURE;
   }
   // Setting program IO
   const char* input_file = argv[1];
   FILE* out = stdout;
   if (argc >= 3){
     out = fopen(argv[2], "w");
     if(out == NULL){
       fprintf(stderr, "Could not open file %s\n", argv[2]);
       return EXIT_FAILURE;
     }
   }
  
   // Settting-up main memory
   Sys_Memory *memory = calloc(1, sizeof(Sys_Memory));
   if(memory == NULL){
     fprintf(stderr, "Could not allocate memory.\n");
     return EXIT_FAILURE;
   }


   CPU_state state = state_init(memory);


   // Load instructions
   if (load_instructions(&state,input_file)!=0){
     return EXIT_FAILURE;
   };
   uint32_t curr_instr = instruction_fetch(&state);
   int curr_status = instruction_decode(&state, curr_instr);
   while(curr_status != DECODE_HALT){
     if(curr_status == DECODE_FAIL){
       return EXIT_FAILURE;
     }
     if(curr_status != DECODE_BRANCH){
       state.pc += PC_OFFSET;
     }
     curr_instr = instruction_fetch(&state);
     curr_status = instruction_decode(&state, curr_instr);
   }
  
   // Printing state and closing program
   output(&state, out);
   if (out!=stdout){
     fclose(out);
   }
   free(memory);
   return EXIT_SUCCESS;
}
