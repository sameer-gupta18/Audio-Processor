#include "output.h"
#include <stdio.h>

void output(FILE* output, Assembled_Instruction* assembled_list, size_t count) {

    for (size_t i = 0; i < count; i++) {
        Assembled_Instruction *ins = &assembled_list[i];

        uint32_t word = 0;

    }

}

static void write_word_le(FILE* output, uint32_t word) {
    unsigned char bytes[4];
    bytes[0] = word & 0xFF;
    bytes[1] = (word >> 8) & 0xFF;
    bytes[2] = (word >> 16) & 0xFF;
    bytes[3] = (word >> 24) & 0xFF;
    fwrite(bytes, 1, 4, output);
}