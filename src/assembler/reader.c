#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include <ctype.h>
#include <stdbool.h>
#include "parserIR.h"
#include "symtable.h"

#define MAX_LINE_LEN 1024
#define READ_OK 0
#define READ_FAIL -1

// Count number of lines with instructions, i.e. ignores blank lines
extern uint64_t num_lines(FILE* input){
    uint64_t count = 0;
    int ch = fgetc(input);
    bool isEmpty = true; 
    while(ch != EOF){
        if(ch == '\n' && !isEmpty){
            count++;
            isEmpty = true; 
        } else if(!isspace(ch)){
            isEmpty = false; 
        }
        ch = fgetc(input); 
        if(ch==EOF && !isEmpty){
            count++;
        }
    }
    return count; 
}

// Reads a line and passes it to instruction parse
extern int read_to_parse(
    FILE *input,
    Parser_Instruction *parsed_list, 
    Sym_Table *sym_table,
    uint64_t *num_instructions
){
    char* buffer = malloc(MAX_LINE_LEN * sizeof(char));
    uint64_t addr = 0; 
    uint64_t count = 0;
    while(fgets(buffer, MAX_LINE_LEN, input)!=NULL){
        size_t len = strlen(buffer);
        if(buffer[len-1]!='\n') {    
            fprintf(stderr, "Could not read line. Buffer too small.");
            free(buffer);
            return READ_FAIL; 
        }
        if(buffer[0]=='\n'){
            continue; 
        }
        buffer[len-1]='\0'; 
        if (parser(buffer, sym_table , parsed_list+count,addr)<0){
            free(buffer);
            return READ_FAIL; 
        }
        if(parsed_list[count].mnemonic != LABEL){
            addr+=4; 
        }
        count++; 
    }
    free(buffer);
    *num_instructions = count; 
    return READ_OK; 
}
