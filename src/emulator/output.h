#ifndef OUTPUT_H
#define OUTPUT_H
#include "processor.h"

extern int output(CPU_State* state);
extern int output_with_file(CPU_State* state, const char* filename);

#endif