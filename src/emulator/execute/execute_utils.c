// #include "../memory.h"
#include <stdbool.h>
#include <stdint.h>
#include "../processor.h"
#include "execute_utils.h"

//Define update flags
void update_flags(CPU_state* state, uint64_t val1, uint64_t val2, uint64_t res, uint8_t sf, bool is_add) {
    //check if registers are 32 or 64 bit
    //Mask is either 32 or 64 bit. 
    //Bitwise & with result will ignore the last 32 bits if registers are set to 32
    uint64_t mask = sf ? 0xFFFFFFFFFFFFFFFFULL : 0xFFFFFFFFULL;
    res &= mask;

    uint64_t check_sign = sf ? (1ULL << 63) : (1ULL << 31);
    
    //Set N flag to signed bit of result
    state->pstate.N = (res & check_sign) != 0;

    //Set Z flag to 1 if res == 0
    state->pstate.Z = res == 0;

    //Set C flag
    if (is_add) {
        //if there is an overflow, set C to 1
        uint64_t sum = (val1 + val2) & mask;
        state->pstate.C = sum < (val1 & mask);
    } 
    else {
        //if there is an underflow, set C to 0
        state->pstate.C = (val1 & mask) >= (val2 & mask);
    }

    //Set V flag to 1 if there is signed overflow
    //s1, s2 and res are 1 if negative, 0 if positive
    int64_t s1 = (val1 & check_sign) != 0; 
    int64_t s2 = (val2 & check_sign) != 0;
    int64_t sr = (res & check_sign) != 0;

    if (is_add) {
        state->pstate.V =
        //If both arguments differ in sign from the result
        (s1 == s2) && (sr != s1);
    } else {
        state->pstate.V =
        //If both s1 is positive, s2 is negative but result is negative
        (s1 != s2) && (sr != s1);
    }
}
