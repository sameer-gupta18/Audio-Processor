#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>
#include <stdbool.h>
#define MEMORY_SIZE (1 << 21)

typedef struct{
    uint8_t data[MEMORY_SIZE];
} Sys_Memory;

#endif
