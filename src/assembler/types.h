#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#define MB (int32_t)(1<<20)
#define XZR 31
#define INSTRUCTION_BYTES 4

typedef enum {EQ=0, NE = 1, GE = 10, LT = 11, GT = 12, LE = 13, AL = 14} conds;

#endif
