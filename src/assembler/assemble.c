#include <stdlib.h>
#include <stdio.h>
#include "encode.h"
#include "output.h"
#include "parserIR.h"
#include "reader.h"
#include "symtable.h"
#define INSTRUCTION_LIST_BUFFER 100
#define ALLOCATE_IR_LIST(IR) calloc(num_instructions+INSTRUCTION_LIST_BUFFER, sizeof(IR))

// Cleanup if exit
void cleanup(
    Parser_Instruction* parsed_list,
    Assembled_Instruction* assembled_list, 
    Sym_Table* sym_table,
    FILE* input, 
    FILE* output
){
    free(parsed_list);
    free(assembled_list);
    symtable_free(sym_table); 
    fclose(input);
    fclose(output); 
}


int main(int argc, char **argv) {
    if(argc != 3){
        fprintf(stderr, "Format: ./assemble <file_in> <file_out>.");
        return EXIT_FAILURE; 
    }
    //Open input and output files
    FILE* input_file = fopen(argv[1], "r");
    if(input_file==NULL){
        fprintf(stderr, "Could not open file, %s", argv[1]);
        return EXIT_FAILURE;
    }
    FILE* output_file = fopen(argv[2],"wb");
    if(output_file==NULL){
        fprintf(stderr, "Could not open file, %s", argv[2]);
        fclose(input_file);
        return EXIT_FAILURE;
    }
    // Count number of instructions and rewind
    uint64_t num_instructions = num_lines(input_file);
    rewind(input_file);
    // Allocate heap space for IR lists and symbol table

    Parser_Instruction* parsed_list = ALLOCATE_IR_LIST(Parser_Instruction);
    Assembled_Instruction* assembled_list = ALLOCATE_IR_LIST(Assembled_Instruction);
    Sym_Table* sym_table = symtable_init();
    
    // Read and parse instructions
    if(read_to_parse(input_file,  parsed_list, sym_table, num_instructions)!=0){
        cleanup(parsed_list, assembled_list, sym_table, input_file, output_file);
        return EXIT_FAILURE; 
    }
    //Encode instructions
    if (encode(sym_table, parsed_list, num_instructions, assembled_list, 0)!=0){
        cleanup(parsed_list, assembled_list, sym_table, input_file, output_file);
        return EXIT_FAILURE; 
    }
    //Output to binary file
    output(output_file, assembled_list);
    cleanup(parsed_list, assembled_list, sym_table, input_file, output_file);
    return EXIT_SUCCESS;
}
