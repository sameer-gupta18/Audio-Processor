#ifndef OUTPUT_H
#define OUTPUT_H
#include "assembleIR.h"
#include <stdio.h>

void output(FILE* output, Assembled_Instruction* assembled_list, size_t count);

static void write_word_le(FILE* output, uint32_t word);

#endif
