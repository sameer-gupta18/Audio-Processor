#include "load.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int load_instructions(CPU_state* state, const char* bin_file) {
    FILE *file = fopen(bin_file, "rb");

    if (file == NULL) {
        perror("Error opening file.");
        return EXIT_FAILURE;
    }

    fseek(file, 0,  SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size % 4 != 0) {
        fprintf(stderr, "Error: file size (%ld bytes) is not a multiple of 4\n", file_size);
        fclose(file);
        return EXIT_FAILURE;
    }

    if (file_size > MEMORY_SIZE) {
        fprintf(stderr, "Binary file too large for memory (%ld > %ld)\n", file_size, (long)MEMORY_SIZE);
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(state->memory, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: expected %ld bytes, read %zu\n", file_size, bytes_read);
        fclose(file);
        return EXIT_FAILURE;
    }


    fclose(file);
    return EXIT_SUCCESS;
}
