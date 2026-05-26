// #include "../memory.h"
#include <stdbool.h>
#include <stdint.h>
#include "../processor.h"
#include "../utils.h"
#include "execute_utils.h"

//Define update flags
int update_flags(CPU_state* state, uint64_t val1, uint64_t val2, uint64_t res, uint8_t sf, bool is_add) {
    //check if regsiters are 32 or 64 bit
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
        //If there is an overflow, the set C flag to 1
        state->pstate.C = (val1 + val2) < val1;
    } else {
        //If there is an underflow, set C flag to 0
        state->pstate.C = val1 >= val2;
    }

    //Set V flag to 1 if there is signed overflow
    //Initialise signed variables for val1, 2 and result
    int64_t s1 = (int64_t)val1;
    int64_t s2 = (int64_t)val2;
    int64_t sr = (int64_t)res;

    if (is_add) {
        state->pstate.V =
        //If both arguments are positive but result is negative
        ((s1 > 0 && s2 > 0 && sr < 0) ||
        //If both arguments are negative, but result is positive
        (s1 < 0 && s2 < 0 && sr > 0));
    } else {
        //If both s1 is positive, s2 is negative but result is negative
        ((s1 > 0 && s2 < 0 && sr < 0) ||
        //If both s1 is negative, s2 is positive but result is positive
        (s1 < 0 && s2 > 0 && sr > 0));
    }
    return 0;
}