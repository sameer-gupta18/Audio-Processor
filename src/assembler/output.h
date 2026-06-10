#ifndef OUTPUT_H
#define OUTPUT_H
#include "assembleIR.h"
#include <stdio.h>

extern void output(FILE* output, Assembled_Instruction* assembled_list, size_t count);

#endif
