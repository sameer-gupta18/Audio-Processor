#ifndef OUTPUT_H
#define OUTPUT_H
#include "processor.h"

extern int output(CPU_state* state);
extern int output_with_file(CPU_state* state, const char* filename);

#endif
