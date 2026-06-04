#include "reader.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include <ctype.h>

#include "symtable.h"

#define MAX_LINE_LEN 1024
#define READ_OK 0
#define READ_FAIL -1

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
    }
    return count; 
}

extern int read_to_parse(
    FILE *input,
    Parser_Instruction *parsed_list, 
    Sym_Table *sym_table,
    uint64_t num_lines
){
    char* buffer = malloc(MAX_LINE_LEN * sizeof(char));
    for(uint64_t i = 0; i < num_lines; i++){
       if(fgets(buffer, MAX_LINE_LEN, input)==NULL){
        free(buffer);
        return READ_FAIL; 
       }  
       size_t len = strlen(buffer);
       if(buffer[len-1]!='\n') {
        fprintf(stderr, "Could not read line. Buffer overrun.");
        free(buffer);
        return READ_FAIL; 
       }
       buffer[len-1]='\0'; 
       if (parser(buffer, sym_table , parsed_list+i)!=0){
        free(buffer);
        return READ_FAIL; 
       }
    }
    free(buffer);
    return READ_OK; 
}
